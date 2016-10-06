// Copyright 2016 Google Inc. All Rights Reserved.

#ifndef FIREBASE_INVITES_CLIENT_CPP_INCLUDE_FIREBASE_INVITES_RECEIVER_H_
#define FIREBASE_INVITES_CLIENT_CPP_INCLUDE_FIREBASE_INVITES_RECEIVER_H_

#include <string>

namespace firebase {
class App;
template <typename ResultType>
class Future;
namespace invites {
namespace internal {

// Forward declaration for platform-specific data, implemented in each library.
class InvitesReceiverInternal;

}  // namespace internal

/// A class for receiving %App Invites and dynamic deep links.
///
/// You would normally use this class when your app starts up.
/// After fetching you might get an invite ID, a deep link, both, or neither.
class InvitesReceiver {
 public:
  /// Results from calling Fetch() to fetch incoming invite / deep link.
  ///
  /// This will be returned by Fetch inside of a Future<FetchResult>. In the
  /// returned Future, you should check the value of Error() - if it's non-zero,
  /// an error occurred while fetching invites. You can use the Future's Error()
  /// and ErrorMessage() to get more information about what went wrong.
  struct FetchResult {
    /// @brief The Invitation ID, if any.
    ///
    /// Will be empty if there was just a deep link, without an invitation.
    /// This invitation ID will match the invitation ID on the sender side, so
    /// you can use analytics to determine that the invitation was accepted, and
    /// even save it for future analytical use.
    std::string invitation_id;

    /// @brief Deep Link URL, if any.
    ///
    /// Will be empty if there was just an App Invite and not a deep link.
    std::string deep_link;

    /// @brief Whether the deep link is a "strong match" or a "weak match" as
    /// defined by the Invites library.
    ///
    /// If the %App Invites SDK indicates a weak match for a deeplink, it means
    /// that the match between the deeplink and the receiving device may not be
    /// perfect. In this case your app should reveal no personal information
    /// from the deeplink.
    bool is_strong_match;
  };

  /// Results from calling ConvertInvitation().
  ///
  /// This will be returned inside of a Future<ConvertInvitationResult>. In the
  /// returned Future, you should check the value of Error() - if it's non-zero,
  /// an error occurred while performing the conversion. You can use the
  /// Future's Error() and ErrorMessage() to get more information about what
  /// went wrong.
  struct ConvertInvitationResult {
    /// The invitation ID that we performed a conversion on.
    std::string invitation_id;
    /// Whether the conversion succeeded.
    bool was_converted;
  };

  /// @brief Instantiate the InvitesReceiver library.
  ///
  /// After this object is created, you can call Fetch() to find out if you
  /// have received an invite or a deep link.
  ///
  /// @note In theory you can have more than one of these active at once, each
  /// using a different Firebase App. In practice, you will probably never have
  /// more than one of these at a time.
  explicit InvitesReceiver(const ::firebase::App& app);

  ~InvitesReceiver();

  /// @brief Check for an incoming invitation and/or deep link.
  ///
  /// This kicks off a possibly asynchronous process. If you run Fetch() again
  /// while the previous Fetch() is still running, you will get the same
  /// result. If you run Fetch() again after the previous Fetch() finishes,
  /// the results of the previous Fetch() will be overridden.
  ///
  /// Usage:
  /// ~~~{.cpp}
  /// InvitesReceiver* receiver = new InvitesReceiver();
  /// auto fetch_result = receiver->Fetch();
  /// // ... later on ...
  /// if (fetch_result.Status() == kFutureStatusComplete) {
  ///   if (fetch_result.Result()->invitation_id != "") {
  ///     // got invitation ID
  ///   }
  /// }
  /// ~~~
  Future<FetchResult> Fetch();

  /// Get the (possibly still pending) results of the most recent Fetch() call.
  Future<FetchResult> FetchLastResult() const;

  /// @brief Mark the invitation as "converted" in some app-specific way.
  ///
  /// Once you have acted on the invite in some application-specific way, you
  /// can call this function to tell Firebase that a "conversion" has occurred
  /// and the invite has been acted on in some significant way.
  ///
  /// You don't need to have called Fetch() first, since a "conversion" can
  /// happen far in the future from when the invite was initially received,
  /// e.g. if it corresponds to the user setting up an account, making a
  /// purchase, etc.
  ///
  /// Just save the invitation ID when you initially receive it, and use it
  /// later when performing the conversion.
  ///
  /// Usage:
  /// ~~~{.cpp}
  /// InvitesReceiver* receiver = new InvitesReceiver();
  /// auto convert_result = receiver->ConvertInvitation(my_invitation_id);
  /// // ... later on ...
  /// if (convert_result.Status() == kFutureStatusComplete) {
  ///   if (convert_result.Result()->error_code == 0) {
  ///     // successfully marked the invitation as converted!
  ///   }
  /// }
  /// ~~~
  ///
  /// @param[in] invitation_id The invitation ID to mark as a conversion.
  ///
  /// @return A future result telling you whether the conversion succeeded.
  Future<ConvertInvitationResult> ConvertInvitation(const char* invitation_id);

  /// Get the (possibly still pending) results of the most recent
  /// ConvertInvitation call.
  ///
  /// @return The future result from the last call to ConvertInvitation().
  Future<ConvertInvitationResult> ConvertInvitationLastResult() const;

 private:
  /// Platform-specific data, actually defined in the implementation file
  /// and newed/deleted in the constructor and destructor, respectively.
  internal::InvitesReceiverInternal* internal_;
};

}  // namespace invites
}  // namespace firebase

#endif  // FIREBASE_INVITES_CLIENT_CPP_INCLUDE_FIREBASE_INVITES_RECEIVER_H_
