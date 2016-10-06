// Copyright 2016 Google Inc. All Rights Reserved.

#ifndef FIREBASE_ANALYTICS_CLIENT_CPP_INCLUDE_FIREBASE_ANALYTICS_PARAMETER_NAMES_H_
#define FIREBASE_ANALYTICS_CLIENT_CPP_INCLUDE_FIREBASE_ANALYTICS_PARAMETER_NAMES_H_

/// @brief Namespace that encompasses all Firebase APIs.
namespace firebase {
/// @brief Firebase Analytics API.
namespace analytics {

/// @defgroup parameter_names Analytics Parameters
///
/// Predefined event parameter names.
///
/// Params supply information that contextualize Events. You can associate
/// up to 25 unique Params with each Event type. Some Params are suggested
/// below for certain common Events, but you are not limited to these. You
/// may supply extra Params for suggested Events or custom Params for
/// Custom events. Param names can be up to 24 characters long, may only
/// contain alphanumeric characters and underscores ("_"), and must start
/// with an alphabetic character. Param values can be up to 36 characters
/// long. The "firebase_" prefix is reserved and should not be used.
/// @{

/// Game achievement ID (string).
/// @code
/// using namespace firebase::analytics;
/// Parameter parameters[] = {
///    Parameter(kParameterAchievementID, "10_matches_won"),
///    // ...
///  };
/// @endcode
static const char* const kParameterAchievementID = "achievement_id";

/// Character used in game (string).
/// @code
/// using namespace firebase::analytics;
/// Parameter parameters[] = {
///    Parameter(kParameterCharacter, "beat_boss"),
///    // ...
///  };
/// @endcode
static const char* const kParameterCharacter = "character";

/// Type of content selected (string).
/// @code
/// using namespace firebase::analytics;
/// Parameter parameters[] = {
///    Parameter(kParameterContentType, "news article"),
///    // ...
///  };
/// @endcode
static const char* const kParameterContentType = "content_type";

/// Coupon code for a purchasable item (string).
/// @code
/// using namespace firebase::analytics;
/// Parameter parameters[] = {
///    Parameter(kParameterCoupon, "zz123"),
///    // ...
///  };
/// @endcode
static const char* const kParameterCoupon = "coupon";

/// Purchase currency in 3-letter <a href="http://en.wikipedia.org/wiki/ISO_4217#Active_codes">
/// ISO_4217</a> format (string).
/// @code
/// using namespace firebase::analytics;
/// Parameter parameters[] = {
///    Parameter(kParameterCurrency, "USD"),
///    // ...
///  };
/// @endcode
static const char* const kParameterCurrency = "currency";

/// Flight or Travel destination (string).
/// @code
/// using namespace firebase::analytics;
/// Parameter parameters[] = {
///    Parameter(kParameterDestination, "Mountain View, CA"),
///    // ...
///  };
/// @endcode
static const char* const kParameterDestination = "destination";

/// The arrival date, check-out date or rental end date for the item. This
/// should be in YYYY-MM-DD format (string).
/// @code
/// using namespace firebase::analytics;
/// Parameter parameters[] = {
///    Parameter(kParameterEndDate, "2015-09-14"),
///    // ...
///  };
/// @endcode
static const char* const kParameterEndDate = "end_date";

/// Flight number for travel events (string).
/// @code
/// using namespace firebase::analytics;
/// Parameter parameters[] = {
///    Parameter(kParameterFlightNumber, "ZZ800"),
///    // ...
///  };
/// @endcode
static const char* const kParameterFlightNumber = "flight_number";

/// Group/clan/guild ID (string).
/// @code
/// using namespace firebase::analytics;
/// Parameter parameters[] = {
///    Parameter(kParameterGroupID, "g1"),
///    // ...
///  };
/// @endcode
static const char* const kParameterGroupID = "group_id";

/// Item category (string).
/// @code
/// using namespace firebase::analytics;
/// Parameter parameters[] = {
///    Parameter(kParameterItemCategory, "t-shirts"),
///    // ...
///  };
/// @endcode
static const char* const kParameterItemCategory = "item_category";

/// Item ID (string).
/// @code
/// using namespace firebase::analytics;
/// Parameter parameters[] = {
///    Parameter(kParameterItemID, "p7654"),
///    // ...
///  };
/// @endcode
static const char* const kParameterItemID = "item_id";

/// The Google <a href="https://developers.google.com/places/place-id">Place ID</a> (string) that
/// corresponds to the associated item. Alternatively, you can supply your
/// own custom Location ID.
/// @code
/// using namespace firebase::analytics;
/// Parameter parameters[] = {
///    Parameter(kParameterItemLocationID, "ChIJiyj437sx3YAR9kUWC8QkLzQ"),
///    // ...
///  };
/// @endcode
static const char* const kParameterItemLocationID = "item_location_id";

/// Item name (string).
/// @code
/// using namespace firebase::analytics;
/// Parameter parameters[] = {
///    Parameter(kParameterItemName, "abc"),
///    // ...
///  };
/// @endcode
static const char* const kParameterItemName = "item_name";

/// Level in game (signed 64-bit integer).
/// @code
/// using namespace firebase::analytics;
/// Parameter parameters[] = {
///    Parameter(kParameterLevel, 42),
///    // ...
///  };
/// @endcode
static const char* const kParameterLevel = "level";

/// Location (string). The Google <a href="https://developers.google.com/places/place-id">Place ID
/// </a> that corresponds to the associated event. Alternatively, you can supply your own custom
/// Location ID.
/// @code
/// using namespace firebase::analytics;
/// Parameter parameters[] = {
///    Parameter(kParameterLocation, "ChIJiyj437sx3YAR9kUWC8QkLzQ"),
///    // ...
///  };
/// @endcode
static const char* const kParameterLocation = "location";

/// Number of nights staying at hotel (signed 64-bit integer).
/// @code
/// using namespace firebase::analytics;
/// Parameter parameters[] = {
///    Parameter(kParameterNumberOfNights, 3),
///    // ...
///  };
/// @endcode
static const char* const kParameterNumberOfNights = "number_of_nights";

/// Number of passengers traveling (signed 64-bit integer).
/// @code
/// using namespace firebase::analytics;
/// Parameter parameters[] = {
///    Parameter(kParameterNumberOfPassengers, 11),
///    // ...
///  };
/// @endcode
static const char* const kParameterNumberOfPassengers = "number_of_passengers";

/// Number of rooms for travel events (signed 64-bit integer).
/// @code
/// using namespace firebase::analytics;
/// Parameter parameters[] = {
///    Parameter(kParameterNumberOfRooms, 2),
///    // ...
///  };
/// @endcode
static const char* const kParameterNumberOfRooms = "number_of_rooms";

/// Flight or Travel origin (string).
/// @code
/// using namespace firebase::analytics;
/// Parameter parameters[] = {
///    Parameter(kParameterOrigin, "Mountain View, CA"),
///    // ...
///  };
/// @endcode
static const char* const kParameterOrigin = "origin";

/// Purchase price (double).
/// @code
/// using namespace firebase::analytics;
/// Parameter parameters[] = {
///    Parameter(kParameterPrice, 1.0),
///    Parameter(kParameterCurrency, "USD"),  // e.g. $1.00 USD
///    // ...
///  };
/// @endcode
static const char* const kParameterPrice = "price";

/// Purchase quantity (signed 64-bit integer).
/// @code
/// using namespace firebase::analytics;
/// Parameter parameters[] = {
///    Parameter(kParameterQuantity, 1),
///    // ...
///  };
/// @endcode
static const char* const kParameterQuantity = "quantity";

/// Score in game (signed 64-bit integer).
/// @code
/// using namespace firebase::analytics;
/// Parameter parameters[] = {
///    Parameter(kParameterScore, 4200),
///    // ...
///  };
/// @endcode
static const char* const kParameterScore = "score";

/// The search string/keywords used (string).
/// @code
/// using namespace firebase::analytics;
/// Parameter parameters[] = {
///    Parameter(kParameterSearchTerm, "periodic table"),
///    // ...
///  };
/// @endcode
static const char* const kParameterSearchTerm = "search_term";

/// Shipping cost (double).
/// @code
/// using namespace firebase::analytics;
/// Parameter parameters[] = {
///    Parameter(kParameterShipping, 9.50),
///    Parameter(kParameterCurrency, "USD"),  // e.g. $9.50 USD
///    // ...
///  };
/// @endcode
static const char* const kParameterShipping = "shipping";

/// Sign up method (string).
/// @code
/// using namespace firebase::analytics;
/// Parameter parameters[] = {
///    Parameter(kParameterSignUpMethod, "google"),
///    // ...
///  };
/// @endcode
static const char* const kParameterSignUpMethod = "sign_up_method";

/// The departure date, check-in date or rental start date for the item.
/// This should be in YYYY-MM-DD format (string).
/// @code
/// using namespace firebase::analytics;
/// Parameter parameters[] = {
///    Parameter(kParameterStartDate, "2015-09-14"),
///    // ...
///  };
/// @endcode
static const char* const kParameterStartDate = "start_date";

/// Tax amount (double).
/// @code
/// using namespace firebase::analytics;
/// Parameter parameters[] = {
///    Parameter(kParameterTax, 1.0),
///    Parameter(kParameterCurrency, "USD"),  // e.g. $1.00 USD
///    // ...
///  };
/// @endcode
static const char* const kParameterTax = "tax";

/// A single ID for a ecommerce group transaction (string).
/// @code
/// using namespace firebase::analytics;
/// Parameter parameters[] = {
///    Parameter(kParameterTransactionID, "ab7236dd9823"),
///    // ...
///  };
/// @endcode
static const char* const kParameterTransactionID = "transaction_id";

/// Travel class (string).
/// @code
/// using namespace firebase::analytics;
/// Parameter parameters[] = {
///    Parameter(kParameterTravelClass, "business"),
///    // ...
///  };
/// @endcode
static const char* const kParameterTravelClass = "travel_class";

/// A context-specific numeric value which is accumulated automatically
/// for each event type. This is a general purpose parameter that is
/// useful for accumulating a key metric that pertains to an event.
/// Examples include revenue, distance, time and points. Value should be
/// specified as signed 64-bit integer or double. Notes: Currency-related
/// values should be supplied using doubleand must be accompanied by a
/// @ref kParameterCurrency parameter. The valid range of accumulated
/// values is [-9,223,372,036,854.77, 9,223,372,036,854.77].
/// @code
/// using namespace firebase::analytics;
/// Parameter parameters[] = {
///    Parameter(kParameterValue, 3.99),
///    Parameter(kParameterCurrency, "USD"),  // e.g. $3.99 USD
///    // ...
///  };
/// @endcode
static const char* const kParameterValue = "value";

/// Name of virtual currency type (string).
/// @code
/// using namespace firebase::analytics;
/// Parameter parameters[] = {
///    Parameter(kParameterVirtualCurrencyName, "virtual_currency_name"),
///    // ...
///  };
/// @endcode
static const char* const kParameterVirtualCurrencyName =
    "virtual_currency_name";
/// @}

}  // namespace analytics
}  // namespace firebase

#endif  // FIREBASE_ANALYTICS_CLIENT_CPP_INCLUDE_FIREBASE_ANALYTICS_PARAMETER_NAMES_H_
