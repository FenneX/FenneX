//
//  DropDownListImplIOS.m
//  FenneX
//
//  Created by Pierre Bertinet on 21/09/15.
//
//

#import "DropDownListImplIOS.h"
#import "AppController.h"
#import "DropDownListWrapper.h"

#define IS_IOS8_OR_NEWER ([[[UIDevice currentDevice] systemVersion] floatValue] >= 8.0)

@implementation DropDownListImplIOS

- (id) init
{
    self = [super init];
    rowHeight = 40.0;
    if(self != nil)
    {
        CGRect screenRect = [[UIScreen mainScreen] bounds];
        CGFloat screenWidth = screenRect.size.width;
        CGFloat screenHeight = screenRect.size.height;
        
        currentOrientation = ((UIViewController*)[AppController sharedController].viewController).interfaceOrientation;
        
        UIViewController* rootVC = (UIViewController*)[AppController sharedController].viewController;
        
        // Take a quarter of a screen in dimension.
        if(!IS_IOS8_OR_NEWER)
        {
            pickerView = [[UIPickerView alloc] initWithFrame:CGRectMake(screenWidth/4, screenHeight/2, screenWidth/2, screenHeight/2)];
            pickerView.transform = CGAffineTransformMakeRotation(((UIViewController*)[AppController sharedController].viewController).interfaceOrientation == UIInterfaceOrientationLandscapeRight ? M_PI / 2 : -M_PI / 2);
        }
        else
        {
            pickerView = [[UIPickerView alloc] initWithFrame:CGRectMake(screenWidth/4, screenHeight/2, screenWidth/2, screenHeight/4)];
        }
        
        pickerView.center = rootVC.view.superview.center;
        [pickerView setDelegate:self];
        pickerView.dataSource = self;
        pickerView.backgroundColor = [UIColor whiteColor];
        [pickerView setAlpha:0.9f];
        
        // Add Title view
        titleView = [[UILabel alloc] initWithFrame: CGRectMake(screenWidth/4, screenHeight/2 - rowHeight, screenWidth/2, rowHeight)];
        if(IS_IOS8_OR_NEWER)
        {
            titleView.center = CGPointMake(rootVC.view.superview.center.x, rootVC.view.superview.center.y - pickerView.bounds.size.height/2 - rowHeight/2);
        }
        else
        {
            titleView.center = CGPointMake(rootVC.view.superview.center.x  + pickerView.bounds.size.height/2 + rowHeight/2, rootVC.view.superview.center.y);
            titleView.transform = CGAffineTransformMakeRotation(currentOrientation == UIInterfaceOrientationLandscapeRight ? M_PI / 2 : -M_PI / 2);
        }
        titleView.backgroundColor = [UIColor lightGrayColor];
        titleView.textColor = [UIColor whiteColor];
        [titleView setFont:[UIFont boldSystemFontOfSize:25]];
        titleView.textAlignment = NSTextAlignmentCenter;
        [titleView setAlpha:0.7f];
        
        // Make a black overlay on all screen behind to capt event and close
        background = [[UIImageView alloc] initWithFrame:CGRectMake(0, 0, screenWidth, screenHeight)];
        background.backgroundColor = [UIColor blackColor];
        [background setAlpha:0.6f];
        
        
        [rootVC.view.superview addSubview:background];
        [rootVC.view.superview addSubview:titleView];
        [rootVC.view.superview addSubview:pickerView];
        
        UITapGestureRecognizer *singleTap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(tapDetected)];
        singleTap.numberOfTapsRequired = 1;
        background.userInteractionEnabled = YES;
        [background addGestureRecognizer:singleTap];
        [self hide];
        _identifier = -1;
        
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(orientationChanged:) name:@"UIDeviceOrientationDidChangeNotification" object:nil];
    }
    return self;
}

- (void) orientationChanged:(NSNotification*)data
{ // Just in case
    if(titleView != nil && pickerView != nil && !IS_IOS8_OR_NEWER)
    {
        UIInterfaceOrientation orientation = ((UIViewController*)[AppController sharedController].viewController).interfaceOrientation;
        if(UIInterfaceOrientationIsLandscape(orientation) && orientation != currentOrientation)
        {
            CGAffineTransform transform = IS_IOS8_OR_NEWER ? CGAffineTransformIdentity : CGAffineTransformMakeRotation(orientation == UIInterfaceOrientationLandscapeRight ? M_PI / 2 : -M_PI / 2);
            
            [UIView transitionWithView:titleView duration:data == nil ? 0 : 0.5 options:UIViewAnimationOptionTransitionNone
                            animations:^{titleView.transform = transform;}
                            completion:NULL];
            [UIView transitionWithView:pickerView duration:data == nil ? 0 : 0.5 options:UIViewAnimationOptionTransitionNone
                            animations:^{pickerView.transform = transform;}
                            completion:NULL];
            
            currentOrientation = orientation;
        }
    }
}

- (void) show
{
    pickerView.hidden = false;
    background.hidden = false;
    titleView.hidden = false;
}

- (void) hide
{
    pickerView.hidden = true;
    background.hidden = true;
    titleView.hidden = true;
}

- (void) dealloc {
    [pickerView dealloc];
    [pickerValues dealloc];
    [super dealloc];
}

- (void) setPossibleValues:(NSArray*)values
{
    pickerValues = [[NSArray alloc] initWithArray: values];
    [pickerView reloadAllComponents];
}

- (void) setIdentifier:(int)identifier
{
    _identifier = identifier;
}

- (void) setTitle:(NSString*)title
{
    titleView.text = title;
}

// Fire when new picker values are selected
- (void)pickerView:(UIPickerView *)thePickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component {
    notifySelectionDone(_identifier, [[NSString stringWithFormat:@"%@",
                                       [pickerValues objectAtIndex:[thePickerView selectedRowInComponent:0]]] UTF8String]);
}

// Number of picker wheels in the picker
- (NSInteger)numberOfComponentsInPickerView:(UIPickerView *)pickerView {
    return 1;
}

// Number of items in each picker wheel
- (NSInteger)pickerView:(UIPickerView *)pickerView numberOfRowsInComponent:(NSInteger)component {
    return [pickerValues count];
}

// Title for Row #
- (NSString *)pickerView:(UIPickerView *)pickerView titleForRow:(NSInteger)row forComponent:(NSInteger)component {
    return [pickerValues objectAtIndex:row];
}


// Row height in pixels
- (CGFloat)pickerView:(UIPickerView *)pickerView rowHeightForComponent:(NSInteger)component {
    return rowHeight;
}

// Column width in pixels
- (CGFloat)pickerView:(UIPickerView *)pickerView widthForComponent:(NSInteger)component {
    CGRect screenRect = [[UIScreen mainScreen] bounds];
    return screenRect.size.width/2;
}

- (void)tapDetected {
    [self hide];
}

@end