// Copyright 2016 Google Inc. All Rights Reserved.

#ifndef FIREBASE_ANALYTICS_CLIENT_CPP_INCLUDE_FIREBASE_ANALYTICS_EVENT_NAMES_H_
#define FIREBASE_ANALYTICS_CLIENT_CPP_INCLUDE_FIREBASE_ANALYTICS_EVENT_NAMES_H_

/// @brief Namespace that encompasses all Firebase APIs.
namespace firebase {
/// @brief Firebase Analytics API.
namespace analytics {

/// @defgroup event_names Analytics Events
///
/// Predefined event names.
///
/// An Event is an important occurrence in your app that you want to
/// measure. You can report up to 500 different types of Events per app
/// and you can associate up to 25 unique parameters with each Event type.
/// Some common events are suggested below, but you may also choose to
/// specify custom Event types that are associated with your specific app.
/// Each event type is identified by a unique name. Event names can be up
/// to 32 characters long, may only contain alphanumeric characters and
/// underscores ("_"), and must start with an alphabetic character. The
/// "firebase_" prefix is reserved and should not be used.
/// @{

/// Add Payment Info event. This event signifies that a user has submitted
/// their payment information to your app.
static const char* const kEventAddPaymentInfo = "add_payment_info";

/// E-Commerce Add To Cart event. This event signifies that an item was
/// added to a cart for purchase. Add this event to a funnel with
/// kEventEcommercePurchase to gauge the effectiveness of your
/// checParameter(kout, If you supply the @ref kParameterValue parameter),
/// you must also supply the @ref kParameterCurrency parameter so that
/// revenue metrics can be computed accurately. Params:
///
/// <ul>
///  <li>@ref kParameterQuantity (signed 64-bit integer)</li>
///  <li>@ref kParameterItemID (string)</li>
///  <li>@ref kParameterItemName (string)</li>
///  <li>@ref kParameterItemCategory (string)</li>
///  <li>@ref kParameterItemLocationID (string) (optional)</li>
///  <li>@ref kParameterPrice (double) (optional)</li>
///  <li>@ref kParameterCurrency (string) (optional)</li>
///  <li>@ref kParameterValue (double) (optional)</li>
///  <li>@ref kParameterOrigin (string) (optional)</li>
///  <li>@ref kParameterDestination (string) (optional)</li>
///  <li>@ref kParameterStartDate (string) (optional)</li>
///  <li>@ref kParameterEndDate (string) (optional)</li>
/// </ul>
static const char* const kEventAddToCart = "add_to_cart";

/// E-Commerce Add To Wishlist event. This event signifies that an item
/// was added to a wishlist. Use this event to identify popular gift items
/// in your app. Note: If you supply the @ref kParameterValue parameter,
/// you must also supply the @ref kParameterCurrency parameter so that
/// revenue metrics can be computed accurately. Params:
///
/// <ul>
///  <li>@ref kParameterQuantity (signed 64-bit integer)</li>
///  <li>@ref kParameterItemID (string)</li>
///  <li>@ref kParameterItemName (string)</li>
///  <li>@ref kParameterItemCategory (string)</li>
///  <li>@ref kParameterItemLocationID (string) (optional)</li>
///  <li>@ref kParameterPrice (double) (optional)</li>
///  <li>@ref kParameterCurrency (string) (optional)</li>
///  <li>@ref kParameterValue (double) (optional)</li>
/// </ul>
static const char* const kEventAddToWishlist = "add_to_wishlist";

/// App Open event. By logging this event when an App is moved to the
/// foreground, developers can understand how often users leave and return
/// during the course of a Session. Although Sessions are automatically
/// reported, this event can provide further clarification around the
/// continuous engagement of app-users.
static const char* const kEventAppOpen = "app_open";

/// E-Commerce Begin Checkout event. This event signifies that a user has
/// begun the process of checking out. Add this event to a funnel with
/// your kEventEcommercePurchase event to gauge the effectiveness of your
/// checkout process. Note: If you supply the @ref kParameterValue
/// parameter, you must also supply the @ref kParameterCurrency parameter
/// so that revenue metrics can be computed accurately. Params:
///
/// <ul>
///  <li>@ref kParameterValue (double) (optional)</li>
///  <li>@ref kParameterCurrency (string) (optional)</li>
///  <li>@ref kParameterTransactionID (string) (optional)</li>
///  <li>@ref kParameterStartDate (string) (optional)</li>
///  <li>@ref kParameterEndDate (string) (optional)</li>
///  <li>@ref kParameterNumberOfNights (signed 64-bit integer) (optional) for
///      hotel bookings</li>
///  <li>@ref kParameterNumberOfRooms (signed 64-bit integer) (optional) for
///      hotel bookings</li>
///  <li>@ref kParameterNumberOfPassengers (signed 64-bit integer) (optional)
///      for travel bookings</li>
///  <li>@ref kParameterOrigin (string) (optional)</li>
///  <li>@ref kParameterDestination (string) (optional)</li>
///  <li>@ref kParameterTravelClass (string) (optional) for travel bookings</li>
/// </ul>
static const char* const kEventBeginCheckout = "begin_checkout";

/// Earn Virtual Currency event. This event tracks the awarding of virtual
/// currency in your app. Log this along with @ref
/// kEventSpendVirtualCurrency to better understand your virtual economy.
/// Params:
///
/// <ul>
///  <li>@ref kParameterVirtualCurrencyName (string)</li>
///  <li>@ref kParameterValue (signed 64-bit integer or double)</li>
/// </ul>
static const char* const kEventEarnVirtualCurrency = "earn_virtual_currency";

/// E-Commerce Purchase event. This event signifies that an item was
/// purchased by a user. Note: This is different from the in-app purchase
/// event, which is reported automatically for App Store-based apps. Note:
/// If you supply the @ref kParameterValue parameter, you must also supply
/// the @ref kParameterCurrency parameter so that revenue metrics can be
/// computed accurately. Params:
///
/// <ul>
///  <li>@ref kParameterCurrency (string) (optional)</li>
///  <li>@ref kParameterValue (double) (optional)</li>
///  <li>@ref kParameterTransactionID (string) (optional)</li>
///  <li>@ref kParameterTax (double) (optional)</li>
///  <li>@ref kParameterShipping (double) (optional)</li>
///  <li>@ref kParameterCoupon (string) (optional)</li>
///  <li>@ref kParameterLocation (string) (optional)</li>
///  <li>@ref kParameterStartDate (string) (optional)</li>
///  <li>@ref kParameterEndDate (string) (optional)</li>
///  <li>@ref kParameterNumberOfNights (signed 64-bit integer) (optional) for
///      hotel bookings</li>
///  <li>@ref kParameterNumberOfRooms (signed 64-bit integer) (optional) for
///      hotel bookings</li>
///  <li>@ref kParameterNumberOfPassengers (signed 64-bit integer) (optional)
///      for travel bookings</li>
///  <li>@ref kParameterOrigin (string) (optional)</li>
///  <li>@ref kParameterDestination (string) (optional)</li>
///  <li>@ref kParameterTravelClass (string) (optional) for travel bookings</li>
/// </ul>
static const char* const kEventEcommercePurchase = "ecommerce_purchase";

/// Generate Lead event. Log this event when a lead has been generated in
/// the app to understand the efficacy of your install and re-engagement
/// campaigns. Note: If you supply the @ref kParameterValue parameter, you
/// must also supply the @ref kParameterCurrency parameter so that revenue
/// metrics can be computed accurately. Params:
///
/// <ul>
///  <li>@ref kParameterCurrency (string) (optional)</li>
///  <li>@ref kParameterValue (double) (optional)</li>
/// </ul>
static const char* const kEventGenerateLead = "generate_lead";

/// Join Group event. Log this event when a user joins a group such as a
/// guild, team or family. Use this event to analyze how popular certain
/// groups or social features are in your app. Params:
///
/// <ul>
///  <li>@ref kParameterGroupID (string)</li>
/// </ul>
static const char* const kEventJoinGroup = "join_group";

/// Level Up event. This event signifies that a player has leveled up in
/// your gaming app. It can help you gauge the level distribution of your
/// userbase and help you identify certain levels that are difficult to
/// pass. Params:
///
/// <ul>
///  <li>@ref kParameterLevel (signed 64-bit integer)</li>
///  <li>@ref kParameterCharacter (string) (optional)</li>
/// </ul>
static const char* const kEventLevelUp = "level_up";

/// Login event. Apps with a login feature can report this event to
/// signify that a user has logged in.
static const char* const kEventLogin = "login";

/// Post Score event. Log this event when the user posts a score in your
/// gaming app. This event can help you understand how users are actually
/// performing in your game and it can help you correlate high scores with
/// certain audiences or behaviors. Params:
///
/// <ul>
///  <li>@ref kParameterScore (signed 64-bit integer)</li>
///  <li>@ref kParameterLevel (signed 64-bit integer) (optional)</li>
///  <li>@ref kParameterCharacter (string) (optional)</li>
/// </ul>
static const char* const kEventPostScore = "post_score";

/// Present Offer event. This event signifies that the app has presented a
/// purchase offer to a user. Add this event to a funnel with the
/// kEventAddToCart and kEventEcommercePurchase to gauge your conversion
/// process. Note: If you supply the @ref kParameterValue parameter, you
/// must also supply the @ref kParameterCurrency parameter so that revenue
/// metrics can be computed accurately. Params:
///
/// <ul>
///  <li>@ref kParameterQuantity (signed 64-bit integer)</li>
///  <li>@ref kParameterItemID (string)</li>
///  <li>@ref kParameterItemName (string)</li>
///  <li>@ref kParameterItemCategory (string)</li>
///  <li>@ref kParameterItemLocationID (string) (optional)</li>
///  <li>@ref kParameterPrice (double) (optional)</li>
///  <li>@ref kParameterCurrency (string) (optional)</li>
///  <li>@ref kParameterValue (double) (optional)</li>
/// </ul>
static const char* const kEventPresentOffer = "present_offer";

/// E-Commerce Purchase Refund event. This event signifies that an item
/// purchase was refunded. Note: If you supply the @ref kParameterValue
/// parameter, you must also supply the @ref kParameterCurrency parameter
/// so that revenue metrics can be computed accurately. Params:
///
/// <ul>
///  <li>@ref kParameterCurrency (string) (optional)</li>
///  <li>@ref kParameterValue (double) (optional)</li>
///  <li>@ref kParameterTransactionID (string) (optional)</li>
/// </ul>
static const char* const kEventPurchaseRefund = "purchase_refund";

/// Search event. Apps that support search features can use this event to
/// contextualize search operations by supplying the appropriate,
/// corresponding parameters. This event can help you identify the most
/// popular content in your app. Params:
///
/// <ul>
///  <li>@ref kParameterSearchTerm (string)</li>
///  <li>@ref kParameterStartDate (string) (optional)</li>
///  <li>@ref kParameterEndDate (string) (optional)</li>
///  <li>@ref kParameterNumberOfNights (signed 64-bit integer) (optional) for
///      hotel bookings</li>
///  <li>@ref kParameterNumberOfRooms (signed 64-bit integer) (optional) for
///      hotel bookings</li>
///  <li>@ref kParameterNumberOfPassengers (signed 64-bit integer) (optional)
///      for travel bookings</li>
///  <li>@ref kParameterOrigin (string) (optional)</li>
///  <li>@ref kParameterDestination (string) (optional)</li>
///  <li>@ref kParameterTravelClass (string) (optional) for travel bookings</li>
/// </ul>
static const char* const kEventSearch = "search";

/// Select Content event. This general purpose event signifies that a user
/// has selected some content of a certain type in an app. The content can
/// be any object in your app. This event can help you identify popular
/// content and categories of content in your app. Params:
///
/// <ul>
///  <li>@ref kParameterContentType (string)</li>
///  <li>@ref kParameterItemID (string)</li>
/// </ul>
static const char* const kEventSelectContent = "select_content";

/// Share event. Apps with social features can log the Share event to
/// identify the most viral content. Params:
///
/// <ul>
///  <li>@ref kParameterContentType (string)</li>
///  <li>@ref kParameterItemID (string)</li>
/// </ul>
static const char* const kEventShare = "share";

/// Sign Up event. This event indicates that a user has signed up for an
/// account in your app. The parameter signifies the method by which the
/// user signed up. Use this event to understand the different behaviors
/// between logged in and logged out users. Params:
///
/// <ul>
///  <li>@ref kParameterSignUpMethod (string)</li>
/// </ul>
static const char* const kEventSignUp = "sign_up";

/// Spend Virtual Currency event. This event tracks the sale of virtual
/// goods in your app and can help you identify which virtual goods are
/// the most popular objects of purchase. Params:
///
/// <ul>
///  <li>@ref kParameterItemName (string)</li>
///  <li>@ref kParameterVirtualCurrencyName (string)</li>
///  <li>@ref kParameterValue (signed 64-bit integer or double)</li>
/// </ul>
static const char* const kEventSpendVirtualCurrency = "spend_virtual_currency";

/// Tutorial Begin event. This event signifies the start of the
/// on-boarding process in your app. Use this in a funnel with
/// kEventTutorialComplete to understand how many users complete this
/// process and move on to the full app experience.
static const char* const kEventTutorialBegin = "tutorial_begin";

/// Tutorial End event. Use this event to signify the user's completion of
/// your app's on-boarding process. Add this to a funnel with
/// kEventTutorialBegin to gauge the completion rate of your on-boarding
/// process.
static const char* const kEventTutorialComplete = "tutorial_complete";

/// Unlock Achievement event. Log this event when the user has unlocked an
/// achievement in your game. Since achievements generally represent the
/// breadth of a gaming experience, this event can help you understand how
/// many users are experiencing all that your game has to offer. Params:
///
/// <ul>
///  <li>@ref kParameterAchievementID (string)</li>
/// </ul>
static const char* const kEventUnlockAchievement = "unlock_achievement";

/// View Item event. This event signifies that some content was shown to
/// the user. This content may be a product, a webpage or just a simple
/// image or text. Use the appropriate parameters to contextualize the
/// event. Use this event to discover the most popular items viewed in
/// your app. Note: If you supply the @ref kParameterValue parameter, you
/// must also supply the @ref kParameterCurrency parameter so that revenue
/// metrics can be computed accurately. Params:
///
/// <ul>
///  <li>@ref kParameterItemID (string)</li>
///  <li>@ref kParameterItemName (string)</li>
///  <li>@ref kParameterItemCategory (string)</li>
///  <li>@ref kParameterItemLocationID (string) (optional)</li>
///  <li>@ref kParameterPrice (double) (optional)</li>
///  <li>@ref kParameterQuantity (signed 64-bit integer) (optional)</li>
///  <li>@ref kParameterCurrency (string) (optional)</li>
///  <li>@ref kParameterValue (double) (optional)</li>
///  <li>@ref kParameterStartDate (string) (optional)</li>
///  <li>@ref kParameterEndDate (string) (optional)</li>
///  <li>@ref kParameterFlightNumber (string) (optional) for travel bookings</li>
///  <li>@ref kParameterNumberOfPassengers (signed 64-bit integer) (optional)
///      for travel bookings</li>
///  <li>@ref kParameterNumberOfNights (signed 64-bit integer) (optional) for
///      travel bookings</li>
///  <li>@ref kParameterNumberOfRooms (signed 64-bit integer) (optional) for
///      travel bookings</li>
///  <li>@ref kParameterOrigin (string) (optional)</li>
///  <li>@ref kParameterDestination (string) (optional)</li>
///  <li>@ref kParameterSearchTerm (string) (optional) for travel bookings</li>
///  <li>@ref kParameterTravelClass (string) (optional) for travel bookings</li>
/// </ul>
static const char* const kEventViewItem = "view_item";

/// View Item List event. Log this event when the user has been presented
/// with a list of items of a certain category. Params:
///
/// <ul>
///  <li>@ref kParameterItemCategory (string)</li>
/// </ul>
static const char* const kEventViewItemList = "view_item_list";

/// View Search Results event. Log this event when the user has been
/// presented with the results of a search. Params:
///
/// <ul>
///  <li>@ref kParameterSearchTerm (string)</li>
/// </ul>
static const char* const kEventViewSearchResults = "view_search_results";
/// @}

}  // namespace analytics
}  // namespace firebase

#endif  // FIREBASE_ANALYTICS_CLIENT_CPP_INCLUDE_FIREBASE_ANALYTICS_EVENT_NAMES_H_
