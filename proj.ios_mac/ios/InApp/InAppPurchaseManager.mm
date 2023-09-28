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

#import "InAppPurchaseManager.h"
#import "Reachability.h"
#import "NativeUtility.h"
#import "SKProduct+priceAsString.h"
#include "FenneX.h"
#include "FileLogger.h"
#import "NSString+RangeOfCharacters.h"

USING_NS_FENNEX;

static inline NSNumber* NSint(const int v)
{
	return [NSNumber numberWithInt:v];
}

@implementation InAppPurchaseManager

@synthesize productsInfos;
@synthesize receiptStatus;

static InAppPurchaseManager* _sharedManager = nil;

+ (InAppPurchaseManager*) sharedManager
{
	@synchronized([InAppPurchaseManager class])
	{
		if (!_sharedManager)
			[[self alloc] init];
		
		return _sharedManager;
	}
	
	return nil;
}

+ (id) alloc
{
	@synchronized([InAppPurchaseManager class])
	{
		NSAssert(_sharedManager == nil, @"Attempted to allocate a second instance of InAppPurchaseManager singleton.");
		_sharedManager = [super alloc];
		return _sharedManager;
	}
	
	return nil;
}

- (id) init 
{
	self = [super init];
	if (self != nil) 
	{
        [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
        productsRequest = nil;
        productsInfos = nil;
        receiptStatus = UnknownStatus;
        initAppleReceipt = [[NSString stringWithString:[self getAppleReceipt]] retain];
        NSLog(@"InAppPurchaseManager init, got apple receipt: %@", initAppleReceipt);
        [self queryReceiptData];
	}
	return self;
}

- (void)queryReceiptData
{
    //Note: it's not possible to know if a trial was used using SKPaymentTransaction. Thanks Apple...
    //Instead, use the deprecated (thanks Apple again) endpoint verifyReceipt to check if a receipt has a trial marker
    //This endpoint is supposed to be called server-side, but frankly for that info we do not care about a M2M
    //since in the end, Apple is checking if the user is eligible for free trial or not. We're just displaying info based on it.
    if([[self getAppleReceipt] length] == 0) {
        //There is no receipt, the user is eligible to free trial
        receiptStatus = Eligible;
        return;
    }
    NSString* appSecret = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"FenneXInAppManagerSecret"];
    if(appSecret == nil || [appSecret length] == 0)
    {
        NSLog(@"Cannot check for free trial eligibility since FenneXInAppManagerSecret is not configured in Info.plist");
        return;
    }
    //Bypass checks if it was already marked as already used.
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* receiptStatusPath = [[paths objectAtIndex:0] stringByAppendingPathComponent:@"receiptStatus.plist"];
    NSDictionary* receiptStatusFile = [NSDictionary dictionaryWithContentsOfFile:receiptStatusPath];
    if(receiptStatusFile != nil && [[receiptStatusFile objectForKey:@"receiptStatus"] isEqualToString:@"AlreadyUsed"]) {
        receiptStatus = AlreadyUsed;
        return;
    }

    //Code from https://gitshah.medium.com/how-to-do-ios-receipt-validation-in-objective-c-edfcbcdb79b2
    NSString* payload = [NSString stringWithFormat:@"{\"receipt-data\" : \"%@\", \"password\" : \"%@\"}",
                    [self getAppleReceipt], appSecret];

    NSData* payloadData = [payload dataUsingEncoding:NSUTF8StringEncoding];
    
#if DEBUG_ONLY
    NSURL* storeURL = [[[NSURL alloc] initWithString:@"https://sandbox.itunes.apple.com/verifyReceipt"] autorelease];
#else
    NSURL* storeURL = [[[NSURL alloc] initWithString:@"https://buy.itunes.apple.com/verifyReceipt"] autorelease];
#endif

    NSMutableURLRequest* storeRequest = [[[NSMutableURLRequest alloc] initWithURL:storeURL] autorelease];
    [storeRequest setHTTPMethod:@"POST"];
    [storeRequest setHTTPBody:payloadData];
    NSURLSession* session = [NSURLSession sessionWithConfiguration: [NSURLSessionConfiguration defaultSessionConfiguration]];
    auto task = [session dataTaskWithRequest:storeRequest completionHandler:^(NSData* _Nullable data, NSURLResponse* _Nullable response, NSError * _Nullable error) {
        if(error) {
            NSLog(@"We got the error while validating the receipt: %s", [[error description] UTF8String]);
            return;
        }
        NSError* localError = nil;
        //Parsing the response as JSON.
        NSDictionary* jsonResponse = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers error:&localError];

        //Getting the latest_receipt_info field value.
        NSArray* receiptInfo = jsonResponse[@"latest_receipt_info"];
        receiptStatus = Eligible;
        if(!receiptInfo || [receiptInfo count] == 0) {
            NSLog(@"%s", "Looks like this customer has no purchases!");
            return;
        }

        //Only check if a free trial has been used. All the other checks should be done server-side to avoid spoofing
        for (NSDictionary *lastReceipt in receiptInfo) {
            NSString* isTrialPeriod = lastReceipt[@"is_trial_period"];
            NSString* isInIntroOfferPeriod = lastReceipt[@"is_in_intro_offer_period"];
            if([isTrialPeriod isEqualToString:@"true"] || [isInIntroOfferPeriod isEqualToString:@"true"]) {
                receiptStatus = AlreadyUsed;
                //save that locally to avoid redoing the query each time when we already know the result
                [[NSDictionary dictionaryWithObjectsAndKeys:@"AlreadyUsed", @"receiptStatus", nil] writeToFile:receiptStatusPath atomically:YES];
            }
        }
        return;
    }];

    [task resume];
}

