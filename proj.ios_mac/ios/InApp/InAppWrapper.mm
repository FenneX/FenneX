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

#include "InAppWrapper.h"
#include "Shorteners.h"
#import "InAppPurchaseManager.h"
#import "SKProduct+priceAsString.h"
#import "NSString+RangeOfCharacters.h"

USING_NS_FENNEX;
USING_NS_CC;

void initializePayements()
{
    [InAppPurchaseManager sharedManager];
}

void inAppPurchaseProduct(const std::string& productID)
{
    [[InAppPurchaseManager sharedManager] buyProductIdentifier:[NSString stringWithFormat:@"%s", productID.c_str()]];
}


void restoreTransaction(const std::string& productID)
{
    [[InAppPurchaseManager sharedManager] restoreTransactions];
}

void releasePayements()
{
    
}

void requestProductsData(std::vector<std::string> products)
{
    NSMutableSet* set = [[NSMutableSet new] autorelease];
    for(std::string identifier : products)
    {
        [set addObject:[NSString stringWithUTF8String:identifier.c_str()]];
    }
    [[InAppPurchaseManager sharedManager] requestProductsData:set];
}

ValueMap getProductsInfos()
{
    NSArray* nsInfos = [InAppPurchaseManager sharedManager].productsInfos;
    if(nsInfos == nil)
    {
        return ValueMap();
    }
    ValueMap infos;
    for(SKProduct* product in nsInfos)
    {
        NSString* number = [product.productIdentifier substringFromSet:[NSCharacterSet decimalDigitCharacterSet]
                                                               options:NSBackwardsSearch|NSAnchoredSearch];
        infos.insert({[product.productIdentifier UTF8String], Value(ValueMap({
                {"Title", Value([product.localizedTitle UTF8String])},
                {"Description", Value([product.localizedDescription UTF8String])},
                {"Price", Value([product.price doubleValue])},
                {"Identifier", Value([product.productIdentifier UTF8String])},
                {"PriceString", Value([product.priceAsString UTF8String])},
                {"Number", Value(number != nil ? [number intValue] : 1)},
                {"PricePerUnitString", Value([[product pricePerUnit:number != nil ? [number intValue] : 1] UTF8String])},
        }))});
    }
    return infos;
}
