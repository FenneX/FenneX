/****************************************************************************
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

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.util.Log;
import android.widget.Toast;

import com.android.vending.billing.util.IabHelper;
import com.android.vending.billing.util.IabHelper.OnConsumeFinishedListener;
import com.android.vending.billing.util.IabResult;
import com.android.vending.billing.util.Inventory;
import com.android.vending.billing.util.Purchase;
import com.android.vending.billing.util.SkuDetails;

import java.text.NumberFormat;
import java.text.ParseException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

@SuppressWarnings({"WeakerAccess", "unused"})
public class InAppManager implements ActivityResultResponder {
    //The code is heavily copied from Trivial Drive Sample. This sample is installed at the same time as the In-app billing library.
    //TODO : construct public key using some native calls
    //TODO : handle refund correctly (currently not handled, despite code trying to)
    //TODO : module to handle consumable/not consumable in buyProductIdentifier ?
    //TODO : clean code
    //TODO : developer payload

    private static final String TAG = "InAppManager";

    // Does the user have the premium upgrade?
    //static boolean mIsPremium = false;

    // SKUs for our products: the premium upgrade (non-consumable)
    //replaced by productID provided by client app
    //static final String SKU_PREMIUM = "premium";

    // (arbitrary) request code for the purchase flow
    private static final int RC_REQUEST = 10102;

    private static IabHelper mHelper;

    private static Inventory mInventory = null;

    @SuppressWarnings("JniMissingFunction")
    private native void notifyInAppEvent(String name, String argument, String token, String reason);

    private static String payload = "test";

    private static List<String> skuToQuery;
    private static boolean queryFinished;

    private static volatile InAppManager instance = null;

    public static InAppManager getInstance() {
        if (instance == null) {
            synchronized (InAppManager.class) {
                if (instance == null) {
                    instance = new InAppManager();
                    NativeUtility.getMainActivity().addResponder(instance);
                    BroadcastReceiver promoReceiver = new BroadcastReceiver() {
                        @Override
                        public void onReceive(Context context, Intent intent) {
                            //Relaunch queryInventory workflow to check for new purchases
                            mHelper.queryInventoryAsync(mGotInventoryListener);
                        }
                    };
                    NativeUtility.getMainActivity().registerReceiver(promoReceiver, new IntentFilter("com.android.vending.billing.PURCHASES_UPDATED"));
                }
            }
        }
        return instance;
    }

    public void destroy() {
        instance = null;
    }

    public static void initialize() {
        InAppManager.getInstance(); //ensure the instance is created
        /* base64EncodedPublicKey should be YOUR APPLICATION'S PUBLIC KEY
         * (that you got from the Google Play developer console). This is not your
         * developer public key, it's the *app-specific* public key.
         *
         * Instead of just storing the entire literal string here embedded in the
         * program,  construct the key at runtime from pieces or
         * use bit manipulation (for example, XOR with some other string) to hide
         * the actual key.  The key itself is not secret information, but we don't
         * want to make it easy for an attacker to replace the public key with one
         * of their own and then fake messages from the server.
         */

        queryFinished = false;

        String base64EncodedPublicKey = NativeUtility.getMainActivity().getPublicKey();

        // Create the helper, passing it our context and the public key to verify signatures with
        Log.d(TAG, "Creating IAB helper.");
        mHelper = new IabHelper(NativeUtility.getMainActivity(), base64EncodedPublicKey);

        // enable debug logging (for a production application, you should set this to false).
        mHelper.enableDebugLogging(true);

        // Start setup. This is asynchronous and the specified listener
        // will be called once setup completes.
        Log.d(TAG, "Starting setup.");
        mHelper.startSetup(new IabHelper.OnIabSetupFinishedListener() {
            public void onIabSetupFinished(IabResult result) {
                Log.d(TAG, "Setup finished.");

                if (!result.isSuccess()) {
                    // Oh noes, there was a problem.
                    Log.e(TAG, "Problem setting up in-app billing: " + result);
                    return;
                }

                // Hooray, IAB is fully set up. Now, let's get an inventory of stuff we own.
                if (skuToQuery != null) {
                    Log.d(TAG, "Setup successful. Querying inventory with " + skuToQuery.size() + " SKUs:");
                    for(String sku : skuToQuery) {
                        Log.d(TAG, "    SKU: " + sku);
                    }
                    mHelper.queryInventoryAsync(true, skuToQuery, mGotInventoryListener);
                    skuToQuery = null;
                } else {
                    Log.d(TAG, "Setup successful. Querying inventory with no SKU");
                    mHelper.queryInventoryAsync(mGotInventoryListener);
                }
            }
        });
    }

    // User clicked the "Upgrade to Premium" button.
    public static void buyProductIdentifier(final String productID) {
        Log.d(TAG, "buying product identifier : " + productID);
        
        /* TODO: for security, generate your payload here for verification. See the comments on 
         *        verifyDeveloperPayload() for more info. Since this is a SAMPLE, we just use 
         *        an empty string, but on a production app you should carefully generate this. */
        final String payload = "test";

        NativeUtility.getMainActivity().runOnGLThread(new Runnable() {
            public void run() {
                if (mHelper == null || mInventory == null) {
                    Log.d(TAG, "Error : not initialized");
                    String event = "InAppSystemFailure";
                    String argument = "NotInitialized";
                    getInstance().notifyInAppEvent(event, argument, "", "InApps system not initialized during buyProductIdentifier");
                    return;
                } else {
                    Purchase existingPurchase = mInventory.getPurchase(productID);
                    //Purchase state = 0 means purchased. 1 is canceled (allow to re-buy), 2 is refunded (allow to re-buy)
                    //noinspection ConstantConditions
                    if (existingPurchase != null && verifyDeveloperPayload(existingPurchase) && existingPurchase.getPurchaseState() == 0) {
                        Log.d(TAG, "Restoring product");
                        getInstance().notifyInAppEvent("ProductRestored", productID, existingPurchase.getToken(), "Restore purchase successful using buyProductIdentifier, skipping purchase flow");
                        return;
                    }
                }
                if (mHelper != null) {
                    mHelper.flagEndAsync();
                }
                try {
                    Purchase purchase = mInventory.getPurchase(productID);
                    mHelper.launchPurchaseFlow(NativeUtility.getMainActivity(), productID, purchase != null ? purchase.getItemType() : IabHelper.ITEM_TYPE_INAPP, RC_REQUEST,
                                mPurchaseFinishedListener, payload);
                } catch (IllegalStateException e) {
                    Log.e("InAppManager", "Illegal state exception : " + e.getMessage());
                    Toast.makeText(NativeUtility.getMainActivity(), "Please retry in a few seconds.", Toast.LENGTH_SHORT).show();
                }
            }
        });
    }

    public static void restoreTransaction(final String productID) {
        Log.d(TAG, "Restore transactions");
        NativeUtility.getMainActivity().runOnGLThread(new Runnable() {
            public void run() {
                if (mHelper == null || mInventory == null) {
                    Log.d(TAG, "Error : not initialized");
                    String event = "InAppSystemFailure";
                    String argument = "NotInitialized";
                    getInstance().notifyInAppEvent(event, argument, "", "InApps system not initialized during restoreTransaction");
                } else {
                    Purchase existingPurchase = mInventory.getPurchase(productID);
                    //noinspection ConstantConditions
                    if (existingPurchase != null && verifyDeveloperPayload(existingPurchase) && existingPurchase.getPurchaseState() == 0) {
                        Log.d(TAG, "Restoring product");
                        getInstance().notifyInAppEvent("ProductRestored", productID, existingPurchase.getToken(), "Restore purchase successful using restoreTransaction");
                    } else {
                        String purchaseState = (existingPurchase == null ? "doesn't exist" :
                                existingPurchase.getPurchaseState() == 1 ? "canceled" :
                                        existingPurchase.getPurchaseState() == 2 ? "refunded" :
                                                "state " + existingPurchase.getPurchaseState());
                        getInstance().notifyInAppEvent("ErrorRestoreFailure", productID, existingPurchase == null ? "" : existingPurchase.getToken(), "Restore purchase failed during restoreTransaction, purchase state: " + purchaseState);
                    }
                }
            }
        });
    }

    public static void release() {
        // very important:
        Log.d(TAG, "Destroying helper.");
        if (mHelper != null) mHelper.dispose();
        mHelper = null;
    }

    //Return true if it uses the activity result is handled by the in-app module
    public boolean onActivityResult(int requestCode, int resultCode, Intent data) {
        Log.d(TAG, "onActivityResult(" + requestCode + "," + resultCode + "," + data);

        // Pass on the activity result to the helper for handling
        if (!mHelper.handleActivityResult(requestCode, resultCode, data)) {
            // not handled
            return false;
        } else {
            Log.d(TAG, "onActivityResult handled by IABUtil.");
        }
        return true;
    }

    // Listener that's called when we finish querying the items and subscriptions we own
    private static IabHelper.QueryInventoryFinishedListener mGotInventoryListener = new IabHelper.QueryInventoryFinishedListener() {
        public void onQueryInventoryFinished(IabResult result, Inventory inventory) {
            Log.d(TAG, "Query inventory finished.");
            if (result.isFailure()) {
                Log.e(TAG, "Failed to query inventory: " + result);
                return;
            }

            mInventory = inventory;
            Log.d(TAG, "Query inventory was successful.");
            for (Purchase purchase : mInventory.getAllPurchases()) {
                if (NativeUtility.getMainActivity().isConsumable(purchase.getSku())) {
                    Log.d(TAG, "Consuming purchase " + purchase.getSku());
                    mHelper.consumeAsync(purchase, mConsumeFinishedListener);
                }
            }
            if (skuToQuery != null) {
                mHelper.queryInventoryAsync(true, skuToQuery, mGotInventoryListener);
                skuToQuery = null;
            } else {
                queryFinished = true;
            }
        }
    };

    /** Verifies the developer payload of a purchase. */
    private static boolean verifyDeveloperPayload(Purchase p) {
        /*
         * TODO: verify that the developer payload of the purchase is correct. It will be
         * the same one that you sent when initiating the purchase.
         * 
         * WARNING: Locally generating a random string when starting a purchase and 
         * verifying it here might seem like a good approach, but this will fail in the 
         * case where the user purchases an item on one device and then uses your app on 
         * a different device, because on the other device you will not have access to the
         * random string you originally generated.
         *
         * So a good developer payload has these characteristics:
         * 
         * 1. If two different users purchase an item, the payload is different between them,
         *    so that one user's purchase can't be replayed to another user.
         * 
         * 2. The payload must be such that you can verify it even when the app wasn't the
         *    one who initiated the purchase flow (so that items purchased by the user on 
         *    one device work on other devices owned by the user).
         * 
         * Using your own server to store and verify developer payloads across app
         * installations is recommended.
         */
        //String payload = p.getDeveloperPayload();
        return true;
    }

    static void requestProductsData(String[] ids) {
        final List<String> list = new ArrayList<>();
        Collections.addAll(list, ids);
        if (queryFinished) {
            queryFinished = false;
            NativeUtility.getMainActivity().runOnUiThread(new Thread(new Runnable() {
                public void run() {
                    mHelper.queryInventoryAsync(true, list, mGotInventoryListener);
                }
            }));
        } else {
            if (skuToQuery == null) {
                skuToQuery = new ArrayList<>();
            }
            skuToQuery.addAll(list);
        }
    }

    static String[] getProductsIds() {
        if (mInventory == null) {
            Log.w(TAG, "mInventory is null, can't get products infos");
            return new String[0];
        }
        Object[] objectArray = mInventory.getAllSku().keySet().toArray();
        return Arrays.copyOf(objectArray, objectArray.length, String[].class);
    }

    static String[] getProductsInfos(String productId) {
        if (mInventory == null) {
            Log.w(TAG, "mInventory is null, can't get products infos");
            return new String[0];
        }
        SkuDetails details = mInventory.getAllSku().get(productId);
        Log.i("InAppManager", "Price String : " + details.getPrice());
        NumberFormat currencyFormatter = NumberFormat.getCurrencyInstance();
        Number price = null;
        try {
            price = currencyFormatter.parse(details.getPrice());
            Log.i("InAppManager", "Price : " + price.toString() + " for " + productId);
        } catch (ParseException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
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
                    pricePerUnit = currencyFormatter.format(price.doubleValue() / unitsNumber);
                }
            }
        }

        return new String[]{
                "Title", "[Str]" + details.getTitle(),
                "Description", "[Str]" + details.getDescription(),
                "Price", (price != null ? "[Flo]" + price.toString() : "[Flo]0"),
                "Identifier", "[Str]" + productId,
                "Units", "[Int]" + unitsNumber,
                "PriceString", "[Str]" + details.getPrice(),
                "PricePerUnitString", (pricePerUnit != null ? "[Str]" + pricePerUnit : "[Str]" + details.getPrice())};
    }

    private static String IABCodeToString(int iabCode) {
        switch (iabCode) {
            case IabHelper.BILLING_RESPONSE_RESULT_OK:
                return "Billing OK";
            case IabHelper.BILLING_RESPONSE_RESULT_USER_CANCELED:
                return "Billing user cancelled";
            case IabHelper.BILLING_RESPONSE_RESULT_BILLING_UNAVAILABLE:
                return "Billing unavailable";
            case IabHelper.BILLING_RESPONSE_RESULT_ITEM_UNAVAILABLE:
                return "Billing item unavailable";
            case IabHelper.BILLING_RESPONSE_RESULT_DEVELOPER_ERROR:
                return "Billing developer error";
            case IabHelper.BILLING_RESPONSE_RESULT_ERROR:
                return "Billing unkown error";
            case IabHelper.BILLING_RESPONSE_RESULT_ITEM_ALREADY_OWNED:
                return "Billing item already owned";
            case IabHelper.BILLING_RESPONSE_RESULT_ITEM_NOT_OWNED:
                return "Billing item not owned";

            case IabHelper.IABHELPER_ERROR_BASE:
                return "IAB error base";
            case IabHelper.IABHELPER_REMOTE_EXCEPTION:
                return "IAB remote exception";
            case IabHelper.IABHELPER_BAD_RESPONSE:
                return "IAB bad response";
            case IabHelper.IABHELPER_VERIFICATION_FAILED:
                return "IAB verification failed";
            case IabHelper.IABHELPER_SEND_INTENT_FAILED:
                return "IAB send intent failed";
            case IabHelper.IABHELPER_USER_CANCELLED:
                return "IAB user cancelled";
            case IabHelper.IABHELPER_UNKNOWN_PURCHASE_RESPONSE:
                return "IAB unknown purchase response";
            case IabHelper.IABHELPER_MISSING_TOKEN:
                return "IAB missing token";
            case IabHelper.IABHELPER_UNKNOWN_ERROR:
                return "IAB unkown error";
            case IabHelper.IABHELPER_SUBSCRIPTIONS_NOT_AVAILABLE:
                return "IAB subscriptions not available";
            case IabHelper.IABHELPER_INVALID_CONSUMPTION:
                return "IAB invalid consumption";
        }
        return "Unknown response code";
    }

    // Callback for when a purchase is finished
    static IabHelper.OnIabPurchaseFinishedListener mPurchaseFinishedListener = new IabHelper.OnIabPurchaseFinishedListener() {
        public void onIabPurchaseFinished(final IabResult result, final Purchase purchase) {
            Log.d(TAG, "Purchase finished: " + result + ", purchase: " + purchase);

            NativeUtility.getMainActivity().runOnGLThread(new Runnable() {
                public void run() {
                    if (result.isFailure()) {
                        Log.e(TAG, "Error purchasing: " + result);
						/*If the activity is cancelled, we can't get IabHelper.IABHELPER_USER_CANCELLED, because it hides BILLING_RESPONSE_RESULT_ITEM_ALREADY_OWNED
						 We changed IabHelper code directly to show the actuall BILLING response code, so we need to handle all of them
						 Most of them default to ErrorTransactionFailure
						 */
                        if (result.getResponse() == IabHelper.BILLING_RESPONSE_RESULT_USER_CANCELED) {
                            getInstance().notifyInAppEvent("PayementCanceledTransactionFailure", "Failure", purchase.getToken(), "Purchase cancelled by user during purchaseFinished");
                        } else if (result.getResponse() == IabHelper.BILLING_RESPONSE_RESULT_ITEM_ALREADY_OWNED) {
                            getInstance().notifyInAppEvent("ProductRestored", purchase.getSku(), purchase.getToken(), "Restore purchase successful during purchaseFinished: item already owned");
                        } else if (result.getResponse() == IabHelper.IABHELPER_VERIFICATION_FAILED) {
                            getInstance().notifyInAppEvent("AuthenticityErrorTransactionFailure", "InvalidPayload", purchase.getToken(), "IABHelper verification failed during purchaseFinished");
                        } else {
                            getInstance().notifyInAppEvent("ErrorTransactionFailure", "Failure", purchase.getToken(), "Unhandled error during purchaseFinished: " + IABCodeToString(result.getResponse()) + ", code:" + result.getResponse());
                        }
                        return;
                    }
                    //noinspection ConstantConditions
                    if (!verifyDeveloperPayload(purchase)) {
                        Log.e(TAG, "Error purchasing. Authenticity verification failed.");
                        getInstance().notifyInAppEvent("AuthenticityErrorTransactionFailure", "InvalidPayload", purchase.getToken(), "Verify developer payload failed during purchaseFinished");
                        return;
                    }

                    if (purchase.getPurchaseState() == 1) //CANCELED
                    {
                        getInstance().notifyInAppEvent("PayementCanceledTransactionFailure", purchase.getSku(), purchase.getToken(), "Purchase cancelled by developer during purchaseFinished");
                        Log.d(TAG, "Purchase cancelled.");
                    } else if (purchase.getPurchaseState() == 2) //REFUNDED
                    {

                        getInstance().notifyInAppEvent("ProductRefunded", purchase.getSku(), purchase.getToken(), "Purchase refunded during purchaseFinished");
                        Log.d(TAG, "Purchase refunded.");
                    } else //PURCHASED, getPurchaseState should be 0
                    {
                        if (NativeUtility.getMainActivity().isConsumable(purchase.getSku())) {
                            Log.d(TAG, "Purchase successful, consuming it, sku : " + purchase.getSku());
                            NativeUtility.getMainActivity().runOnUiThread(new Runnable() {
                                public void run() {
                                    mHelper.consumeAsync(purchase, mConsumeFinishedListener);
                                }
                            });
                        } else {
                            Log.d(TAG, "Purchase successful, returning it");
                            getInstance().notifyInAppEvent("ProductPurchased", purchase.getSku(), purchase.getToken(), "Non-consumable purchase successful");
                        }
                    }
                }
            });
        }
    };

    static OnConsumeFinishedListener mConsumeFinishedListener = new IabHelper.OnConsumeFinishedListener() {
        public void onConsumeFinished(final Purchase purchase, final IabResult result) {
            Log.d(TAG, "Consume finished: " + result + ", purchase: " + purchase);

            NativeUtility.getMainActivity().runOnGLThread(new Runnable() {
                public void run() {
                    if (result.isFailure()) {
                        Log.e(TAG, "Error consuming: " + result);
                        if (result.getResponse() == IabHelper.IABHELPER_USER_CANCELLED) {
                            getInstance().notifyInAppEvent("PayementCanceledTransactionFailure", "Failure", purchase.getToken(), "Purchase cancelled by user during consumeFinished");
                        } else if (result.getResponse() == IabHelper.IABHELPER_VERIFICATION_FAILED) {
                            getInstance().notifyInAppEvent("AuthenticityErrorTransactionFailure", "InvalidPayload", purchase.getToken(), "IABHelper verification failed during consumeFinished");
                        } else {
                            getInstance().notifyInAppEvent("ErrorTransactionFailure", "Failure", purchase.getToken(), "Unhandled error during consumeFinished: " + IABCodeToString(result.getResponse()) + ", code:" + result.getResponse());
                        }
                        return;
                    }
                    //noinspection ConstantConditions
                    if (!verifyDeveloperPayload(purchase)) {
                        Log.e(TAG, "Error consuming. Authenticity verification failed.");
                        getInstance().notifyInAppEvent("AuthenticityErrorTransactionFailure", "InvalidPayload", purchase.getToken(), "Verify developer payload failed during consumeFinished");
                        return;
                    }

                    if (purchase.getPurchaseState() == 1) //CANCELED
                    {
                        getInstance().notifyInAppEvent("PayementCanceledTransactionFailure", purchase.getSku(), purchase.getToken(), "Purchase cancelled by developer during consumeFinished");
                        Log.d(TAG, "Consume canceled.");
                    } else if (purchase.getPurchaseState() == 2) //REFUNDED
                    {
                        getInstance().notifyInAppEvent("ProductRefunded", purchase.getSku(), purchase.getToken(), "Purchase refunded during consumeFinished");
                        Log.d(TAG, "Consume refunded.");
                    } else //PURCHASED, getPurchaseState should be 0
                    {
                        getInstance().notifyInAppEvent("ProductPurchased", purchase.getSku(), purchase.getToken(), "Consumable purchase successful");
                        Log.d(TAG, "Consume successful.");
                    }
                }
            });
        }
    };
}
