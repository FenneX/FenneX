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
#import "NSString+RangeOfCharacters.h"

USING_NS_FENNEX;

static inline NSNumber* NSint(const int v)
{
	return [NSNumber numberWithInt:v];
}

@implementation InAppPurchaseManager

@synthesize productsInfos;

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
	}
	return self;
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
    // Optional: Record the transaction on the server side...    
}

- (void)completeTransaction:(SKPaymentTransaction *)transaction {
	
    NSLog(@"completeTransaction...");
	
    [self recordTransaction: transaction];	
	NSLog(@"Product purchased : %@", transaction.payment.productIdentifier);
    NSString* number = [transaction.payment.productIdentifier substringFromSet:[NSCharacterSet decimalDigitCharacterSet]
                                                           options:NSBackwardsSearch|NSAnchoredSearch];
    
    Value toSend = Value(ValueMap({{"ProductID", Value([transaction.payment.productIdentifier UTF8String])},
                                   {"Number", Value([number intValue])},
                                   {"PurchaseToken", Value([[self getAppleReceipt] UTF8String])},
                                   {"Reason", Value("Purchase successful")}}));
    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("ProductPurchased", &toSend);
    //[[NSNotificationCenter defaultCenter] postNotificationName:@"ProductPurchased" object:transaction.payment.productIdentifier];
    [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
	
}

- (void)restoreTransaction:(SKPaymentTransaction *)transaction {
	
    NSLog(@"restoreTransaction...");
	
    [self recordTransaction: transaction];
	NSLog(@"Product restored : %@", transaction.originalTransaction.payment.productIdentifier);
    
    Value toSend = Value(ValueMap({{"ProductID", Value([transaction.payment.productIdentifier UTF8String])},
        {"PurchaseToken", Value([[self getAppleReceipt] UTF8String])},
        {"Reason", Value("Restore purchase successful")}}));
    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("ProductRestored", &toSend);
    //[[NSNotificationCenter defaultCenter] postNotificationName:@"ProductRestored" object:transaction.originalTransaction.payment.productIdentifier];
    [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
	
}

- (void)failedTransaction:(SKPaymentTransaction *)transaction {
    
    ValueMap infos = {{"ProductID", Value([transaction.payment.productIdentifier UTF8String])},
                      {"PurchaseToken", Value([[self getAppleReceipt] UTF8String])}};
    if (transaction.error.code != SKErrorPaymentCancelled)
    {
		//[[NSNotificationCenter defaultCenter] postNotificationName:@"ErrorTransactionFailure" object:self userInfo:[NSDictionary dictionaryWithObject:transaction.error.localizedDescription forKey:@"Description"]];
        
        infos["Reason"] = Value(std::string("Transaction error: ") + [transaction.error.localizedDescription UTF8String] + ", code: " + std::to_string(transaction.error.code));
        Value toSend = Value(infos);
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("ErrorTransactionFailure",&toSend);
        NSLog(@"Transaction error: %@, code: %ld", transaction.error.localizedDescription, (long)transaction.error.code);
    }
	else 
	{
		//[[NSNotificationCenter defaultCenter] postNotificationName:@"PayementCanceledTransactionFailure" object:self userInfo:nil];
        infos["Reason"] = Value("Purchase cancelled by user");
        Value toSend = Value(infos);
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("PayementCanceledTransactionFailure", &toSend);
	}
	
    [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
	
}

- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions
{
    for (SKPaymentTransaction *transaction in transactions)
    {
        switch (transaction.transactionState)
        {
            case SKPaymentTransactionStatePurchased:
                [self completeTransaction:transaction];
                break;
            case SKPaymentTransactionStateFailed:
                [self failedTransaction:transaction];
                break;
            case SKPaymentTransactionStateRestored:
                [self restoreTransaction:transaction];
				break;
			case SKPaymentTransactionStatePurchasing:
				NSLog(@"Transaction added to queue, purchasing ...");
				break;
            default:
                break;
        }
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
                    Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("ErrorTransactionFailure", &toSend);
                }
            }
        }
        else
        {
            //[[NSNotificationCenter defaultCenter] postNotificationName:@"CantPayTransactionFailure" object:self userInfo:nil];
            Value toSend = Value(ValueMap({{"ProductID", Value([productIdentifier UTF8String])}, {"Reason", Value("Payement disabled on this device")}}));
            Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("CantPayTransactionFailure", &toSend);
        }
	}
    else
    {
        NSLog(@"Invalid product identifier");
		//[[NSNotificationCenter defaultCenter] postNotificationName:@"ErrorTransactionFailure" object:self userInfo:[NSDictionary dictionaryWithObject:@"Internal error" forKey:@"Description"]];
        
        Value toSend = Value(ValueMap({{"ProductID", Value([productIdentifier UTF8String])}, {"Reason", Value("Invalid product identifier")}}));
        Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("ErrorTransactionFailure", &toSend);
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