- (void)requestProductsData:(NSSet*)productIdentifiers;
{
    if(isConnected())
    {
        productsRequest = [[SKProductsRequest alloc] initWithProductIdentifiers:productIdentifiers];
        productsRequest.delegate = self;
        [productsRequest start];
    }
    else
    {
        NSLog(@"InAppPurchaseManager : not connected, can't fetch products infos");
    }
}

- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response
{
    if(productsInfos == nil)
    {
        productsInfos = [[NSMutableArray alloc] initWithArray:response.products];
    }
    else
    {
        [productsInfos addObjectsFromArray:response.products];
    }
    for(SKProduct* product in productsInfos)
    {
        NSLog(@"Product title: %@" , product.localizedTitle);
        NSLog(@"Product description: %@" , product.localizedDescription);
        NSLog(@"Product price: %@" , product.price);
        NSLog(@"Product id: %@" , product.productIdentifier);
        NSLog(@"Product locale identifier : %@" , product.priceLocale.localeIdentifier);
        NSLog(@"Product price as string: %@" , product.priceAsString);
        NSString* number = [product.productIdentifier substringFromSet:[NSCharacterSet decimalDigitCharacterSet]
                                                               options:NSBackwardsSearch|NSAnchoredSearch];
        NSLog(@"Product introductory price: %@" , product.introductoryPrice);
        if(number != nil)
        {
            NSLog(@"Product price per unit as string: %@", [product pricePerUnit:[number intValue]]);
        }
        NSLog(@"---");
    }
    BOOL invalid = false;
    for (NSString *invalidProductId in response.invalidProductIdentifiers)
    {
        NSLog(@"Invalid product id: %@" , invalidProductId);
        invalid = true;
    }
    NSLog(@"Receipt: %@" , [self getAppleReceipt]);
    
    // finally release the reqest we alloc/init’ed in requestProUpgradeProductData
    [productsRequest release];
    productsRequest = nil;
    
    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(invalid ? "FailFetchProductsInfos" : "ProductsInfosFetched");
}

- (void)request:(SKRequest *)request didFailWithError:(NSError *)error
{
    [productsRequest release];
    productsRequest = nil;
    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("FailFetchProductsInfos");
}

- (void)recordTransaction:(SKPaymentTransaction *)transaction {   
	NSLog(@"Recording transaction ...");
    //There should be a receipt now, query receipt data
    if([initAppleReceipt length] == 0 && [[self getAppleReceipt] length] != 0) {
        receiptStatus = UnknownStatus;
        [self queryReceiptData];
        [initAppleReceipt autorelease];
        initAppleReceipt = [[NSString stringWithString:[self getAppleReceipt]] retain];
        NSLog(@"InAppPurchaseManager re-querying receipt data with apple receipt: %@", initAppleReceipt);
    }
    // Optional: Record the transaction on the server side...    
}

