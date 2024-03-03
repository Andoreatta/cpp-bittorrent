#pragma once
#include <string_view>
#include "../nlohmann/json.hpp"

using json = nlohmann::json;

std::pair<std::string_view, size_t> decode_bencoded_string(std::string_view encoded_value);
std::pair<int64_t, size_t> decode_bencoded_integer(std::string_view encoded_value);
std::pair<json, size_t> decode_bencoded_list(std::string_view encoded_value);
std::pair<json, size_t> decode_bencoded_dictionary(std::string_view encoded_value);