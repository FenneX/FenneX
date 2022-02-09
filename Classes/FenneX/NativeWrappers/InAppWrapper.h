/****************************************************************************
Copyright (c) 2013-2014 Auticiel SAS

http://www.fennex.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************///

#ifndef FenneX_InAppWrapper_h
#define FenneX_InAppWrapper_h

#include "cocos2d.h"

USING_NS_CC;

//When integrating, don't forget to add StoreKit (for InApp) and SystemConfiguration (for Reachability) frameworks on iOS
//On Android, don't forget to call .InAppManager.queueQueryPurchases() from the main Activity onResume

//should be done at the launch of the app, so that it can try to continue existing payements
void initializePayements();

/*
 Will throw notifications for success :
 - InAppBuySuccess (iOS, Android) with argument ProductID, PurchaseToken, OrderId, NeedAcknowledgment
 - InAppRestoreSuccess (iOS, Android) with argument ProductID, PurchaseToken, OrderId, NeedAcknowledgment. If fields are empty, the restore was successful, but there is no purchase to be restored
 - LicenseStatusUpdate (Android debug) with argument Authorized (CCBool)

   Will throw notifications for failure : 
 - NoConnectionTransactionFailure (iOS, Android) => app should prompt the user to connect and retry (on Android, the Store handles that notification, but it's thrown if service is disconnected)
 - CantPayTransactionFailure (iOS, Android) => app should prompt the user to activate payement in settings and retry
 - PayementCanceledTransactionFailure (iOS, Android) => app should confirm the transaction was cancelled
 - NoPurchasesFailure (iOS, Android) => if in a restore flow, app should tell the user there is no purchase to be restored
 - InAppBuyFailure (iOS, Android) => app should notify it failed and prompt to retry later
 - InAppRestoreFailure (iOS, Android) => app should notify it failed and prompt to retry later
 - InAppServiceFailure (Android) => app should prompt user to verify connection and restart the app
 
 All error notifications contains:
 - Reason (a displayable reason to troubleshoot)
 */

//Consumables are not handled, only subscriptions
void inAppPurchaseProduct(const std::string& productID);

//Note that it's only implemented on Android, as iOS doesn't require acknowledging purchases
void acknowledgePurchase(const std::string& token);

void restoreTransactions();

/*
 Request the products data, to be available for later use
 Will throw events "FailFetchProductsInfos" or "ProductsInfosFetched" when receiving response
 */
void requestProductsData(std::vector<std::string> products);

/*
 Return a ValueMap with each key being a product ID and values being a ValueMap describing this productID :
 - Title (String)
 - Description (String)
 - Price (Double) (will return 0 on Android if the formatter can't recognize it. iOS always have the correct price)
 - Identifier (String) (again for convenience)
 - PriceString (String) (localized price)
 - PricePerUnitString (String) (localized price per unit, it will be equal to Price if the Unit isn't found. The unit should be at the end of the productId)
 - Unit (Integer) default to 1 if the Unit isn't found
*/
ValueMap getProductsInfos();

#endif
