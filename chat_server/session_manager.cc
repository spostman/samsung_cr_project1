// Code review content development project.
// Code style follows Google C++ Style Guide.
// (https://google.github.io/styleguide/cppguide.html)

#include "session_manager.h"

#include <chrono>
#include <future>

#include "cpprest/asyncrt_utils.h"
#include "spdlog/spdlog.h"

using namespace std;
using chrono::system_clock;
using chrono::duration;
using ::utility::conversions::to_utf8string;
using ::utility::string_t;
using ::spdlog::info;

namespace chatserver {

  // Default session alive time (second).
  const time_t kSessionAliveTime = 30;
  // Length of session id.
  const size_t kSessionLength = 32;
  // Period to check session expire (second).
  const time_t kSessionCheckInterval = 1;
  // Session seed.
  const string_t kSessionValue = UU(
      "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

  SessionManager::SessionManager() : SessionManager(kSessionAliveTime) {
  }

  SessionManager::SessionManager(time_t session_alive_time)
      : rand_(std::random_device{}()),
        session_generator_(0, kSessionValue.size() - 1),
        session_alive_time_(session_alive_time) {
  }

  SessionManager::~SessionManager() {
    run_thread_ = false;
  }

  bool SessionManager::IsExistSessionId(string_t session_id) {
    const lock_guard<mutex> lock(mutex_sessions_);

    if (sessions_.find(session_id) == sessions_.end()) {
      return false;
    } else {
      return true;
    }
  }

  Session SessionManager::CreateSession(string_t user_id) {
    const lock_guard<mutex> lock(mutex_sessions_);

    // If user ID exists, update session active time
    if (user_id_to_session_id_.find(user_id) != user_id_to_session_id_.end()) {
      sessions_[user_id_to_session_id_[user_id]].last_activity_time = 
          system_clock::to_time_t(system_clock::now());
      return sessions_[user_id_to_session_id_[user_id]];
    }

    Session new_session;
    new_session.session_id = GenerateSessionId();
    new_session.user_id = user_id;
    new_session.last_activity_time = system_clock::to_time_t(
        system_clock::now());

    sessions_[new_session.session_id] = new_session;
    user_id_to_session_id_[new_session.user_id] = new_session.session_id;
    return new_session;
  }

  bool SessionManager::DeleteSession(string_t session_id) {
    const lock_guard<mutex> lock(mutex_sessions_);
    if (sessions_.find(session_id) == sessions_.end()) {
      return false;
    } else {
      user_id_to_session_id_.erase(sessions_[session_id].user_id);
      sessions_.erase(session_id);
      return true;
    }
  }

  bool SessionManager::RenewLastActivityTime(string_t session_id) {
    const lock_guard<mutex> lock(mutex_sessions_);

    if (sessions_.find(session_id) == sessions_.end()) {
      return false;
    } else {
      sessions_[session_id].last_activity_time = system_clock::
         to_time_t(system_clock::now());
      return true;
    }
  }

  const string_t SessionManager::GenerateSessionId() {
    string_t result;
    for (size_t i = 0; i < kSessionLength; i++) {
      result += kSessionValue.at(session_generator_(rand_));
    }
    return result;
  }

  bool SessionManager::GetUserIDFromSessionId(string_t session_id, 
                                              string_t* out_user_id) {
    if (out_user_id == nullptr)
      return false;
    const lock_guard<mutex> lock(mutex_sessions_);

    if (user_id_to_session_id_.find(sessions_[session_id].user_id) == 
        user_id_to_session_id_.end()) {
      return false;
    } else {
      *out_user_id = sessions_[session_id].user_id;
      return true;
    }
  }

  void SessionManager::RunSessionExpireThread() {
    run_thread_ = true;
    // Getting return value is necessary to run async thread,
    // but the return value is not used.
    async_thread_result_ = async(launch::async,
        &SessionManager::CheckAndDeleteExpiredSession, this);
  }

  void SessionManager::CheckAndDeleteExpiredSession() {
    while (run_thread_) {
      // Remove expired sessions every kSessionCheckInterval
      this_thread::sleep_for(duration<int>(kSessionCheckInterval));
      {
        const lock_guard<mutex> lock(mutex_sessions_);
        auto current_time = system_clock::now();
        for (auto it = sessions_.cbegin(); it != sessions_.cend();) {
          duration<double> diff = current_time -
            system_clock::from_time_t(
                it->second.last_activity_time);
          if (diff.count() > session_alive_time_) {
            info("Delete expired session: {}", 
                to_utf8string(it->second.user_id));
            user_id_to_session_id_.erase(it->second.user_id);
            sessions_.erase(it++);
          } else {
            ++it;
          }
        }
      }
    }
  }

} // namespace chatserver
