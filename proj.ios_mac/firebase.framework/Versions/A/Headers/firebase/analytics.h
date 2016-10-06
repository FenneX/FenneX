// Copyright 2016 Google Inc. All Rights Reserved.

#ifndef FIREBASE_ANALYTICS_CLIENT_CPP_INCLUDE_FIREBASE_ANALYTICS_H_
#define FIREBASE_ANALYTICS_CLIENT_CPP_INCLUDE_FIREBASE_ANALYTICS_H_

#include <cstddef>
#include <cstdint>

#include "firebase/app.h"

/// @brief Namespace that encompasses all Firebase APIs.
namespace firebase {

/// @brief Firebase Analytics API.
///
/// See <a href="/docs/analytics">the developer guides</a> for general
/// information on using Firebase Analytics in your apps.
namespace analytics {

/// @brief Event parameter.
///
/// Parameters supply information that contextualize events
/// (see @ref analytics::LogEvent()).  You can associate up to 25 unique
/// Parameters with each event type (name).
///
/// Common event types (names) are suggested in @ref event_names
/// (%event_names.h) with parameters of common event types defined in
/// @ref parameter_names (%parameter_names.h).
///
/// You are not limited to the set of event types and parameter names suggested
/// in @ref event_names (%event_names.h) and  %parameter_names.h respectively.
/// Additional Parameters can be supplied for suggested event types or custom
/// Parameters for custom event types.
///
/// Parameter names must be a combination of letters and digits
/// (matching the regular expression [a-zA-Z0-9]) between 1 and 24 characters
/// long starting with a letter [a-zA-Z] character.
///
/// Parameter string values can be up to 36 characters long.
///
/// An array of this structure is passed to LogEvent in order to associate
/// parameter's of an event (Parameter::name) with values (Parameter::value)
/// where each value can be a double, 64-bit integer or string.
///
/// For example, a game may log an achievement event along with the
/// character the player is using and the level they're currently on:
///
/// @code
/// using namespace firebase::analytics;
/// int64_t current_level = GetCurrentLevel();
/// const Parameter achievement_parameters[] = {
///   Parameter(kParameterAchievementID,  "ultimate_wizard"),
///   Parameter(kParameterCharacter, "mysterion"),
///   Parameter(kParameterLevel, current_level),
/// };
/// LogEvent(kEventUnlockAchievement, achievement_parameters,
///          sizeof(achievement_parameters) /
///          sizeof(achievement_parameters[0]));
/// @endcode
///
/// Alternatively, users can explicitly set the type field associated with each
/// parameter value:
///
/// @code
/// using namespace firebase::analytics;
/// int64_t current_level = GetCurrentLevel();
/// const Parameter achievement_parameters[] = {
///   { kParameterAchievementID, kString, "ultimate_wizard" },
///   { kParameterCharacter, kString, "mysterion" },
///   { kParameterLevel, kInt64, current_level },
/// };
/// LogEvent(kEventUnlockAchievement, achievement_parameters,
///          sizeof(achievement_parameters) /
///          sizeof(achievement_parameters[0]));
/// @endcode
///
struct Parameter {
  /// Construct a 64-bit integer parameter.
  ///
  /// @param parameter_name Name of the parameter (see Parameter::name).
  /// @param parameter_value Integer value for the parameter.
  Parameter(const char* parameter_name, int parameter_value)
      : name(parameter_name), type(kInt64) {
    value.int64_value = parameter_value;
  }

  /// Construct a 64-bit integer parameter.
  ///
  /// @param parameter_name Name of the parameter (see Parameter::name).
  /// @param parameter_value Integer value for the parameter.
  Parameter(const char* parameter_name, int64_t parameter_value)
      : name(parameter_name), type(kInt64) {
    value.int64_value = parameter_value;
  }

  /// Construct a floating point parameter.
  ///
  /// @param parameter_name Name of the parameter (see Parameter::name).
  /// @param parameter_value Floating point value for the parameter.
  Parameter(const char* parameter_name, double parameter_value)
      : name(parameter_name), type(kDouble) {
    value.double_value = parameter_value;
  }

  /// Construct a string parameter.
  ///
  /// @param parameter_name Name of the parameter (see Parameter::name).
  /// @param parameter_value String value for the parameter (see kString).
  Parameter(const char* parameter_name, const char* parameter_value)
      : name(parameter_name), type(kString) {
    value.string_value = parameter_value;
  }

// clang-format off
// clang-format on


