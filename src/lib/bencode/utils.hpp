#pragma once
#include <string_view>
#include <cstdint>

uint64_t string_to_uint64(std::string_view str);
int64_t string_to_int64(std::string_view str);
void print_hash_in_hex(const std::string &hash);