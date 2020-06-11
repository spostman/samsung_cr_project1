// Code review content development project.
// Code style follows Google C++ Style Guide.
// (https://google.github.io/styleguide/cppguide.html)

# include "chat_client_view.h"

using namespace std;
using ::utility::string_t;
using ::web::json::array;

namespace chatclient {

  string_t ChatClientView::GetUserInput(const string_t output_message) const {
    string_t user_input;
    ucout << output_message;
    getline(ucin, user_input);
    return user_input;
  }

  void ChatClientView::DisplayMessage(string_t display_message) const {
    ucout << display_message << endl;
  }

  void ChatClientView::DisplayChatRoomList(const ::array& chat_room_list) const {
    ucout << "chat room list -> ";
    for (auto i : chat_room_list) {
      ucout << i[UU("room")] << ", ";
    }
    ucout << endl;
  }

} // namespace chatclient