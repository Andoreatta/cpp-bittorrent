#pragma once
#include <string_view>
#include "../nlohmann/json.hpp"

using json = nlohmann::json;

json decode_bencoded_string(std::string_view encoded_value);
json decode_bencoded_integer(std::string_view encoded_value);
json decode_bencoded_list(std::string_view encoded_value);
json extract_and_decode(std::string_view str, size_t &index, char type);