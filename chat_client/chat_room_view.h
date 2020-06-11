// Code review content development project.
// Code style follows Google C++ Style Guide.
// (https://google.github.io/styleguide/cppguide.html)

#ifndef CHATCLIENT_CHATROOMVIEW_H_
#define CHATCLIENT_CHATROOMVIEW_H_

#include <list>
#include <mutex>
#include <windows.h>

#include "chat_message.h"
#include "cpprest/details/basic_types.h"

// Process every console display and input of ChatRoom.
// Please use this class to display a message and get user input of ChatRoom.
// Create mutex_lock on functions that can be called simultaneously by ChatRoom.
// Mutex_lock is applied to functions that can be called
// by multiple threads of ChatRoom at the same time.
// Functions that erase the screen message and change the position of
// the console cursor have the same mutex lock (mutex_console_).
//  - ClearConsole(), SetCursorPosition()
// DisplayChatMessages() also use mutex lock (mutex_chat_display_).

namespace chatclient {

  class ChatRoomView {
   public:
    // Get user console input while displaying the display_message.
    utility::string_t GetUserInput(utility::string_t display_message) const;

    // Display display_message on the console screen.
    void DisplayMessage(utility::string_t display_message);

    // Display the given chat room list on the console screen.
    // Use mutex_chat_display_ to make mutex lock.
    void DisplayChatMessages(
        const std::list<chatserver::ChatMessage>& chat_message,
        utility::string_t current_chat_room,
        int max_display_chat_message);

    // Clear every console message.
    // Use mutex_console_ to make mutex lock.
    void ClearConsole();

    // Clear console message between start_y and end_y lines.
    // Use mutex_console_ to make mutex lock.
    void ClearConsole(short start_y, short end_y);

    // Set the position of the console input cursor.
    // Use mutex_console_ to make mutex lock.
    void SetCursorPosition(short y, short x);

   private:
    // Mutex for control the console screen.
    // Used: ClearConsole, SetCursorPosition, DisplayChatMessages,
    //       DisplayMessage
    std::mutex mutex_console_;
  };

} // namespace chatclient

#endif // CHATCLIENT_CHATROOMVIEW_H_
