// Copyright 2016 Google Inc. All Rights Reserved.

#ifndef FIREBASE_APP_CLIENT_CPP_SRC_INCLUDE_FIREBASE_INTERNAL_COMMON_H_
#define FIREBASE_APP_CLIENT_CPP_SRC_INCLUDE_FIREBASE_INTERNAL_COMMON_H_

// This file contains definitions that configure the SDK.

// Move operators use rvalue references, which are a C++11 extension.
// Also, stlport doesn't implement std::move().
#if __cplusplus >= 201103L && !defined(_STLPORT_VERSION)
#define FIREBASE_USE_MOVE_OPERATORS
#endif  // __cplusplus >= 201103L && !defined(_STLPORT_VERSION)

#endif  // FIREBASE_APP_CLIENT_CPP_SRC_INCLUDE_FIREBASE_INTERNAL_COMMON_H_
