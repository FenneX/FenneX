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

#import "SKProduct+freeTrial.h"
#import "InAppPurchaseManager.h"

@implementation SKProduct (freeTrial)

- (FreeTrialStatus) freeTrialStatus
{
    if(self.introductoryPrice == nil)
    {
        return NoOffer;
    }
    if([self.introductoryPrice.price compare:NSDecimalNumber.zero] != NSOrderedSame)
    {
        //The introductory price is not a free trial
        return NoOffer;
    }
    // Will return either Eligible, AlreadyUsed or UnknownStatus
    return [InAppPurchaseManager sharedManager].receiptStatus;
}
- (FreeTrialPeriod) freeTrialPeriod
{
    if(self.freeTrialStatus == Eligible)
    {
        switch(self.introductoryPrice.subscriptionPeriod.unit)
        {
            case SKProductPeriodUnitDay:
                return Day;
            case SKProductPeriodUnitWeek:
                return Week;
            case SKProductPeriodUnitMonth:
                return Month;
            case SKProductPeriodUnitYear:
                return Year;
        }
    }
    return UnknownPeriod;
}

- (int) freeTrialPeriodNumber
{
    if(self.freeTrialStatus == Eligible)
    {
        return self.introductoryPrice.numberOfPeriods;
    }
    return 0;
}

@end
