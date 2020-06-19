// Code review content development project.
// Code style follows Google C++ Style Guide.
// (https://google.github.io/styleguide/cppguide.html)

#include "http_requester.h"

#include "chat_room.h"

using namespace std;
using ::web::uri;
using ::web::http::methods;
using ::web::http::http_response;
using ::web::http::status_codes;
using ::web::http::status_code;
using ::web::json::value;
using ::utility::string_t;
using ::utility::conversions::to_string_t;

namespace chatclient {

  // Length of nonce.
  const size_t kNonceLength = 10;
  // Nonce seed.
  const string_t kNonceValue = UU(
      "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

  HttpRequester::HttpRequester(const string_t chat_server_url)
      : chat_server_url_(chat_server_url),
        rand_(std::random_device{}()),
        session_generator_(0, kNonceValue.size() - 1) {
    http_client_ = make_unique<web::http::client::http_client>(
        web::http::uri_builder(chat_server_url).to_uri());
  }

  status_code HttpRequester::MakeHttpRequest(string_t http_method,
                                             string_t query_url) const {
    const http_response response = 
        http_client_->request(http_method,
                              uri::encode_uri(query_url)).get();
    if (response.status_code() != status_codes::OK) {
      ProcessHttpResponseFailure(response);
    }
    return response.status_code();
  }

  status_code HttpRequester::MakeHttpRequest(string_t http_method,
                                             string_t query_url,
                                             const value& body_data) const {
    const http_response response =
        http_client_->request(http_method,
            uri::encode_uri(query_url), body_data).get();
    if (response.status_code() != status_codes::OK) {
      ProcessHttpResponseFailure(response);
    }
    return response.status_code();
  }
  
  http_response HttpRequester::MakeHttpRequestForResponse(
      string_t http_method,
      string_t query_url) const {
    const http_response response = 
        http_client_->request(http_method, 
                              uri::encode_uri(query_url)).get();
    if (response.status_code() != status_codes::OK) {
      ProcessHttpResponseFailure(response);
    }
    return response;
  }

  http_response HttpRequester::MakeHttpRequestForResponse(
      string_t http_method,
      string_t query_url,
      const value& body_data) const {
    const http_response response = http_client_->request(
        http_method,
        uri::encode_uri(query_url), 
        body_data).get();

    if (response.status_code() != status_codes::OK) {
      ProcessHttpResponseFailure(response);
    }
    return response;
  }

  string_t HttpRequester::HashString(string_t string) const {
    return to_string_t(to_string(hash<string_t>{}(string)));
  }

  string_t HttpRequester::GenerateNonce() {
    string_t result;
    for (size_t i = 0; i < kNonceLength; i++) {
      result += kNonceValue.at(session_generator_(rand_));
    }
    return result;
  }

  void HttpRequester::ProcessHttpResponseFailure(
      const http_response& response) const {
    const string_t body =
        response.content_ready().get().extract_utf16string(true).get();
    ucout << "HTTP error response " << response.status_code();
    // If the body has an error message.
    if (body.length() > 0) {
      ucout << ": " << body;
    }
    ucout << endl;
  }

} // namespace chatclient