  /// @brief Name of the parameter.
  ///
  /// Parameter names must be a combination of letters and digits
  /// (matching the regular expression [a-zA-Z0-9]) between 1 and 24 characters
  /// long starting with a letter [a-zA-Z] character.
  const char* name;
  /// @brief Type of a @ref Parameter.
  enum Type {
    /// Parameter::value.double_value contains the value.
    kDouble,
    /// Parameter::value.int64_value contains the value.
    kInt64,
    /// Parameter::value.string_value contains the value.
    ///
    /// Parameter string values can be up to 36 characters long.
    kString,
    // NOLINTNEXTLINE
  } type;  ///<@brief Type of the parameter.
  /// @brief Value of the parameter.
  ///
  /// The selected union member is determined by Parameter::type.
  /// See Parameter for an example.
  union Value {
    /// Value of type double.
    ///
    /// @see kDouble
    double double_value;
    /// 64-bit integer parameter value.
    ///
    /// @see kInt64
    int64_t int64_value;
    /// String parameter value up to 36 characters long.
    ///
    /// @see kString
    const char* string_value;
    // NOLINTNEXTLINE
  } value;  ///<@brief Value of the parameter.
};

/// @brief Initialize the Analytics API.
///
/// This must be called prior to calling any other methods in the
/// firebase::analytics namespace.
///
/// @param[in] app Default @ref firebase::App instance.
///
/// @see firebase::App::GetInstance().
void Initialize(const App& app);

/// @brief Terminate the Analytics API.
///
/// Cleans up resources associated with the API.
void Terminate();

/// @brief Sets whether analytics collection is enabled for this app on this
/// device.
///
/// This setting is persisted across app sessions. By default it is enabled.
///
/// @param[in] enabled true to enable analytics collection, false to disable.
void SetAnalyticsCollectionEnabled(bool enabled);

/// @brief Log an event with one string parameter.
///
/// @param[in] name Name of the event to log. Should contain 1 to 32
/// alphanumeric characters or underscores. The name must start with an
/// alphabetic character. Some event names are reserved. See @ref event_names
/// (%event_names.h) for the list of reserved event names. The "firebase_"
/// prefix is reserved and should not be used. Note that event names are
/// case-sensitive and that logging two events whose names differ only in
/// case will result in two distinct events.
/// @param[in] parameter_name Name of the parameter to log.
/// For more information, see @ref Parameter.
/// @param[in] parameter_value Value of the parameter to log.
///
/// @see LogEvent(const char*, const Parameter*, size_t)
void LogEvent(const char* name, const char* parameter_name,
              const char* parameter_value);

/// @brief Log an event with one float parameter.
///
/// @param[in] name Name of the event to log. Should contain 1 to 32
/// alphanumeric characters or underscores. The name must start with an
/// alphabetic character. Some event names are reserved. See @ref event_names
/// (%event_names.h) for the list of reserved event names. The "firebase_"
/// prefix is reserved and should not be used. Note that event names are
/// case-sensitive and that logging two events whose names differ only in
/// case will result in two distinct events.
/// @param[in] parameter_name Name of the parameter to log.
/// For more information, see @ref Parameter.
/// @param[in] parameter_value Value of the parameter to log.
///
/// @see LogEvent(const char*, const Parameter*, size_t)
void LogEvent(const char* name, const char* parameter_name,
              const double parameter_value);

/// @brief Log an event with one 64-bit integer parameter.
///
/// @param[in] name Name of the event to log. Should contain 1 to 32
/// alphanumeric characters or underscores. The name must start with an
/// alphabetic character. Some event names are reserved. See @ref event_names
/// (%event_names.h) for the list of reserved event names. The "firebase_"
/// prefix is reserved and should not be used. Note that event names are
/// case-sensitive and that logging two events whose names differ only in
/// case will result in two distinct events.
/// @param[in] parameter_name Name of the parameter to log.
/// For more information, see @ref Parameter.
/// @param[in] parameter_value Value of the parameter to log.
///
/// @see LogEvent(const char*, const Parameter*, size_t)
void LogEvent(const char* name, const char* parameter_name,
              const int64_t parameter_value);

/// @brief Log an event with one integer parameter
/// (stored as a 64-bit integer).
///
/// @param[in] name Name of the event to log. Should contain 1 to 32
/// alphanumeric characters or underscores. The name must start with an
/// alphabetic character. Some event names are reserved. See @ref event_names
/// (%event_names.h) for the list of reserved event names. The "firebase_"
/// prefix is reserved and should not be used. Note that event names are
/// case-sensitive and that logging two events whose names differ only in
/// case will result in two distinct events.
/// @param[in] parameter_name Name of the parameter to log.
/// For more information, see @ref Parameter.
/// @param[in] parameter_value Value of the parameter to log.
///
/// @see LogEvent(const char*, const Parameter*, size_t)
void LogEvent(const char* name, const char* parameter_name,
              const int parameter_value);

/// @brief Log an event with no parameters.
///
/// @param[in] name Name of the event to log. Should contain 1 to 32
/// alphanumeric characters or underscores. The name must start with an
/// alphabetic character. Some event names are reserved. See @ref event_names
/// (%event_names.h) for the list of reserved event names. The "firebase_"
/// prefix is reserved and should not be used. Note that event names are
/// case-sensitive and that logging two events whose names differ only in
/// case will result in two distinct events.
///
/// @see LogEvent(const char*, const Parameter*, size_t)
void LogEvent(const char* name);

// clang-format off

/// @brief Log an event with associated parameters.
///
/// An Event is an important occurrence in your app that you want to
/// measure.  You can report up to 500 different types of events per app and
/// you can associate up to 25 unique parameters with each Event type.
///
/// Some common events are documented in @ref event_names (%event_names.h),
/// but you may also choose to specify custom event types that are associated
/// with your specific app.
///
/// @param[in] name Name of the event to log. Should contain 1 to 32
/// alphanumeric characters or underscores. The name must start with an
/// alphabetic character. Some event names are reserved. See @ref event_names
/// (%event_names.h) for the list of reserved event names. The "firebase_"
/// prefix is reserved and should not be used. Note that event names are
/// case-sensitive and that logging two events whose names differ only in
/// case will result in two distinct events.
/// @param[in] parameters Array of Parameter structures.
/// @param[in] number_of_parameters Number of elements in the parameters
/// array.
void LogEvent(const char* name, const Parameter* parameters,
              size_t number_of_parameters);
// clang-format on

/// @brief Set a user property to the given value.
///
/// Properties associated with a user allow a developer to segment users
/// into groups that are useful to their application.  Up to 25 properties
/// can be associated with a user.
///
/// Suggested property names are listed @ref user_property_names
/// (%user_property_names.h) but you're not limited to this set. For example,
/// the "gamertype" property could be used to store the type of player where
/// a range of values could be "casual", "mid_core", or "core".
///
/// @param[in] name Name of the user property to set.  This must be a
/// combination of letters and digits (matching the regular expression
/// [a-zA-Z0-9] between 1 and 24 characters long starting with a letter
/// [a-zA-Z] character.
/// @param[in] value Value to set the user property to.  Set this argument to
/// NULL or nullptr to remove the user property.  The value can be between 1
/// to 36 characters long.
void SetUserProperty(const char* name, const char* value);

/// @brief Sets the user ID property.
///
/// This feature must be used in accordance with
/// <a href="https://www.google.com/policies/privacy">Google's Privacy
/// Policy</a>
///
/// @param[in] user_id The user ID associated with the user of this app on this
/// device.  The user ID must be non-empty and no more than 36 characters long.
/// Setting user_id to NULL or nullptr removes the user ID.
void SetUserId(const char* user_id);

/// @brief Sets the minimum engagement time required before starting a session.
///
/// @note The default value is 10000 (10 seconds).
///
/// @param milliseconds The minimum engagement time required to start a new
/// session.
void SetMinimumSessionDuration(int64_t milliseconds);

/// @brief Sets the duration of inactivity that terminates the current session.
///
/// @note The default value is 1800000 (30 minutes).
///
/// @param milliseconds The duration of inactivity that terminates the current
/// session.
void SetSessionTimeoutDuration(int64_t milliseconds);

}  // namespace analytics
}  // namespace firebase

#endif  // FIREBASE_ANALYTICS_CLIENT_CPP_INCLUDE_FIREBASE_ANALYTICS_H_
