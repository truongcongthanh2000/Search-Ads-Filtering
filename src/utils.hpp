#pragma once

#include <string>
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <map>

namespace utils {
static const std::string white_space_chars = " \t\n\r\f\v";

inline std::string ltrim(std::string s, const std::string& chars = white_space_chars) {
    s.erase(0, s.find_first_not_of(chars));
    return s;
}

inline std::string rtrim(std::string s, const std::string& chars = white_space_chars) {
    s.erase(s.find_last_not_of(chars) + 1);
    return s;
}

inline std::string trim(std::string s, const std::string& chars = white_space_chars) {
    return ltrim(rtrim(s, chars), chars);
}

std::string time() {
    // Get current time
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    // Convert time to string
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}


std::string decode_url(const std::string& url) {
    std::stringstream decoded;
    size_t length = url.length();
    for (size_t i = 0; i < length; ++i) {
        if (url[i] == '%') {
            if (i + 2 < length) {
                // Extract hexadecimal value after '%'
                std::string hex = url.substr(i + 1, 2);
                try {
                    // Convert hexadecimal to integer
                    int value = std::stoi(hex, nullptr, 16);
                    decoded << static_cast<char>(value);
                    i += 2; // Move to next character after '%XX'
                } catch (const std::invalid_argument& e) {
                    // If conversion fails, append '%' and continue
                    decoded << url[i];
                }
            } else {
                // If '%' is at the end of string or not enough characters to decode
                decoded << url[i];
            }
        } else if (url[i] == '+') {
            // Replace '+' with space
            decoded << ' ';
        } else {
            // Append normal characters
            decoded << url[i];
        }
    }
    return decoded.str();
}

void parseURL(const std::string& url, std::string& uri, std::map<std::string, std::string>& params) {
    // Decode the URL
    std::string decoded_url = utils::decode_url(url);

    // Find the position of the '?' character which separates the URI and the query string
    size_t pos = decoded_url.find('?');

    // If there is no '?' character, the entire URL is considered as the URI
    if (pos == std::string::npos) {
        uri = decoded_url;
        return;
    }

    // Extract the URI part
    uri = decoded_url.substr(0, pos);

    // Extract the query string part
    std::string queryString = decoded_url.substr(pos + 1);

    // Split the query string into key-value pairs
    size_t start = 0;
    while (start < queryString.length()) {
        size_t end = queryString.find('&', start);

        if (end == std::string::npos) {
            end = queryString.length();
        }

        std::string param = queryString.substr(start, end - start);
        size_t eqPos = param.find('=');

        if (eqPos != std::string::npos) {
            std::string key = param.substr(0, eqPos);
            std::string value = param.substr(eqPos + 1);
            params[key] = value;
        }

        start = end + 1;
    }
}


} // namespace utils
