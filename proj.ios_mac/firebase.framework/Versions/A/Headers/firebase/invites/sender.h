// Copyright 2016 Google Inc. All Rights Reserved.

#ifndef FIREBASE_INVITES_CLIENT_CPP_INCLUDE_FIREBASE_INVITES_SENDER_H_
#define FIREBASE_INVITES_CLIENT_CPP_INCLUDE_FIREBASE_INVITES_SENDER_H_

#include <string>
#include <vector>

namespace firebase {
class App;
template <typename ResultType>
class Future;

/// @brief Firebase Invites API.
///
/// Firebase Invites is a cross-platform solution for sending personalized
/// email and SMS invitations, on-boarding users, and measuring the impact
/// of invitations.
namespace invites {

/// Platforms supported by %App Invites. This allows you to
/// specify app IDs for alternate platforms.
/// @see firebase::invites::SetAppID()
enum InvitesPlatform { kPlatformAndroid, kPlatformIOS };

class InvitesSharedData;

namespace internal {
// Forward declaration for platform-specific data, implemented in each library.
class InvitesSenderInternal;
}  // namespace internal

/// A class for sending %App Invites to your contacts.
///
/// You can specify various  parameters about the invite (image, text,
/// analytics tracking IDs), and then call @ref SendInvite() to allow the user
/// to pick contacts and send off the invitation.
class InvitesSender {
 public:
  /// Maximum length for an invitation message set via SetMessageText().
  static const unsigned int kMaxMessageLength = 100;

  /// Maximum length for an HTML invitation message set via
  /// SetEmailContentHtml().
  static const unsigned int kMaxEmailHtmlContentLength = 512000;

  /// Minimum length for the call to action button set via
  /// SetCallToActionText().
  static const unsigned int kMinCallToActionTextLength = 2;

  /// Maximum length for the call to action button set via
  /// SetCallToActionText().
  static const unsigned int kMaxCallToActionTextLength = 20;

  /// Maximum length for the app description set via
  /// SetDescriptionText().
  static const unsigned int kMaxDescriptionTextLength = 1000;

  /// Results from calling SendInvite() to send an invitation.
  ///
  /// This will be returned by SendInvite() inside a Future<SendInviteResult>.
  /// In the returned Future, you should check the value of Error() - if it's
  /// non-zero, an error occurred while sending invites. You can use the
  /// Future's Error() and ErrorMessage() to get more information about what
  /// went wrong.
  struct SendInviteResult {
    /// @brief The Invitation IDs we sent invites to, if any.
    ///
    /// If this is empty, it means the user either chose to back out of the
    /// sending UI without sending invitations (Error() == 0) or something
    /// went wrong (Error() != 0).
    ///
    /// If this is nonempty, then these invitation IDs will match the invitation
    /// IDs on the receiving side, which may be helpful for analytics purposes.
    std::vector<std::string> invitation_ids;
  };

  /// @brief Instantiate the InvitesSender library. After this, you set various
  /// bits of data on it and call ShowUI to show the invite UI.
  ///
  /// @note In theory you can have more than one of these, each using a
  /// different Firebase App (but only one can be displayed in the UI at a
  /// time).
  ///
  /// In practice, you will probably never have more than one of these at a
  /// time.
  explicit InvitesSender(const ::firebase::App& app);

  /// @brief Destructor.
  ///
  /// If the invite UI is still being displayed, this will cancel it as well.
  ~InvitesSender();

  /// @brief Set the title text for the Invites UI window.
  ///
  /// @note You must call this and SetMessageText() to send invitations.
  ///
  /// @param[in] title_text A UTF8 string with title text for the UI.
  void SetTitleText(const char* title_text);

  /// @brief Start displaying the invitation UI, which will ultimately result
  /// in sending zero or more invitations.
  ///
  /// This will take all of the invitation settings you have previously
  /// specified, and display a UI to the user where they can share a link
  /// to the app with their friends.
  ///
  /// At a minimum, you will need to have called SetTitleText() and
  /// SetMessageText() or the invitation will not be sent.
  ///
  /// Usage:
  /// ~~~{.cpp}
  /// InvitesSender* sender = new InvitesSender();
  /// auto send_result = sender->Fetch();
  /// // ... later on ...
  /// if (send_result.Status() == kFutureStatusComplete) {
  ///   if (send_result.Result()->error_code == 0) {
  ///     if (send_result.Result()->invitation_ids.length() > 0) {
  ///       // Invitations were sent.
  ///     }
  ///     else {
  ///       // User canceled.
  ///     }
  ///   }
  /// }
  /// ~~~
  ///
  /// @return A future result telling us whether the invitation was sent.
  ///
  /// @note If sending an invite is already pending, calling functions to change
  /// invitation options (e.g. SetMessageText()) will not change the current
  /// settings.
  Future<SendInviteResult> SendInvite();

  /// @brief Get the results of the previous call to SendInvite. This will stay
  /// available until you call SendInvite again.
  ///
  /// @return The future result from the most recent call to SendInvite().
  Future<SendInviteResult> SendInviteLastResult();

  /// Reset any previously-set invitation parameters back to their default
  /// values. This allows you to reuse the existing InvitesSender class
  /// for multiple invitations, and also frees up a bit of memory if you
  /// call it when you are finished with InvitesSender for now.
  void Reset();

  /// @brief Sets the client ID for your app for another platform (don't call
  /// this for your current platform).
  ///
  /// Make sure the client ID you specify here matches the client ID for your
  /// project in Google Developer Console for the other platform.
  ///
  /// @note Firebase is smart enough to infer this automatically if your
  /// project in Google Developer Console has only one app for each platform.
  ///
  /// @param[in] other_platform The platform you want to set the client ID for.
  /// @param[in] client_id The Google Developer Console client ID for the app.
  void SetOtherPlatformClientID(InvitesPlatform other_platform,
                                const char* client_id);

