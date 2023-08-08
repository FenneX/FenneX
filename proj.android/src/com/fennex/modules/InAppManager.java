/*
***************************************************************************
 * Copyright (c) 2013-2014 Auticiel SAS
 * <p>
 * http://www.fennex.org
 * <p>
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * <p>
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * <p>
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ****************************************************************************///

package com.fennex.modules;

import android.util.Log;

import com.android.billingclient.api.AcknowledgePurchaseParams;
import com.android.billingclient.api.AcknowledgePurchaseResponseListener;
import com.android.billingclient.api.BillingClient;
import com.android.billingclient.api.BillingClientStateListener;
import com.android.billingclient.api.BillingFlowParams;
import com.android.billingclient.api.BillingResult;
import com.android.billingclient.api.Purchase;
import com.android.billingclient.api.PurchasesResponseListener;
import com.android.billingclient.api.PurchasesUpdatedListener;
import com.android.billingclient.api.SkuDetails;
import com.android.billingclient.api.SkuDetailsParams;

import java.text.NumberFormat;
import java.text.ParseException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Locale;
import java.util.Objects;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import androidx.annotation.NonNull;

//Handle interface with Android BillingClient
//Doesn't handle consuming in-app purchases (not needed for subscriptions).
public class InAppManager implements
        PurchasesUpdatedListener /* onPurchasesUpdated */,
        PurchasesResponseListener /* onQueryPurchasesResponse */,
        AcknowledgePurchaseResponseListener /* onAcknowledgePurchaseResponse */ {
    private static final String TAG = "InAppManager";

    private static final String SERVICE_EVENT_TYPE = "Service";
    private static final String BUY_EVENT_TYPE = "Buy";
    private static final String RESTORE_EVENT_TYPE = "Restore";
    private static native void notifyFailure(String eventType, String code, String reason);
    private static native void notifySuccess(String eventType, String sku, String token, String orderId, boolean needAcknowledgment);
    private static native void notifyProductsInfosFetched(boolean success);

    //An instance is needed for listeners to be called
    private static volatile InAppManager instance = null;

    //The Android billing client that handle everything
    private static BillingClient billingClient = null;

    //If the purchases need to be queried once the billing client is initialized. Turned on at app creation and on resume
    private static boolean shouldQueryPurchases = true;

    //Which product SKU (identifiers) should be queried. Set by the app. Until they are set, purchases won't be queried
    private static List<String> skuListToQuery = new ArrayList<>();

    //The result of the last sku query.
    private static List<SkuDetails> skuDetailsList = null;

    //In-app queued for purchase
    private static String skuToBuy = null;
    private static boolean buyInProgress = false;

    //Bought in-apps to acknowledge
    private static String tokenToAcknowledge = null;

    public static void initialize() {
        //Called from C++ to ensure there is an initialization
        getInstance();
    }

    public static void queueQueryPurchases() {
        //Must be called on each resume to refresh purchases, in case the user got a purchase outside the app
        if(!shouldQueryPurchases) {
            restoreTransactions();
        }
    }

    //Called by native code
    @SuppressWarnings("unused")
    public static void buyProductIdentifier(final String productID) {
        boolean billingStarted = getInstance().isBillingStarted();
        if(!billingStarted || skuDetailsList == null) {
            if(skuToBuy != null) {
                notifyFailure(BUY_EVENT_TYPE, "NotInitialized", (!billingStarted ? "BillingClient" : "Products details") + "not initialized during buyProductIdentifier and there is already a waiting purchase");
            }
            else {
                skuToBuy = productID;
            }
            return;
        }
        SkuDetails skuDetails = null;
        for(SkuDetails details : skuDetailsList) {
            if(details.getSku().equals(productID)) {
                skuDetails = details;
            }
        }
        if(skuDetails == null) {
            notifyFailure(BUY_EVENT_TYPE, "UnknownSku", "Product ID " + productID + " not found during buyProductIdentifier");
            return;
        }
        if(skuToBuy != null) {
            notifyFailure(BUY_EVENT_TYPE, "InProgress", "There is already a queued product " + skuToBuy + " to buy.");
            return;
        }
        BillingFlowParams billingFlowParams = BillingFlowParams.newBuilder()
                .setSkuDetails(skuDetails)
                .build();
        BillingResult result = billingClient.launchBillingFlow(NativeUtility.getMainActivity(), billingFlowParams);
        int responseCode = result.getResponseCode();
        if (responseCode ==  BillingClient.BillingResponseCode.OK) {
            Log.i(TAG, "Launch billing flow successful, awaiting response.");
            buyInProgress = true;
        }
        else {
            buyInProgress = false;
            if (responseCode == BillingClient.BillingResponseCode.USER_CANCELED) {
                notifyFailure(BUY_EVENT_TYPE, "PayementCanceled", "Purchase cancelled by user during launchBillingFlow");
            }
            else if(responseCode == BillingClient.BillingResponseCode.SERVICE_DISCONNECTED) {
                skuToBuy = productID;
                getInstance().isBillingStarted(true);
            }
            else if(responseCode == BillingClient.BillingResponseCode.BILLING_UNAVAILABLE) {
                notifyFailure(BUY_EVENT_TYPE, "BillingUnavailable", "Play services are not available or no Google account is set up");
            }
            else if(responseCode == BillingClient.BillingResponseCode.ITEM_ALREADY_OWNED) {
                Log.w(TAG, "Product " + productID + " already owned, launching restore flow");
                restoreTransactions();
            }
            else {
            /* List of possible errors: https://stackoverflow.com/questions/68825055/what-result-codes-can-be-returned-from-billingclient-launchbillingflow
             * Errors not handled explicitly here:
                - DEVELOPER_ERROR => no need to handle, should only happen in dev if not properly setup
                - ERROR => duh, no info
                - FEATURE_NOT_SUPPORTED => looks like https://developer.android.com/reference/com/android/billingclient/api/BillingClient.FeatureType probably not something to handle
                - ITEM_NOT_OWNED => not possible here, only in consume flow, which we don't use
                - ITEM_UNAVAILABLE => no need to handle, should never happen
             */
                Log.e(TAG, "Error when launching billing flow, error code: " + responseCode + ", message: " + result.getDebugMessage());
                notifyFailure(BUY_EVENT_TYPE, "BillingFlowError", "Error "+ responseCode + " during launch billing flow: " + result.getDebugMessage());
            }
        }
    }

    //Called by native code
    @SuppressWarnings("unused")
    public static void acknowledgePurchase(String token) {
        if(getInstance().isBillingStarted()) {
            AcknowledgePurchaseParams acknowledgePurchaseParams =
                    AcknowledgePurchaseParams.newBuilder()
                            .setPurchaseToken(token)
                            .build();
            billingClient.acknowledgePurchase(acknowledgePurchaseParams, getInstance());
        }
        else {
            tokenToAcknowledge = token;
        }
    }

    //Called by native code
    @SuppressWarnings("unused")
    public static void restoreTransactions() {
        if(getInstance().isBillingStarted()) {
            Log.i(TAG, "Querying subs purchases");
            billingClient.queryPurchasesAsync(BillingClient.SkuType.SUBS, getInstance());
        }
        else {
            Log.i(TAG, "Queuing query purchases request");
            shouldQueryPurchases = true;
        }
    }

    //Called by native code
    @SuppressWarnings("unused")
    public static void requestProductsData(String[] skuList) {
        getInstance().requestProductsData(new ArrayList<>(Arrays.asList(skuList)));
    }

    private void requestProductsData(List<String> skuList) {
        //Actual requesting of product data when both the setup is finished and the app provided the list of SKUs to query
        if(!isBillingStarted()) {
            skuListToQuery = skuList;
            return;
        }
        for(String sku : skuList) {
            Log.i(TAG, "Requesting infos for " + sku);
        }
        SkuDetailsParams.Builder params = SkuDetailsParams.newBuilder();
        params.setSkusList(skuList).setType(BillingClient.SkuType.SUBS);
        billingClient.querySkuDetailsAsync(params.build(), (result, list) -> {
            skuDetailsList = list;
            Log.i(TAG, "Got " + Objects.requireNonNull(list).size() + " sku details, billing response was: " + result.getResponseCode() + ": " + result.getDebugMessage());
            for(SkuDetails details : skuDetailsList) {
                Log.i(TAG, "  " + details.getSku() + ": " + details.getTitle() + ", " + details.getPrice());
            }
            NativeUtility.getMainActivity().runOnGLThread(() -> notifyProductsInfosFetched(result.getResponseCode() == BillingClient.BillingResponseCode.OK));
        });
    }

    //Called by native code
    @SuppressWarnings("unused")
    public static String[] getProductsIds() {
        if(skuDetailsList == null) return new String[] {};
        List<String> skuIDsList = new ArrayList<>();
        for(SkuDetails sku : skuDetailsList) {
            skuIDsList.add(sku.getSku());
        }
        return skuIDsList.toArray(new String[] {});

    }

    //Called by native code
    @SuppressWarnings("unused")
    public static String[] getProductsInfos(String productId) {
        Log.i(TAG, "Returning product infos for: " + productId);
        if(skuDetailsList == null) return new String[] {};
        for(SkuDetails details : skuDetailsList) {
            if(productId.equals(details.getSku())) {
                //Note: very old code from 2015 that was taken as-is during upgrade to Billing V3 library, since SkuDetails is unchanged.
                Log.i("InAppManager", "Price String : " + details.getPrice() + ", type : " + details.getType());
                NumberFormat currencyFormatter = NumberFormat.getCurrencyInstance();
                Number price = null;
                // This is a simple tricks to work around the currency bug. To keep the same format between the price and the price per unit
                boolean needReplace = false;
                String priceString = details.getPrice();
                try {
                    price = currencyFormatter.parse(details.getPrice());
                    Log.i("InAppManager", "Price : " + (price != null ? price.toString() : "null") + " for " + productId);
                } catch (ParseException e) {
                    e.printStackTrace();
                    // There is a bug in Java see here : https://stackoverflow.com/questions/15586099/numberformat-parse-fails-for-some-currency-strings
                    // So we have to extract the price an other way :
                    try {
                        // Regex to extract double
                        String regex ="(-)?(([^\\\\d])(0)|[1-9]\\d*)(.)(\\d+)";
                        Matcher matcher = Pattern.compile( regex ).matcher(details.getPrice());
                        if(matcher.find()) {
                            priceString = matcher.group();
                            needReplace = true;
                            price = Double.valueOf(priceString);
                        }
                    }
                    catch (Exception scannerException) {
                        scannerException.printStackTrace();
                    }
                }
                String pricePerUnit = null;
                int unitsNumber = 1;
                if (price != null) {
                    int lastNumberIndex = productId.length();
                    while (lastNumberIndex > 0 && productId.charAt(lastNumberIndex - 1) >= '0' && productId.charAt(lastNumberIndex - 1) <= '9') {
                        lastNumberIndex--;
                    }
                    if (lastNumberIndex < productId.length()) {
                        unitsNumber = Integer.parseInt(productId.substring(lastNumberIndex));
                        if (unitsNumber > 0) {
                            if(needReplace) {
                                //Use US locale to ensure C++ code will be able to read the float regardless of user locale
                                pricePerUnit = details.getPrice().replace(priceString, String.format(Locale.US, "%.2f", price.doubleValue() / unitsNumber));
                            }
                            else {
                                pricePerUnit = currencyFormatter.format(price.doubleValue() / unitsNumber);
                            }
                        }
                    }
                }

                return new String[]{
                        "Title", "[Str]" + details.getTitle(),
                        "Description", "[Str]" + details.getDescription(),
                        "Price", "[Flo]" + (price != null ? price.toString() : "0"),
                        "Identifier", "[Str]" + productId,
                        "Units", "[Int]" + unitsNumber,
                        "PriceString", "[Str]" + details.getPrice(),
                        "PricePerUnitString", (pricePerUnit != null ? "[Str]" + pricePerUnit : "")};
            }
        }
        return new String[] {};
    }

    public static InAppManager getInstance() {
        //There is an instance to handle Listeners and ensure there is always a single billingClient
        if (instance == null) {
            synchronized (InAppManager.class) {
                if (instance == null) {
                    instance = new InAppManager();
                }
            }
        }
        return instance;
    }

    //Make the constructor private and only use static variables
    private InAppManager() {
        //start the billing client
        isBillingStarted();
    }

    private boolean isBillingStarted() {
        return isBillingStarted(false);
    }

    private boolean isBillingStarted(boolean forceRestart) {
        //Billing client either don't exist or had an issue, start a new one
        if(billingClient == null || billingClient.getConnectionState() == BillingClient.ConnectionState.CLOSED || billingClient.getConnectionState() == BillingClient.ConnectionState.DISCONNECTED) {
            billingClient = BillingClient.newBuilder(NativeUtility.getMainActivity()).setListener(this).enablePendingPurchases().build();
        }
        if(!forceRestart) {
            // It's started, manager can use it
            if (billingClient.getConnectionState() == BillingClient.ConnectionState.CONNECTED)
                return true;

            // It's connecting already, nothing to do, but actions should be queue
            if (billingClient.getConnectionState() == BillingClient.ConnectionState.CONNECTING)
                return false;
        }

        billingClient.startConnection(new BillingClientStateListener() {
            @Override
            public void onBillingSetupFinished(@NonNull BillingResult result) {
                int responseCode = result.getResponseCode();
                if (responseCode == BillingClient.BillingResponseCode.OK) {
                    Log.i(TAG, "Billing setup finished, requesting products data...");
                    // BillingClient is initialized, execute pending queries
                    requestProductsData(skuListToQuery);
                    if(shouldQueryPurchases) {
                        Log.i(TAG, "Querying purchases...");
                        billingClient.queryPurchasesAsync(BillingClient.SkuType.SUBS, getInstance());
                    }
                    if(tokenToAcknowledge != null) {
                        Log.i(TAG, "Acknowledging token \"" + tokenToAcknowledge + "\"...");
                        AcknowledgePurchaseParams acknowledgePurchaseParams =
                                AcknowledgePurchaseParams.newBuilder()
                                        .setPurchaseToken(tokenToAcknowledge)
                                        .build();
                        billingClient.acknowledgePurchase(acknowledgePurchaseParams, getInstance());
                        tokenToAcknowledge = null;
                    }
                }
                else if(responseCode == BillingClient.BillingResponseCode.SERVICE_DISCONNECTED) {
                    Log.i(TAG, "Service disconnected, restarting it.");
                    isBillingStarted(true);
                }
                else if(responseCode == BillingClient.BillingResponseCode.BILLING_UNAVAILABLE) {
                    Log.e(TAG, "Error during billing client setup, billing is unavailable");
                    NativeUtility.getMainActivity().runOnGLThread(() -> notifyFailure(SERVICE_EVENT_TYPE, "BillingUnavailable", "Play services are not available or no Google account is set up"));
                }
                else {
                    Log.e(TAG, "Error during billing client setup, error code: " + responseCode + ", message: " + result.getDebugMessage());
                    NativeUtility.getMainActivity().runOnGLThread(() -> notifyFailure(SERVICE_EVENT_TYPE, "BillingSetupFailure", "Error " + responseCode + " during onBillingSetupFinished: " + result.getDebugMessage()));
                }
            }

            @Override
            public void onBillingServiceDisconnected() {
                //Try to restart the service immediately, no need to wait.
                isBillingStarted();
            }
        });
        return false;
    }

    void handlePurchase(Purchase purchase, boolean buy) {
        //Note: consuming an in-app purchase is currently not supported
        if(purchase.getPurchaseState() == Purchase.PurchaseState.PURCHASED) {
            NativeUtility.getMainActivity().runOnGLThread(() -> {
                for (String sku : purchase.getSkus()) {
                    //It's up to the native code to call acknowledgePurchase once server verification is done.
                    notifySuccess(buy ? BUY_EVENT_TYPE : RESTORE_EVENT_TYPE, sku, purchase.getPurchaseToken(), purchase.getOrderId(), !purchase.isAcknowledged());
                }
            });
        }
        //Right now, UNSPECIFIED_STATE and PENDING purchases are not explicitly handled.
        //Google documentation states that "Additional forms of payment are not available for subscriptions purchases." which means no pending transactions
        else {
            for (String sku : purchase.getSkus()) {
                Log.i(TAG, "Purchase in " + (purchase.getPurchaseState() == Purchase.PurchaseState.PENDING ? "PENDING" : "UNSPECIFIED_STATE") + "state for sku " + sku + " and order " + purchase.getOrderId() + ", ignoring it");
            }
        }
    }

    @Override
    public void onPurchasesUpdated(BillingResult result, List<Purchase> purchases) {
        Log.i(TAG, "On Purchases updated, purchases is " + (purchases == null ? "null" : String.valueOf(purchases.size())));
        int responseCode = result.getResponseCode();
        if (responseCode == BillingClient.BillingResponseCode.OK && purchases != null) {
            for (Purchase purchase : purchases) {
                handlePurchase(purchase, buyInProgress);
            }
        }
        else {
            NativeUtility.getMainActivity().runOnGLThread(() -> {
                if (responseCode == BillingClient.BillingResponseCode.USER_CANCELED) {
                    notifyFailure(buyInProgress ? BUY_EVENT_TYPE : RESTORE_EVENT_TYPE, "PayementCanceled", "Purchase cancelled by user during onPurchasesUpdated");
                } else if (responseCode == BillingClient.BillingResponseCode.SERVICE_DISCONNECTED) {
                    notifyFailure(buyInProgress ? BUY_EVENT_TYPE : RESTORE_EVENT_TYPE, "ServiceDisconnected", "Service disconnected during request, please retry");
                } else if (responseCode == BillingClient.BillingResponseCode.BILLING_UNAVAILABLE) {
                    notifyFailure(buyInProgress ? BUY_EVENT_TYPE : RESTORE_EVENT_TYPE, "BillingUnavailable", "Play services are not available or no Google account is set up");
                } else {
            /* List of possible errors: https://stackoverflow.com/questions/68825055/what-result-codes-can-be-returned-from-billingclient-launchbillingflow
             * Errors not handled explicitly here:
                - DEVELOPER_ERROR => no need to handle, should only happen in dev if not properly setup
                - ERROR => duh, no info
                - FEATURE_NOT_SUPPORTED => looks like https://developer.android.com/reference/com/android/billingclient/api/BillingClient.FeatureType probably not something to handle
                - ITEM_NOT_OWNED => not possible here, only in consume flow, which we don't use
                - ITEM_ALREADY_OWNED => not possible here, it should be launched before
                - ITEM_UNAVAILABLE => no need to handle, should never happen
             */
                    Log.e(TAG, "Error during onPurchasesUpdated, error code: " + responseCode + ", message: " + result.getDebugMessage());
                    notifyFailure(buyInProgress ? BUY_EVENT_TYPE : RESTORE_EVENT_TYPE, "PurchaseUpdateFailure", "Error " + responseCode + " during onPurchasesUpdated: " + result.getDebugMessage());
                }
            });
        }
        buyInProgress = false;
    }

    @Override
    public void onQueryPurchasesResponse(@NonNull BillingResult result, @NonNull List<Purchase> list) {
        Log.i(TAG, "Got purchases responses with " + list.size() + " purchases.");
        int responseCode = result.getResponseCode();
        shouldQueryPurchases = false;
        for (Purchase purchase : list) {
            handlePurchase(purchase, false);
        }
        if (responseCode == BillingClient.BillingResponseCode.OK) {
            if(list.isEmpty()) {
                NativeUtility.getMainActivity().runOnGLThread(() -> notifyFailure(buyInProgress ? BUY_EVENT_TYPE : RESTORE_EVENT_TYPE, "NoPurchases", "No purchases found during query."));
            }
        }
        else {
            if(responseCode == BillingClient.BillingResponseCode.SERVICE_DISCONNECTED) {
                shouldQueryPurchases = true;
                isBillingStarted(true);
            }
            else {
                Log.e(TAG, "Could not query purchases, error code "+ responseCode + ", details: " + result.getDebugMessage());
                if(!buyInProgress) {
                    NativeUtility.getMainActivity().runOnGLThread(() -> {
                        //Send a signal so that if the user is waiting for restore, he can be alerted something went wrong
                        notifyFailure(RESTORE_EVENT_TYPE, "BillingFlowError", "Could not query purchases, error code " + responseCode + ", details: " + result.getDebugMessage());
                    });
                }
            }
        }
    }

    @Override
    public void onAcknowledgePurchaseResponse(@NonNull BillingResult result) {
        int responseCode = result.getResponseCode();
        if(responseCode == BillingClient.BillingResponseCode.OK) {
            Log.i(TAG, "Token acknowledgment successful");
        }
        else { //There isn't a lot of things to be done: acknowledgement isn't a user-facing feature, and there is little point retrying.
            Log.e(TAG, "Error acknowledging purchase, error code "+ responseCode + ", details: " + result.getDebugMessage());
        }
    }
}
