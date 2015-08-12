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
//On Android, don't forget to call InAppManager.onActivityResult from the main Activity and replace call to main Activity from InAppManager
//On Android, for licensing, READ_PHONE_STATE permission is required

//should be done at the launch of the app, so that it can try to continue existing payements
void initializePayements();

/* Will throw notifications for success :
 - ProductPurchased (iOS, Android) with argument ProductID (CCString corresponding to the one passed to purchaseProduct)
 - ProductRestored (iOS, Android) with argument ProductID (CCString corresponding to the one passed to purchaseProduct)
 - ProductRefunded (Android) with argument ProductID (CCString corresponding to the one passed to purchaseProduct) : client code have to remove corresponding in-app
 - LicenseStatusUpdate (Android debug) with argument Authorized (CCBool)
 
   Will throw notifications for failure : 
 - NoConnectionTransactionFailure (iOS) => app should prompt the user to connect and retry (on Android, the Store handles that notification)
 - CantPayTransactionFailure (iOS) => app should prompt the user to activate payement in settigns and retry
 - PayementCanceledTransactionFailure (iOS, Android) => app should confirm the transaction was cancelled
 - ErrorTransactionFailure (iOS, Android) => app should notify it failed and prompt to retry later
 - AuthenticityErrorTransactionFailure (Android) => cannot be thrown currently because there is no verifyPayload. App should notify the user there is an authenticity problem
 - InAppSystemFailure (Android) => app should prompt user to verify Play Store and retry
 */

//Big difference between iOS and Android : iOS handle consumable/not consumable, whereas Android do not, you have to manually consume consumable, and never consume not consumables
void inAppPurchaseProduct(const std::string& productID);
void restoreTransaction(const std::string& productID);

//should be done when the app exit for proper cleanup. Note : required for Android, not for iOS
void releasePayements();

//Request the products data, to be available for later use
void requestProductsData(CCArray* products);
/*
 Return a CCDictionary with each key being a product ID and values being a CCDictionary describing this productID :
 - Title (CCString)
 - Description (CCString)
 - Price (CCFloat) (will return 0 on Android if the formatter can't recognize it. iOS always have the correct price)
 - Identifier (CCString) (again for convenience)
 - PriceString (CCString) (localized price)
 - PricePerUnitString (CCString) (localized price per unit, it will be equal to Price if the Unit isn't found. The unit should be at the end of the productId)
 - Unit (CCInteger) default to 1 if the Unit isn't found
*/
CCDictionary* getProductsInfos();

/*
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
//This will be called once at first launch to check if there was a purchase on V1
bool checkNativePremium();
#endif*/

#endif
