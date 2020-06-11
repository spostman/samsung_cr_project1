// Code review content development project.
// Code style follows Google C++ Style Guide.
// (https://google.github.io/styleguide/cppguide.html)

#include "chat_room_view.h"

#include "spdlog/spdlog.h"

using namespace std;
using ::chatserver::ChatMessage;
using ::utility::string_t;
using ::utility::ostringstream_t;

namespace chatclient {

  string_t ChatRoomView::GetUserInput(const string_t output_message) const {
    string_t user_input;
    ucout << output_message;
    getline(ucin, user_input);
    return user_input;
  }

  void ChatRoomView::DisplayMessage(string_t display_message) {
    const lock_guard<mutex> lock(mutex_console_);
    ucout << display_message << endl;
  }

  void ChatRoomView::DisplayChatMessages(const list<ChatMessage>& chat_message,
                                         string_t current_chat_room_,
                                         int max_display_chat_message) {
    const lock_guard<mutex> lock(mutex_console_);

    // Save current cursor position.
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO console_screen_buffer_info;
    GetConsoleScreenBufferInfo(console, &console_screen_buffer_info);
    const COORD current_cursor_position = 
        console_screen_buffer_info.dwCursorPosition;

    // Move the cursor position where the chat message will be displayed.
    COORD set_curosr_position;
    set_curosr_position.X = 0;
    set_curosr_position.Y = 0;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), 
        set_curosr_position);
    
    // Clear console screen where the chat messages are displayed.
    console = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(console, &console_screen_buffer_info);
    COORD cursor_position = console_screen_buffer_info.dwCursorPosition;
    const COORD start_position = {0, 1};
    DWORD written;
    GetConsoleScreenBufferInfo(console, &console_screen_buffer_info);
    FillConsoleOutputCharacterA(
        console,
        ' ',
        console_screen_buffer_info.dwSize.X * (max_display_chat_message+ 1),
        start_position, &written);
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursor_position);
    // End to clear console screen where the chat messages are displayed.

    // Display chat messages.
    ucout << "---------- Chat messages in [" << current_chat_room_
          << "] ----------" << endl;

    for (ChatMessage message : chat_message) {
      tm time;
      localtime_s(&time, &message.date);
      char buffer[32];
      strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", &time);
      ucout << "[" << buffer << "] <" << message.user_id
            << "> " << message.chat_message << endl;
    }
    // Restore start cursor position.
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), current_cursor_position);
  }

  void ChatRoomView::ClearConsole() {
    const lock_guard<mutex> lock(mutex_console_);
    const HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO console_screen_buffer_info;
    GetConsoleScreenBufferInfo(console, &console_screen_buffer_info);
    const COORD topLeft = {0, 0};
    DWORD written;
    GetConsoleScreenBufferInfo(console, &console_screen_buffer_info);
    FillConsoleOutputCharacterA(
        console,
        ' ',
        console_screen_buffer_info.dwSize.X *
        console_screen_buffer_info.dwSize.Y,
        topLeft, &written);
  }

  void ChatRoomView::ClearConsole(const short start_y, const short end_y) {
    const lock_guard<mutex> lock(mutex_console_);
    const HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO console_screen_buffer_info;
    GetConsoleScreenBufferInfo(console, &console_screen_buffer_info);
    const COORD current_cursor_position =
        console_screen_buffer_info.dwCursorPosition;
    const COORD start_position = {0, start_y};
    DWORD written;
    GetConsoleScreenBufferInfo(console, &console_screen_buffer_info);
    FillConsoleOutputCharacterA(
        console,
        ' ',
        console_screen_buffer_info.dwSize.X * (end_y - start_y + 1),
        start_position, &written);

    COORD current_position;
    current_position.X = current_cursor_position.X;
    current_position.Y = current_cursor_position.Y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), current_position);
  }

  void ChatRoomView::SetCursorPosition(const short x, const short y) {
    const lock_guard<mutex> lock(mutex_console_);
    COORD current_position;
    current_position.X = x;
    current_position.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), current_position);
  }

} // namespace chatclient