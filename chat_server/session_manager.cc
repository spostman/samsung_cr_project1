#include "session_manager.h"

#include <chrono>
#include <random>

#include "cpprest/asyncrt_utils.h"
#include "spdlog/spdlog.h"

using namespace std;
using ::chrono::system_clock;
using ::chrono::duration;
using ::utility::conversions::to_utf8string;
using ::utility::string_t;
using ::spdlog::info;

namespace chatserver {
  // Session alive time (second)
  const time_t kSessionAliveTime = 10;
  // Length of session id
  const size_t kSessionLength = 32;
  // Period to check session expire (second)
  const time_t kSessionCheckInterval = 3;

  bool SessionManager::IsExistSessionID(string_t session_id) {
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
      sessions_[user_id_to_session_id_[user_id]].session_id = 
          GenerateSessionID();
      sessions_[user_id_to_session_id_[user_id]].last_activity_time = 
          system_clock::to_time_t(system_clock::now());
      return sessions_[user_id_to_session_id_[user_id]];
    }

    Session new_session;
    new_session.session_id = GenerateSessionID();
    new_session.user_id = user_id;
    new_session.last_activity_time = system_clock::to_time_t(
        system_clock::now());

    sessions_[new_session.session_id] = new_session;
    user_id_to_session_id_[new_session.user_id] = new_session.session_id;
    return new_session;
  }

  bool SessionManager::DeleteSession(string_t session_id) {
    const lock_guard<mutex> lock(mutex_sessions_);
    if(sessions_.find(session_id) == sessions_.end()) {
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

  const string_t SessionManager::GenerateSessionID() const {
    static string_t session_values = UU(
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    random_device rd;
    mt19937 rand(rd());
    const uniform_int_distribution<> session_generator(
        0, session_values.size() - 1);

    string_t result;
    for (size_t i = 0; i < kSessionLength; i++) {
      result += session_values.at(session_generator(rand));
    }
    return result;
  }

  bool SessionManager::GetUserIDFromSessionID(string_t session_id, 
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
    thread t1(&SessionManager::CheckExpiredSession, this);
    t1.detach();
  }

  void SessionManager::CheckExpiredSession() {
    
    while (true) {
      // Remove expired session every kSessionCheckInterval
      this_thread::sleep_for(duration<int>(kSessionCheckInterval));
      {
        const lock_guard<mutex> lock(mutex_sessions_);
        info("Try to find expired sessions");
        auto current_time = system_clock::now();
        for (auto it = sessions_.cbegin(); it != sessions_.cend();) {
          duration<double> diff = current_time -
            system_clock::from_time_t(
                it->second.last_activity_time);
          if (diff.count() > kSessionAliveTime) {
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