- (void)completeTransaction:(SKPaymentTransaction *)transaction {
	
    NSLog(@"completeTransaction...");
	
    [self recordTransaction: transaction];	
	NSLog(@"Product purchased : %@", transaction.payment.productIdentifier);
    NSString* number = [transaction.payment.productIdentifier substringFromSet:[NSCharacterSet decimalDigitCharacterSet]
                                                           options:NSBackwardsSearch|NSAnchoredSearch];
    
    Value toSend = Value(ValueMap({
        {"ProductID", Value([transaction.payment.productIdentifier UTF8String])},
        {"Number", Value([number intValue])},
        {"PurchaseToken", Value([[self getAppleReceipt] UTF8String])},
        {"OrderId", Value("Unavailable on iOS")},
        {"NeedAcknowledgment", Value(false)},
    }));
    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("InAppBuySuccess", &toSend);
    [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
	
}

- (void)restoreTransaction:(SKPaymentTransaction *)transaction {
	
    NSLog(@"restoreTransaction...");
	
    [self recordTransaction: transaction];
	NSLog(@"Product restored : %@", transaction.originalTransaction.payment.productIdentifier);
    
    Value toSend = Value(ValueMap({
        {"ProductID", Value([transaction.payment.productIdentifier UTF8String])},
        {"PurchaseToken", Value([[self getAppleReceipt] UTF8String])},
        {"OrderId", Value("Unavailable on iOS")},
        {"NeedAcknowledgment", Value(false)},
    }));
    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("InAppRestoreSuccess", &toSend);
    [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
	
}

- (void)failedTransaction:(SKPaymentTransaction *)transaction {
    
    ValueMap infos = {{"ProductID", Value([transaction.payment.productIdentifier UTF8String])},
                      {"PurchaseToken", Value([[self getAppleReceipt] UTF8String])}};
    if (transaction.error.code != SKErrorPaymentCancelled)
    {        
        infos["Reason"] = Value(std::string("Transaction error: ") + [transaction.error.localizedDescription UTF8String] + ", code: " + std::to_string(transaction.error.code));
        Value toSend = Value(infos);
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("InAppBuyFailure",&toSend);
        NSLog(@"Transaction error: %@, code: %ld", transaction.error.localizedDescription, (long)transaction.error.code);
    }
	else 
	{
        infos["Reason"] = Value("Purchase cancelled by user");
        Value toSend = Value(infos);
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("PayementCanceledTransactionFailure", &toSend);
	}
	
    [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
	
}

const char* getPrintableTransactionState(SKPaymentTransactionState state)
{
    if(state == SKPaymentTransactionStatePurchasing) return "Purchasing";
    if(state == SKPaymentTransactionStatePurchased) return "Purchased";
    if(state == SKPaymentTransactionStateFailed) return "Failed";
    if(state == SKPaymentTransactionStateRestored) return "Restored";
    if(state == SKPaymentTransactionStateDeferred) return "Deferred";
    return "Unknown";
}

- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions
{
    bool hasSuccessfulTransactions = false;
    NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
    dateFormatter.dateFormat = @"HH:mm 'on' dd-MM-yyyy";
    for (SKPaymentTransaction *transaction in transactions)
    {
        if(transaction.transactionState == SKPaymentTransactionStatePurchased || transaction.transactionState == SKPaymentTransactionStateRestored)
        {
            hasSuccessfulTransactions = true;
        }
        const char* identifier = transaction.transactionIdentifier != nil ? [transaction.transactionIdentifier UTF8String] : "NULL";
        const char* productIdentifier = transaction.originalTransaction != nil ? [transaction.originalTransaction.payment.productIdentifier UTF8String] : "No original transaction";
        const char* date = transaction.transactionDate != nil ? [[dateFormatter stringFromDate:transaction.transactionDate] UTF8String] : "NULL";
        FileLogger::info("InAppPurchaseManager", string_format("Got transaction %s for product %s with state %s from %s", identifier, productIdentifier, getPrintableTransactionState(transaction.transactionState), date));
    }
    if(hasSuccessfulTransactions)
    {
        //Ignore states "failed" and "purchasing", send complete/restore signals
        for (SKPaymentTransaction *transaction in transactions)
        {
            if(transaction.transactionState == SKPaymentTransactionStatePurchased)
            {
                [self completeTransaction:transaction];
            }
            else if(transaction.transactionState == SKPaymentTransactionStateRestored)
            {
                [self restoreTransaction:transaction];
            }
        }
    }
    else
    {
        for (SKPaymentTransaction *transaction in transactions)
        {
            if(transaction.transactionState == SKPaymentTransactionStateFailed)
            {
                [self failedTransaction:transaction];
            }
            else if(transaction.transactionState == SKPaymentTransactionStatePurchasing)
            {
                NSLog(@"Transaction added to queue, purchasing ...");
            }
            else if(transaction.transactionState == SKPaymentTransactionStateDeferred)
            {
                NSLog(@"Transaction is deferred, awaiting external action.");
            }
        }
    }
}

- (void)paymentQueue:(SKPaymentQueue *)queue restoreCompletedTransactionsFailedWithError:(NSError *)error {
    ValueMap infos = {};
    if (error.code != SKErrorPaymentCancelled)
    {
        DelayedDispatcher::eventAfterDelay("InAppRestoreFailure", Value(ValueMap({{"Reason", Value(std::string("Restore error: ") + [error.localizedDescription UTF8String] + ", code: " + std::to_string(error.code))}})), 0.01);
        NSLog(@"Restore error: %@, code: %ld", error.localizedDescription, (long)error.code);
    }
    else
    {
        DelayedDispatcher::eventAfterDelay("PayementCanceledTransactionFailure", Value(ValueMap({{"Reason", Value("Purchase cancelled by user")}})), 0.01);
    }
}

- (void)paymentQueueRestoreCompletedTransactionsFinished:(SKPaymentQueue *)queue {
    bool hasSuccessfulTransactions = false;
    for (SKPaymentTransaction *transaction in queue.transactions)
    {
        if(transaction.transactionState == SKPaymentTransactionStatePurchased || transaction.transactionState == SKPaymentTransactionStateRestored)
        {
            hasSuccessfulTransactions = true;
        }
    }
    //No need to send an event if there is a successful transaction
    if(!hasSuccessfulTransactions)
    {
        DelayedDispatcher::eventAfterDelay("NoPurchasesFailure", Value(ValueMap({{"Reason", Value("No successful transaction found in payment queue")}})), 0.01);
    }
}

- (void)buyProductIdentifier:(NSString *)productIdentifier {
	
    if(productIdentifier != nullptr && productIdentifier.length > 0)
    {
        NSLog(@"Buying %@ ...", productIdentifier);
        
        if ([SKPaymentQueue canMakePayments])
        {
            if (!isConnected())
            {
                //[[NSNotificationCenter defaultCenter] postNotificationName:@"NoConnectionTransactionFailure" object:self userInfo:nil];
                
                Value toSend = Value(ValueMap({{"ProductID", Value([productIdentifier UTF8String])}, {"Reason", Value("No connection")}}));
                Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("NoConnectionTransactionFailure", &toSend);
            }
            else
            {
                SKProduct* targetProduct = nil;
                for(SKProduct* product in productsInfos)
                {
                    if([product.productIdentifier isEqualToString:productIdentifier])
                    {
                        targetProduct = product;
                    }
                }
                if(targetProduct != nil)
                {
                    SKPayment *payment = [SKPayment paymentWithProduct:targetProduct];
                    [[SKPaymentQueue defaultQueue] addPayment:payment];
                }
                else
                {
                    Value toSend = Value(ValueMap({{"ProductID", Value([productIdentifier UTF8String])}, {"Reason", Value("Product for identifier not found")}}));
                    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("InAppBuyFailure", &toSend);
                }
            }
        }
        else
        {
            Value toSend = Value(ValueMap({{"ProductID", Value([productIdentifier UTF8String])}, {"Reason", Value("Payement disabled on this device")}}));
            Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("CantPayTransactionFailure", &toSend);
        }
	}
    else
    {
        NSLog(@"Invalid product identifier");
        Value toSend = Value(ValueMap({{"ProductID", Value([productIdentifier UTF8String])}, {"Reason", Value("Invalid product identifier")}}));
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("InAppBuyFailure", &toSend);
    }
}

- (void)restoreTransactions
{
    [[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
}

- (void) dealloc
{
	[super dealloc];
}

- (NSString*) getAppleReceipt
{
    NSData* receipt = [NSData dataWithContentsOfURL:[[NSBundle mainBundle] appStoreReceiptURL]];
    return receipt == nullptr ? @"" : [receipt base64EncodedStringWithOptions:0];
}

@end
