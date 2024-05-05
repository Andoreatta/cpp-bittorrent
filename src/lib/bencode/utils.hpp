#pragma once
#include <string_view>
#include <cstdint>
#include <string>

uint64_t string_to_uint64(std::string_view str);
int64_t string_to_int64(std::string_view str);
std::string hash_to_hex_string(const std::string &piece_hash);