#pragma once
#include <string_view>
#include <cstdint>

size_t find_next(std::string_view str, char ch, size_t start);
uint64_t string_to_uint64(std::string_view str);