#ifndef CHATSERVER_SESSIONMANAGER_H_
#define CHATSERVER_SESSIONMANAGER_H_

#include <set>
#include <string>
#include <map>
#include <mutex>

#include "cpprest/details/basic_types.h"
#include "session.h"

namespace chatserver {

  // This class is designed to manage session for each connected accounts.
  // A session should be created when an account login.
  // A session is automatically deleted,
  // if there is no activity for the alive time after creation.
  // All functions that access session_ have a mutex lock.
  // Example:
  //   SessionManager session_manager;
  // Create a session
  //   Session session = CreateSession("kaist");
  // Run thread to remove expired sessions
  //   session_manager.RunSessionExpireThread();
  // If activity with account of "kaist"
  //   if(RenewLastActivityTime(session.session_id)) {
  //     do something to success to renew last activity time
  //   } else {
  //     do something to fail to renew last activity time
  //   }
  // Get user ID using session ID
  //   string_t session_id;
  //   if(session_manager.GetUserIDFromSessionID(session.session_id,
  //                                             &session_id)) {
  //     do something to success to get user ID
  //   } else {
  //     do something to fail to get user ID
  //   }

  class SessionManager {
  public:
    // Check the given session ID exists.
    bool IsExistSessionID(utility::string_t session_id);
    // Create a session ID by a user ID.
    // If the user ID has session, renew session alive time.
    Session CreateSession(utility::string_t user_id);
    // Delete the given session ID.
    bool DeleteSession(utility::string_t session_id);
    // Alive time is updated when an activity occurs in the given session.
    bool RenewLastActivityTime(utility::string_t session_id);
    // Get the user ID corresponding to the given session ID.
    // Return false If the given ID has no session.
    bool GetUserIDFromSessionID(utility::string_t session_id, 
                                utility::string_t* out_user_id);
    // Execute thread that deletes sessions that are over alive time
    void RunSessionExpireThread();

  private:
    // Create session id of length kSessionLength using alphabet and number
    const utility::string_t GenerateSessionID() const;
    // Delete expired sessions every kSessionCheckInterval seconds
    void CheckExpiredSession();

    // Store session information <session_id, Session>
    std::map<utility::string_t, Session> sessions_;
    // Store user_id mapping with session_id <user_id, session_id>
    std::map<utility::string_t, utility::string_t> user_id_to_session_id_;
    // Mutex for member variable, sessions_
    std::mutex mutex_sessions_;
  };
} // namespace chatserver
#endif CHATSERVER_SESSIONMANAGER_H_ // CHATSERVER_SESSIONMANAGER_H_
