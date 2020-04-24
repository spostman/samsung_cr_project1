#ifndef CHATSERVER_SESSION_H_
#define CHATSERVER_SESSION_H_

#include "cpprest/details/basic_types.h"

namespace chatserver {
  // Session information structure (session_id, user_id, last_activity_time).
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