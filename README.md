# samsung_cr_project1

Versions
 - visual studio 2017
 - spdlog-1.5.0
 - googletest-release-1.8.1
 - cpprestsdk.v.141

ToDo
 - Install cpprestsdk for every projects without google_test_framework.
 - Modify basic_types.h of cpprestsdk for every visual studio projects in External Dependencies. It can crash with googletest and spdlog.
 - basic_types.h: line 83 ~ 88 should be modified as follows.
 ```cpp
    #ifndef _TURN_OFF_PLATFORM_STRING
	// The 'UU' macro can be used to create a string or character literal of the platform type, i.e. utility::char_t.
	// If you are using a library causing conflicts with 'U' macro, it can be turned off by defining the macro
	// '_TURN_OFF_PLATFORM_STRING' before including the C++ REST SDK header files, and e.g. use '_XPLATSTR' instead.
	#define UU(x) _XPLATSTR(x)
	#endif // !_TURN_OFF_PLATFORM_STRING
 ```
