# samsung_cr_project1
Implementing C++ chat server/client using REST API.

# System environments
 - visual studio 2017 (Debug/Release, x86)
 - spdlog-1.5.0
 - googletest-release-1.8.1
 - cpprestsdk.v.141

# Cautions
 - Please use Debug/Release, x86 mode in Visual Studio, when you run this project.
 - When adding a new class to chat_client and chat_server, please add the class name in <Configuration Properties → Linker → Input → Additional Dependencies> of its test project.
 - Build the chat_client, chat_server project before running the test.
 
# ToDo: When you re-install or update cpprestsdk
 - Modify basic_types.h of cpprestsdk for every visual studio project in External Dependencies. It can crash with googletest and spdlog.
 - basic_types.h: line 83 ~ 88 should be modified as follows.
 ```cpp
    #ifndef _TURN_OFF_PLATFORM_STRING
	// The 'UU' macro can be used to create a string or character literal of the platform type, i.e. utility::char_t.
	// If you are using a library causing conflicts with 'U' macro, it can be turned off by defining the macro
	// '_TURN_OFF_PLATFORM_STRING' before including the C++ REST SDK header files, and e.g. use '_XPLATSTR' instead.
	#define UU(x) _XPLATSTR(x)
	#endif // !_TURN_OFF_PLATFORM_STRING
 ```
