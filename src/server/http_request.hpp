#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <iostream>

#include "http_message.hpp"
#include "../utils.hpp"

namespace http_message {

class HttpRequest : public HttpMessage {
   private:
    HttpMethod method_;
    std::string uri_;
    std::string body_;
    std::string raw_request_str_;
    std::map<std::string, std::string> params_;

    const std::string& get(const std::string& key) const {
        static const std::string empty_string("");
        auto pos = params_.find(key);
        if (pos == params_.end()) {
            return empty_string;
        }
        return pos->second;
    }

   public:
    HttpRequest(std::string&& raw_string) : raw_request_str_(std::move(raw_string)) {
        parse_request();
    }

    HttpMethod get_method() const {
        return this->method_;
    }

    void set_method(HttpMethod method) {
        this->method_ = method;
    }

    std::string get_uri() const {
        return this->uri_;
    }

    void set_uri(const std::string& uri) {
        this->uri_ = uri;
    }

    const std::string& operator[](const std::string& key) const {
        return get(key);
    }

    void parse_request() {
        size_t start = 0, end = 0;
        std::string method_str, version_str;

        // parse the first line
        end = raw_request_str_.find("\r\n", start);
        if (end == std::string::npos) {
            throw std::invalid_argument("Invalid request string");
        }
        std::istringstream iss(raw_request_str_.substr(start, end - start));
        std::string uri_raw;
        iss >> method_str >> uri_raw >> version_str;
        method_ = string_to_http_method(method_str);
        version_ = string_to_http_version(version_str);
        utils::parseURL(uri_raw, uri_, params_);

        if (!iss.good() && !iss.eof()) {
            throw std::invalid_argument("Invalid request string");
        }

        // parse headers
        start = end + 2;  // skip \r\n
        end = raw_request_str_.find("\r\n\r\n", start);
        if (end != std::string::npos) {
            iss.clear();
            iss.str(raw_request_str_.substr(start, end - start));

            std::string line;
            std::string key, value;
            while (std::getline(iss, line)) {
                std::istringstream ss(line);
                std::getline(ss, key, ':');
                std::getline(ss, value);

                headers_[utils::trim(key)] = utils::trim(value);
            }
        }

        // parse body
        start = end + 4;  // skip \r\n\r\n
        end = raw_request_str_.length();
        if (start < end) {
            body_ = raw_request_str_.substr(start, end - start);
        }
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << http_method_to_string(method_) << " " << uri_ << " " << http_version_to_string(version_) << '\n';
        oss << this->get_header_string() << '\n';
        oss << body_ << std::endl;
        return oss.str();
    }
};

}  // namespace http_message

#endif // HTTP_REQUEST_HPP
