// Copyright (c) 2009 Marshall A. Greenblatt. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the name Chromium Embedded
// Framework nor the names of its contributors may be used to endorse
// or promote products derived from this software without specific prior
// written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#ifndef _CEF_TYPES_H
#define _CEF_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif


// Bring in platform-specific definitions.
#ifdef _WIN32
#include "cef_types_win.h"
#endif


// Define handler return value types. Returning RV_HANDLED indicates
// that the implementation completely handled the method and that no further
// processing is required.  Returning RV_CONTINUE indicates that the
// implementation did not handle the method and that the default handler
// should be called.
enum cef_retval_t
{
  RV_HANDLED   = 0,
  RV_CONTINUE  = 1,
};

// Various browser navigation types supported by chrome.
enum cef_handler_navtype_t
{
  NAVTYPE_LINKCLICKED = 0,
  NAVTYPE_FORMSUBMITTED,
  NAVTYPE_BACKFORWARD,
  NAVTYPE_RELOAD,
  NAVTYPE_FORMRESUBMITTED,
  NAVTYPE_OTHER,
};

// Supported error code values. See net\base\net_error_list.h for complete
// descriptions of the error codes.
enum cef_handler_errorcode_t
{
  ERR_FAILED = -2,
  ERR_ABORTED = -3,
  ERR_INVALID_ARGUMENT = -4,
  ERR_INVALID_HANDLE = -5,
  ERR_FILE_NOT_FOUND = -6,
  ERR_TIMED_OUT = -7,
  ERR_FILE_TOO_BIG = -8,
  ERR_UNEXPECTED = -9,
  ERR_ACCESS_DENIED = -10,
  ERR_NOT_IMPLEMENTED = -11,
  ERR_CONNECTION_CLOSED = -100,
  ERR_CONNECTION_RESET = -101,
  ERR_CONNECTION_REFUSED = -102,
  ERR_CONNECTION_ABORTED = -103,
  ERR_CONNECTION_FAILED = -104,
  ERR_NAME_NOT_RESOLVED = -105,
  ERR_INTERNET_DISCONNECTED = -106,
  ERR_SSL_PROTOCOL_ERROR = -107,
  ERR_ADDRESS_INVALID = -108,
  ERR_ADDRESS_UNREACHABLE = -109,
  ERR_SSL_CLIENT_AUTH_CERT_NEEDED = -110,
  ERR_TUNNEL_CONNECTION_FAILED = -111,
  ERR_NO_SSL_VERSIONS_ENABLED = -112,
  ERR_SSL_VERSION_OR_CIPHER_MISMATCH = -113,
  ERR_SSL_RENEGOTIATION_REQUESTED = -114,
  ERR_CERT_COMMON_NAME_INVALID = -200,
  ERR_CERT_DATE_INVALID = -201,
  ERR_CERT_AUTHORITY_INVALID = -202,
  ERR_CERT_CONTAINS_ERRORS = -203,
  ERR_CERT_NO_REVOCATION_MECHANISM = -204,
  ERR_CERT_UNABLE_TO_CHECK_REVOCATION = -205,
  ERR_CERT_REVOKED = -206,
  ERR_CERT_INVALID = -207,
  ERR_CERT_END = -208,
  ERR_INVALID_URL = -300,
  ERR_DISALLOWED_URL_SCHEME = -301,
  ERR_UNKNOWN_URL_SCHEME = -302,
  ERR_TOO_MANY_REDIRECTS = -310,
  ERR_UNSAFE_REDIRECT = -311,
  ERR_UNSAFE_PORT = -312,
  ERR_INVALID_RESPONSE = -320,
  ERR_INVALID_CHUNKED_ENCODING = -321,
  ERR_METHOD_NOT_SUPPORTED = -322,
  ERR_UNEXPECTED_PROXY_AUTH = -323,
  ERR_EMPTY_RESPONSE = -324,
  ERR_RESPONSE_HEADERS_TOO_BIG = -325,
  ERR_CACHE_MISS = -400,
  ERR_INSECURE_RESPONSE = -501,
};

// Structure representing menu information.
typedef struct _cef_handler_menuinfo_t
{
  int typeFlags;
  int x;
  int y;
  const wchar_t* linkUrl;
  const wchar_t* imageUrl;
  const wchar_t* pageUrl;
  const wchar_t* frameUrl;
  const wchar_t* selectionText;
  const wchar_t* misspelledWord;
  int editFlags;
  const wchar_t* securityInfo;
} cef_handler_menuinfo_t;

// The cef_handler_menuinfo_t typeFlags value will be a combination of the
// following values.
enum cef_handler_menutypebits_t
{
  // No node is selected
  MENUTYPE_NONE = 0x0,
  // The top page is selected
  MENUTYPE_PAGE = 0x1,
  // A subframe page is selected
  MENUTYPE_FRAME = 0x2,
  // A link is selected
  MENUTYPE_LINK = 0x4,
  // An image is selected
  MENUTYPE_IMAGE = 0x8,
  // There is a textual or mixed selection that is selected
  MENUTYPE_SELECTION = 0x10,
  // An editable element is selected
  MENUTYPE_EDITABLE = 0x20,
  // A misspelled word is selected
  MENUTYPE_MISSPELLED_WORD = 0x40,
  // A video node is selected
  MENUTYPE_VIDEO = 0x80,
  // A video node is selected
  MENUTYPE_AUDIO = 0x100,
};

// The cef_handler_menuinfo_t editFlags value will be a combination of the
// following values.
enum cef_handler_menucapabilitybits_t
{
  MENU_CAN_DO_NONE = 0x0,
  MENU_CAN_UNDO = 0x1,
  MENU_CAN_REDO = 0x2,
  MENU_CAN_CUT = 0x4,
  MENU_CAN_COPY = 0x8,
  MENU_CAN_PASTE = 0x10,
  MENU_CAN_DELETE = 0x20,
  MENU_CAN_SELECT_ALL = 0x40,
  MENU_CAN_GO_FORWARD = 0x80,
  MENU_CAN_GO_BACK = 0x100,
};

// Supported menu ID values.
enum cef_handler_menuid_t
{
  MENU_ID_NAV_BACK = 10,
  MENU_ID_NAV_FORWARD = 11,
  MENU_ID_NAV_RELOAD = 12,
  MENU_ID_NAV_STOP = 13,
  MENU_ID_UNDO = 20,
  MENU_ID_REDO = 21,
  MENU_ID_CUT = 22,
  MENU_ID_COPY = 23,
  MENU_ID_PASTE = 24,
  MENU_ID_DELETE = 25,
  MENU_ID_SELECTALL = 26,
  MENU_ID_PRINT = 30,
  MENU_ID_VIEWSOURCE = 31,
};

// Post data elements may represent either bytes or files.
enum cef_postdataelement_type_t
{
  PDE_TYPE_EMPTY  = 0,
  PDE_TYPE_BYTES,
  PDE_TYPE_FILE,
};


#ifdef __cplusplus
}
#endif

#endif // _CEF_TYPES_H
