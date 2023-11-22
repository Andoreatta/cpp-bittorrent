// C++ 20
#include <iostream>
#include <string>
#include <string_view>
#include <charconv>

#include "lib/nlohmann/json.hpp"

using json = nlohmann::json;

// stoll, but for std::string_view
int64_t string_to_int64(std::string_view str) {
        int64_t number;
        auto [p, ec] = std::from_chars(str.data(), str.data() + str.size(), number);    // returned type is from_chars_result struct
        if (ec != std::errc()) {
                throw std::invalid_argument("Invalid integer: " + std::string(str));
        }
        return number;
}


json decode_bencoded_string(std::string_view encoded_value)
{
        if (!std::isdigit(encoded_value[0]))
        {
                throw std::invalid_argument("No integer found at first part: " + std::string(encoded_value));
        }

        size_t colon_index = encoded_value.find(':');
        if (colon_index == std::string::npos)
        {
                throw std::invalid_argument("No colon found: " + std::string(encoded_value));
        }

        int64_t number = string_to_int64(encoded_value.substr(0, colon_index));
        std::string_view extracted_string = encoded_value.substr(colon_index + 1, number);

        return json(std::string(extracted_string));
}


json decode_bencoded_integer(std::string_view encoded_value)
{
        if (encoded_value.front() != 'i' || encoded_value.back() != 'e')
        {
                throw std::invalid_argument("Invalid bencode signaling: " + std::string(encoded_value));
        }

        // -2 to remove the 'i' and 'e' characters
        int64_t number = string_to_int64(encoded_value.substr(1, encoded_value.size() - 2));

        return json(number);
}


int main(int argc, char *argv[])
{
        // If there are less than 3 arguments or the first argument is not "decode", print usage and exit
        if (argc < 3 || std::string_view(argv[1]) != "decode")
        {
                std::cerr << "Usage: " << argv[0] << " decode <encoded_value>" << std::endl;
                return 1;
        }

        std::string_view encoded_value(argv[2]);

        json result;
        if (encoded_value.front() == 'i')
        {
                result = decode_bencoded_integer(encoded_value);
        }
        else
        {
                result = decode_bencoded_string(encoded_value);
        }

        std::cout << result.dump() << std::endl;

        return 0;
}