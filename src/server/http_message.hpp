#pragma once

#include <string>
#include <map>
#include <sstream>

namespace http_message {

enum class HttpMethod {
    GET,
    POST,
    PUT,
    DELETE
};

std::string http_method_to_string(HttpMethod method) {
    switch (method) {
        case HttpMethod::GET:
            return "GET";
        case HttpMethod::POST:
            return "POST";
        case HttpMethod::PUT:
            return "PUT";
        case HttpMethod::DELETE:
            return "DELETE";
        default:
            throw std::invalid_argument("Method not supported");
    }
}

HttpMethod string_to_http_method(const std::string& method) {
    if (method == "GET") {
        return HttpMethod::GET;
    } else if (method == "POST") {
        return HttpMethod::POST;
    } else if (method == "PUT") {
        return HttpMethod::PUT;
    } else if (method == "DELETE") {
        return HttpMethod::DELETE;
    } else {
        throw std::invalid_argument("Method not supported");
    }
}

enum class HttpVersion {
    HTTP_1_0,
    HTTP_1_1
};

std::string http_version_to_string(HttpVersion version) {
    switch (version) {
        case HttpVersion::HTTP_1_0:
            return "HTTP/1.0";
        case HttpVersion::HTTP_1_1:
            return "HTTP/1.1";
        default:
            throw std::invalid_argument("Version not supported");
    }
}

HttpVersion string_to_http_version(const std::string& version) {
    if (version == "HTTP/1.0") {
        return HttpVersion::HTTP_1_0;
    } else if (version == "HTTP/1.1") {
        return HttpVersion::HTTP_1_1;
    } else {
        throw std::invalid_argument("Version not supported");
    }
}

enum class HttpStatus {
    OK = 200,
    Created = 201,
    NoContent = 204,
    BadRequest = 400,
    Unauthorized = 401,
    Forbidden = 403,
    NotFound = 404,
    MethodNotAllowed = 405,
    InternalServerError = 500
};

std::string http_status_to_string(HttpStatus status) {
    switch (status)
    {
        case HttpStatus::OK:
            return "OK";
        case HttpStatus::Created:
            return "Created";
        case HttpStatus::NoContent:
            return "No Content";
        case HttpStatus::BadRequest:
            return "Bad Request";
        case HttpStatus::Unauthorized:
            return "Unauthorized";
        case HttpStatus::Forbidden:
            return "Forbidden";
        case HttpStatus::NotFound:
            return "Not Found";
        case HttpStatus::MethodNotAllowed:
            return "Method Not Allowed";
        case HttpStatus::InternalServerError:
            return "Internal Server Error";
        default:
            throw std::invalid_argument("Invalid HTTP Status");
    }
}

HttpStatus string_to_http_status(const std::string &value) {
    if (value == "OK")
        return HttpStatus::OK;
    else if (value == "Created")
        return HttpStatus::Created;
    else if (value == "No Content")
        return HttpStatus::NoContent;
    else if (value == "Bad Request")
        return HttpStatus::BadRequest;
    else if (value == "Unauthorized")
        return HttpStatus::Unauthorized;
    else if (value == "Forbidden")
        return HttpStatus::Forbidden;
    else if (value == "Not Found")
        return HttpStatus::NotFound;
    else if (value == "Method Not Allowed")
        return HttpStatus::MethodNotAllowed;
    else if (value == "Internal Server Error")
        return HttpStatus::InternalServerError;
    else
        throw std::invalid_argument("Invalid HTTP Status");
}

// A base class for HTTP messages
class HttpMessage {
protected:
    HttpVersion version_;
    std::map<std::string, std::string> headers_;
    std::string body_;

public:
    HttpMessage() {}

    HttpVersion get_version() const {
        return this->version_;
    }

    void set_version(const HttpVersion& version) {
        this->version_ = version;
    }

    void set_version(const std::string& version) {
        this->version_ = string_to_http_version(version);
    }

    std::string get_header(const std::string& name) const {
        auto it = this->headers_.find(name);
        if (it != this->headers_.end()) {
            return it->second;
        } else {
            return "";
        }
    }

    void set_header(const std::string& name, const std::string& value) {
        this->headers_[name] = value;
    }

    std::string get_header_string() const {
        std::stringstream ss;
        for (const auto& pair : this->headers_) {
            ss << pair.first << ": " << pair.second << "\r\n";
        }

        if (!body_.empty() && get_header("Content-Length").empty()) {
            ss << "Content-Length: " << body_.size() << "\r\n";
        }

        return ss.str();
    }

    std::string get_body() const {
        return this->body_;
    }

    void set_body(const std::string& body) {
        this->body_ = body;
    }
};

} // namespace http_message