  /// @brief Set the text of the invitation message.
  ///
  /// SetMessageText(), SetCustomImageUrl(), and SetCallToActionText() comprise
  /// the standard text invitation options.
  ///
  /// If you use the standard text methods as well as SetEmailContentHtml() and
  /// SetEmailSubjectText(), the HTML methods will take priority if your
  /// platform supports them.
  ///
  /// The user is able to modify this message before sending the invite.
  ///
  /// @note The length of this message must not exceed kMaxMessageLength
  /// characters, so it can fit in an SMS message along with the link.
  ///
  /// @note You must call this and SetTitleText() or you will not be able to
  /// send an invitation.
  ///
  /// @param[in] message_text UTF8 text for the invitation message.
  void SetMessageText(const char* message_text);

  /// @brief Set an image to include in the invitation.
  ///
  /// @param[in] image_url URL for the image.
  void SetCustomImageUrl(const char* image_url);

  /// @brief Text shown on the email invitation button for the user to
  /// accept the invitation.
  ///
  /// Default text will be used if this is not set (or set to null).
  ///
  /// @note The length of this text must not exceed kMaxCallToActionTextLength
  /// characters, and must be no shorter than kMinCallToActionTextLength
  /// characters.
  ///
  /// @param[in] call_to_action_text UTF8 text for the invitation button.
  void SetCallToActionText(const char* call_to_action_text);

  /// @brief Set the app description text for email invitations.
  ///
  /// @note The length of this text must not exceed kMaxDescriptionTextLength
  /// characters.
  ///
  /// @note This function is for iOS only. On Android, this setting will be
  /// ignored, and your app's description will be automatically populated from
  /// its Google Play listing.
  ///
  /// @param[in] description_text UTF8 text describing your app, for emails.
  void SetDescriptionText(const char* description_text);

  /// @brief Set the full HTML content of the invitation that will be sent.
  ///
  /// This should be properly-formatted UTF8 HTML with no JavaScript. The
  /// pattern %%APPINVITE_LINK_PLACEHOLDER%% will be replaced with the
  /// invitation URL.
  ///
  /// This function takes precendence over the text funcitons SetMessageText(),
  /// SetCallToActionText(), and SetCustomImageUrl(). If you want full control
  /// over the contents of the invitation, you should use this function.
  ///
  /// If you use this function you must also use SetEmailSubjectText() or the
  /// HTML content will be ignored.
  ///
  /// If you do use these HTML text methods, they will take priority over the
  /// standard text methods, if HTML is supported on your platform. You probably
  /// still want to use SetMessageText() and the other text methods in case your
  /// platform doesn't support HTML.
  ///
  /// @note HTML invitation content is only supported on Android.
  ///
  /// @note The length of the HTML email content must not exceed
  /// kMaxEmailContentHtmlLength characters.
  ///
  /// @param[in] invitation_html HTML text content, with UTF8 encoding.
  void SetEmailContentHtml(const char* invitation_html);

  /// @brief Set the subject text for an HTML e-mail.
  ///
  /// If you use this, you must use SetEmailContentHtml() as well or the
  /// HTML content will be ignored.
  ///
  /// @param[in] invitation_subject_text UTF8 text for the email subject line.
  void SetEmailSubjectText(const char* invitation_subject_text);

  /// @brief Set an optional deep link that will be sent with the message.
  ///
  /// If you don't specify this, your invite will have no deep link.
  /// If you specify nullptr, you can remove any previously-set deep link and
  /// use the default value of no deep link.
  ///
  /// @param[in] deep_link_url A URL that your app knows how to handle.
  void SetDeepLinkUrl(const char* deep_link_url);

  /// @brief Sets the optional Google Analytics Tracking id.
  ///
  /// The tracking id should be created for the calling application under
  /// Google Analytics. The tracking id is recommended so that invitations sent
  /// from the calling application are available in Google Analytics.
  ///
  /// @note This function is only supported on Android. On iOS, if you want
  /// to track invitations in Google Analytics, you will have to do so manually.
  ///
  /// @param[in] tracking_id Google Analytics tracking ID.
  void SetGoogleAnalyticsTrackingID(const char* tracking_id);

  /// Sets the optional minimum version of the android app installed on the
  /// receiving device. If you don't specify this, any Android version will be
  /// allowed.
  ///
  /// @param[in] version_code Android version code, as used in the manifest.
  void SetAndroidMinimumVersionCode(int version_code);

  /// @brief Add an optional additional referral parameter, which is passed to
  /// the invite URL as a key/value pair.
  ///
  /// You can have any number of these.  Use ClearReferralParams() to remove
  /// these referral parameters.
  ///
  /// If you specify a key that was previously specified, you will replace
  /// its value with the new value you specify (or delete the key
  /// entirely, if you specified nullptr as the value).
  /// These key/value pairs will be included in the referral URL as query
  /// parameters, so they can be read by the app on the receiving side.
  ///
  /// @note Referral parameters are only supported on Android.
  ///
  /// @param[in] key Referral parameter key, of the key/value pair.
  /// @param[in] value Referral parameter value, of the key/value pair.
  void AddReferralParam(const char* key, const char* value);

  /// @brief Clear any referral parameters previously added via
  /// AddReferralParam().
  void ClearReferralParams();

 private:
  /// Platform-specific data, actually defined in the implementation file
  /// and newed/deleted in the constructor and destructor, respectively.
  internal::InvitesSenderInternal* internal_;
};

}  // namespace invites
}  // namespace firebase

#endif  // FIREBASE_INVITES_CLIENT_CPP_INCLUDE_FIREBASE_INVITES_SENDER_H_
