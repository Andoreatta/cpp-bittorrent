#include "decode.hpp"
#include "utils.hpp"

// transforms a bencoded string (e.g. 5:hello) into a json string (e.g. "hello")
std::pair<std::string_view, size_t> decode_bencoded_string(std::string_view encoded_value)
{
        if (!std::isdigit(encoded_value.front()))
        {
                throw std::invalid_argument("No integer found at first part");
        }

        size_t colon_index = encoded_value.find_first_of(':');
        if (colon_index == std::string::npos)
        {
                throw std::invalid_argument("No colon found");
        }

        uint64_t string_length = string_to_uint64(encoded_value.substr(0, colon_index));

        std::string_view extracted_string = encoded_value.substr(colon_index + 1, string_length);

        return {extracted_string, colon_index + 1 + string_length};
}

// transforms a bencoded integer (e.g. i-52e) into a json number (e.g. -52)
std::pair<int64_t, size_t> decode_bencoded_integer(std::string_view encoded_value)
{
        if (encoded_value.front() != 'i')
        {
                throw std::invalid_argument("Invalid bencode signaling");
        }

        size_t end_index = encoded_value.find_first_of('e');
        if (end_index == std::string::npos)
        {
                throw std::invalid_argument("No end of integer bencode found");
        }

        int64_t extracted_integer = string_to_int64(encoded_value.substr(1, end_index - 1));

        return {extracted_integer, end_index + 1};
}

// transforms a bencoded list (e.g. l5:helloi52el7:goodbyei21eee) into a json array (e.g. ["hello", 52, ["goodbye", 21]])
std::pair<json, size_t> decode_bencoded_list(std::string_view encoded_value)
{
        if (encoded_value.front() != 'l')
        {
                throw std::invalid_argument("Invalid bencode signaling");
        }

        json decoded_list = json::array();
        size_t current_index = 1;

        while (current_index < encoded_value.size())
        {
                switch (encoded_value[current_index])
                {
                case 'i':
                {
                        auto [decoded_integer, integer_length] = decode_bencoded_integer(encoded_value.substr(current_index));
                        decoded_list.push_back(decoded_integer);
                        current_index += integer_length;
                        break;
                }
                case 'l':
                {
                        auto [decoded_nested_list, nested_list_length] = decode_bencoded_list(encoded_value.substr(current_index));
                        decoded_list.push_back(decoded_nested_list);
                        current_index += nested_list_length;
                        break;
                }
                case 'e':
                {
                        return {decoded_list, current_index + 1};
                }
                default:
                {
                        if (std::isdigit(encoded_value[current_index]))
                        {
                                auto [decoded_string, string_length] = decode_bencoded_string(encoded_value.substr(current_index));
                                decoded_list.push_back(decoded_string);
                                current_index += string_length;
                        }
                        else
                        {
                                throw std::invalid_argument("Invalid character in bencoded list");
                        }
                        break;
                }
                }
        }

        throw std::invalid_argument("No end of list bencode found");
}