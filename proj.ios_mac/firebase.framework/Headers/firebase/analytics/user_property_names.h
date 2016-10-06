// Copyright 2016 Google Inc. All Rights Reserved.

#ifndef FIREBASE_ANALYTICS_CLIENT_CPP_INCLUDE_FIREBASE_ANALYTICS_USER_PROPERTY_NAMES_H_
#define FIREBASE_ANALYTICS_CLIENT_CPP_INCLUDE_FIREBASE_ANALYTICS_USER_PROPERTY_NAMES_H_

/// @brief Namespace that encompasses all Firebase APIs.
namespace firebase {
/// @brief Firebase Analytics API.
namespace analytics {

/// @defgroup user_property_names Analytics User Properties
///
/// Predefined user property names.
///
/// A UserProperty is an attribute that describes the app-user. By
/// supplying UserProperties, you can later analyze different behaviors of
/// various segments of your userbase. You may supply up to 25 unique
/// UserProperties per app, and you can use the name and value of your
/// choosing for each one. UserProperty names can be up to 24 characters
/// long, may only contain alphanumeric characters and underscores ("_"),
/// and must start with an alphabetic character. UserProperty values can
/// be up to 36 characters long. The "firebase_" prefix is reserved and
/// should not be used.
/// @{

/// The method used to sign in. For example, "google", "facebook" or
/// "twitter".
static const char* const kUserPropertySignUpMethod = "sign_up_method";
/// @}

}  // namespace analytics
}  // namespace firebase

#endif  // FIREBASE_ANALYTICS_CLIENT_CPP_INCLUDE_FIREBASE_ANALYTICS_USER_PROPERTY_NAMES_H_
