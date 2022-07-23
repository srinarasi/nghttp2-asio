/*
 * nghttp2 - HTTP/2 C Library
 *
 * Copyright (c) 2013 Tatsuhiro Tsujikawa
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef HTTP2_H
#define HTTP2_H

#include "nghttp2_config.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <array>

#include <nghttp2/nghttp2.h>

#include "url-parser/url_parser.h"

#include "util.h"
#include "template.h"
#include "allocator.h"

namespace nghttp2 {

namespace http2 {

// Returns reason-phrase for given |status code|.  If there is no
// known reason-phrase for the given code, returns empty string.
StringRef get_reason_phrase(unsigned int status_code);

// Returns string version of |status_code|. (e.g., "404")
StringRef stringify_status(BlockAllocator &balloc, unsigned int status_code);

// Copies the |field| component value from |u| and |url| to the
// |dest|. If |u| does not have |field|, then this function does
// nothing.
void copy_url_component(std::string &dest, const http_parser_url *u, int field,
                        const char *url);

// Creates nghttp2_nv using |name| and |value| and returns it. The
// returned value only references the data pointer to name.c_str() and
// value.c_str().  If |no_index| is true, nghttp2_nv flags member has
// NGHTTP2_NV_FLAG_NO_INDEX flag set.
nghttp2_nv make_nv(const std::string &name, const std::string &value,
                   bool no_index = false);

nghttp2_nv make_nv(const StringRef &name, const StringRef &value,
                   bool no_index = false);

// Create nghttp2_nv from string literal |name| and std::string
// |value|.
template <size_t N>
nghttp2_nv make_nv_ls(const char (&name)[N], const std::string &value) {
  return {(uint8_t *)name, (uint8_t *)value.c_str(), N - 1, value.size(),
          NGHTTP2_NV_FLAG_NO_COPY_NAME};
}

// Header fields to be indexed, except HD_MAXIDX which is convenient
// member to get maximum value.
//
// generated by genheaderfunc.py
enum {
  HD__AUTHORITY,
  HD__HOST,
  HD__METHOD,
  HD__PATH,
  HD__PROTOCOL,
  HD__SCHEME,
  HD__STATUS,
  HD_ACCEPT_ENCODING,
  HD_ACCEPT_LANGUAGE,
  HD_ALT_SVC,
  HD_CACHE_CONTROL,
  HD_CONNECTION,
  HD_CONTENT_LENGTH,
  HD_CONTENT_TYPE,
  HD_COOKIE,
  HD_DATE,
  HD_EARLY_DATA,
  HD_EXPECT,
  HD_FORWARDED,
  HD_HOST,
  HD_HTTP2_SETTINGS,
  HD_IF_MODIFIED_SINCE,
  HD_KEEP_ALIVE,
  HD_LINK,
  HD_LOCATION,
  HD_PROXY_CONNECTION,
  HD_SEC_WEBSOCKET_ACCEPT,
  HD_SEC_WEBSOCKET_KEY,
  HD_SERVER,
  HD_TE,
  HD_TRAILER,
  HD_TRANSFER_ENCODING,
  HD_UPGRADE,
  HD_USER_AGENT,
  HD_VIA,
  HD_X_FORWARDED_FOR,
  HD_X_FORWARDED_PROTO,
  HD_MAXIDX,
};

using HeaderIndex = std::array<int16_t, HD_MAXIDX>;

// Looks up header token for header name |name| of length |namelen|.
// Only headers we are interested in are tokenized.  If header name
// cannot be tokenized, returns -1.
int lookup_token(const uint8_t *name, size_t namelen);

// Constructs path by combining base path |base_path| with another
// path |rel_path|.  The base path and another path can have optional
// query component.  This function assumes |base_path| is normalized.
// In other words, it does not contain ".." or "."  path components
// and starts with "/" if it is not empty.
std::string path_join(const StringRef &base, const StringRef &base_query,
                      const StringRef &rel_path, const StringRef &rel_query);

StringRef path_join(BlockAllocator &balloc, const StringRef &base_path,
                    const StringRef &base_query, const StringRef &rel_path,
                    const StringRef &rel_query);

// true if response has body, taking into account the request method
// and status code.
bool expect_response_body(const std::string &method, int status_code);

// true if response has body, taking into account status code only.
bool expect_response_body(int status_code);

} // namespace http2

} // namespace nghttp2

#endif // HTTP2_H