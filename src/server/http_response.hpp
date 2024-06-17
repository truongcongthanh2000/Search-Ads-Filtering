#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include "http_message.hpp"

namespace http_message {

class HttpResponse : public HttpMessage {
private:
    HttpStatus status_;
    std::string reason_;

public:
    HttpResponse(HttpStatus status = HttpStatus::OK) {}

    HttpStatus get_status() const {
        return this->status_;
    }

    void set_status(HttpStatus status) {
        this->status_ = status;
    }

    std::string get_reason() const {
        return this->reason_;
    }

    void set_reason(const std::string &reason) {
        this->reason_ = reason;
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << http_version_to_string(HttpVersion::HTTP_1_1) << ' ';
        oss << static_cast<int>(status_) << ' ' << http_status_to_string(status_) << "\r\n";
        oss << this->get_header_string();
        if (!body_.empty()) {
            oss << "\r\n" << body_;
        }
        return oss.str();
    }
};

} // namespace http_message

#endif // HTTP_RESPONSE_HPP
