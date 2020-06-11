// Code review content development project.
// Code style follows Google C++ Style Guide.
// (https://google.github.io/styleguide/cppguide.html)

#ifndef CHATSERVER_SESSION_H_
#define CHATSERVER_SESSION_H_

#include "cpprest/details/basic_types.h"

// Session information structure (session_id, user_id, last_activity_time).

namespace chatserver {

  struct Session {
    // Session id.
    utility::string_t session_id;

    // User id.
    utility::string_t user_id;

    // Last activity time of this session.
    time_t last_activity_time;
  };

} // namespace chatserver

#endif CHATSERVER_SESSION_H_ // CHATSERVER_SESSION_H_