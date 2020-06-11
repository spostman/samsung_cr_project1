// Code review content development project.
// Code style follows Google C++ Style Guide.
// (https://google.github.io/styleguide/cppguide.html)

#ifndef CHATSERVER_SESSIONMANAGER_H_
#define CHATSERVER_SESSIONMANAGER_H_

#include <future>
#include <map>
#include <mutex>
#include <random>

#include "cpprest/details/basic_types.h"
#include "session.h"

// This class is designed to manage a session for each connected account.
// A session should be established after each account logins. If there is no
// activity during the specified alive time, a session is deleted automatically.
// All functions that access session_ have guarded with a mutex lock.
//
// Example:
//   SessionManager session_manager;
// Create a session
//   Session session = CreateSession("kaist");
// Run thread to remove expired sessions
//   session_manager.RunSessionExpireThread();
// If there exisit an activity in the session
//   if (RenewLastActivityTime(session.session_id)) {
//     Succeeds in renewing the last activity time for session.
//   } else {
//     Fail to renew the last activity time for session.
//   }
// Get user ID using session ID
//   string_t session_id;
//   if (session_manager.GetUserIDFromSessionId(session.session_id,
//                                             &session_id)) {
//     Succeeds! Prpcess with session_id.
//   } else {
//     Fail to get session_id.
//   }

namespace chatserver {

  class SessionManager {
   public:
    // Close the thread for the session expire when the class is ended.
    SessionManager();

    // Close the thread for the session expire when the class is ended.
    ~SessionManager();

    // Check the given session ID exists.
    bool IsExistSessionId(utility::string_t session_id);

    // Create a session ID for a given user ID.
    // If the user ID has session, renew session alive time.
    Session CreateSession(utility::string_t user_id);

    // Delete the given session with session_id.
    bool DeleteSession(utility::string_t session_id);

    // Update the last alive time for the session with a given session_id.
    bool RenewLastActivityTime(utility::string_t session_id);

    // Get the user ID corresponding to the given session ID.
    // Return false If the given ID has no session.
    bool GetUserIDFromSessionId(utility::string_t session_id, 
                                utility::string_t* out_user_id);

    // Execute thread that deletes sessions that are over alive time.
    void RunSessionExpireThread();
    
   private:
    // Create session id of length kSessionLength using alphabet and number.
    const utility::string_t GenerateSessionId();

    // Delete expired sessions every kSessionCheckInterval seconds.
    void CheckAndDeleteExpiredSession();

    // Store session information <session_id, Session>
    std::map<utility::string_t, Session> sessions_;

    // Store user_id mapping with session_id <user_id, session_id>
    std::map<utility::string_t, utility::string_t> user_id_to_session_id_;

    // Mutex for member variables: sessions_, user_id_to_session_id_
    std::mutex mutex_sessions_;

    // The variable to control the async thread for start and stop.
    bool run_thread_;

    // This variable is used to maintain the asynchronous state of the thread.
    std::future<void> async_thread_result_;

    // Internal members to generate randomized identifiers.
    std::mt19937 rand_;
    std::uniform_int_distribution<> session_generator_;
  };

} // namespace chatserver

#endif CHATSERVER_SESSIONMANAGER_H_ // CHATSERVER_SESSIONMANAGER_H_
