#ifndef FIREBASE_INVITES_CLIENT_CPP_INCLUDE_FIREBASE_INVITES_H_
#define FIREBASE_INVITES_CLIENT_CPP_INCLUDE_FIREBASE_INVITES_H_

// Copyright 2016 Google Inc. All Rights Reserved.

#include "firebase/app.h"
#include "firebase/invites/receiver.h"
#include "firebase/invites/sender.h"

namespace firebase {

/// A C++ version of the Firebase Invites API.

namespace invites {

/// @brief Initialize the Firebase Invites library.
///
/// You must call this before instantiating any InvitesReceiver or InvitesSender
/// objects.
///
/// @return kInitResultSuccess if initialization succeeded, or
/// kInitResultFailedMissingDependency on Android if Google Play services is
/// not available on the current device.
InitResult Initialize(const App& app);

}  // namespace invites
}  // namespace firebase

#endif  // FIREBASE_INVITES_CLIENT_CPP_INCLUDE_FIREBASE_INVITES_H_
