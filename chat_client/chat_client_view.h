// Code review content development project.
// Code style follows Google C++ Style Guide.
// (https://google.github.io/styleguide/cppguide.html)

#ifndef CHATCLIENT_CHATCLIENTVIEW_H_
#define CHATCLIENT_CHATCLIENTVIEW_H_

#include "cpprest/details/basic_types.h"
#include "cpprest/json.h"

// The class is designed to display chat messages and chat rooms on the stdout
// console. It also accepts a chat message from the stdin console.

namespace chatclient {

  class ChatClientView {
   public:
    // Get user console input while displaying the display_message.
    utility::string_t GetUserInput(utility::string_t display_message) const;

    // Display display_message on the stdout console.
    void DisplayMessage(utility::string_t display_message) const;

    // Display a given chat room list on the stdout screen.
    void DisplayChatRoomList(const web::json::array& chat_room_list) const;
  };

} // namespace chatclient

#endif // CHATCLIENT_CHATCLIENTVIEW_H_
