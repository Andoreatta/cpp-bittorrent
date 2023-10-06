#include <iostream>
#include <string>
#include <cctype>
#include <cstdlib>

#include "lib/nlohmann/json.hpp"

using json = nlohmann::json;

json decode_bencoded_value(const std::string& encoded_value) {
    if (!std::isdigit(encoded_value[0])) {
        throw std::invalid_argument("Invalid encoded value: " + encoded_value);
    }

    size_t colon_index = encoded_value.find(':');
    if (colon_index == std::string::npos) {
        throw std::invalid_argument("Invalid encoded value: " + encoded_value);
    }

    std::string number_string = encoded_value.substr(0, colon_index);
    int64_t number = std::atoll(number_string.c_str());

    std::string str = encoded_value.substr(colon_index + 1, number);
    return json(str);
}


int main(int argc, char* argv[]) {
    if (argc < 3 || std::string(argv[1]) != "decode") {
        std::cerr << "Usage: " << argv[0] << " decode <encoded_value>" << std::endl;
        return 1;
    }

    std::string encoded_value = argv[2];
    json decoded_value = decode_bencoded_value(encoded_value);
    std::cout << decoded_value.dump() << std::endl;

    return 0;
}