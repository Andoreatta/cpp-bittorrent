#pragma once
#include <string_view>
#include "../nlohmann/json.hpp"

using json = nlohmann::json;

std::string encode_to_bencoded_string(const json &json_value);