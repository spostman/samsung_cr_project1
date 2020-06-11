// Code review content development project.
// Code style follows Google C++ Style Guide.
// (https://google.github.io/styleguide/cppguide.html)

#ifndef CHATCLIENT_HTTPREQUESTER_H_
#define CHATCLIENT_HTTPREQUESTER_H_

#include <random>

#include "cpprest/http_client.h"

// This class is a utility class designed to make HTTP requests to
// a given chat server.
// It internally calls http_client of cpprest library to
// generate and send HTTP requests.
// The class is NOT thread-safe.
// Example:
//   HttpRequester http_requester(chat_server_url);
//   http_response = http_requester.MakeHttpRequestForResponse(...);

namespace chatclient {

  class HttpRequester {
   public:
    HttpRequester(utility::string_t chat_server_url);

    // Make an HTTP request to the chat server using given parameters.
    // This function process failure of HTTP request
    // by calling ProcessHttpResponseFailure.
    // This function returns whether the HTTP request was a success.
    web::http::status_code MakeHttpRequest(
        utility::string_t http_method,
        utility::string_t query_url) const;

    // Make an HTTP request to the chat server using given parameters.
    // This function process failure of HTTP request
    // by calling ProcessHttpResponseFailure.
    // This function returns http_response.
    web::http::http_response MakeHttpRequestForResponse(
        utility::string_t http_method,
        utility::string_t query_url) const;

    // Hash string.
    utility::string_t HashString(utility::string_t string) const;

    // Generate random nonce.
    utility::string_t GenerateNonce();

   private:
    // This function print error message when an HTTP request fails
    void ProcessHttpResponseFailure(web::http::http_response) const;

    // HTTP_client: can make an HTTP request to a chat server
    std::unique_ptr<web::http::client::http_client> http_client_;

    // Chat server URL
    utility::string_t chat_server_url_;

    // Internal members to generate randomized identifiers.
    std::mt19937 rand_;
    std::uniform_int_distribution<> session_generator_;
  };

} // namespace chatclient

#endif CHATCLIENT_HTTPREQUESTER_H_ // CHATCLIENT_HTTPREQUESTER_H_