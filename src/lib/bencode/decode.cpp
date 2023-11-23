#include "decode.hpp"
#include "utils.hpp"

// transforms a bencoded string (e.g. 5:hello) into a json string (e.g. "hello")
json decode_bencoded_string(std::string_view encoded_value)
{
        if (!std::isdigit(encoded_value.front()))
        {
                throw std::invalid_argument("No integer found at first part");
        }

        size_t colon_index = encoded_value.find(':');
        if (colon_index == std::string::npos)
        {
                throw std::invalid_argument("No colon found");
        }

        uint64_t number = string_to_uint64(encoded_value.substr(0, colon_index));
        std::string_view extracted_string = encoded_value.substr(colon_index + 1, number);

        return json(extracted_string);
}

// transforms a bencoded integer (e.g. i52e) into a json number (e.g. 52)
json decode_bencoded_integer(std::string_view encoded_value)
{
        if (encoded_value.front() != 'i' || encoded_value.back() != 'e')
        {
                throw std::invalid_argument("Invalid bencode signaling");
        }

        uint64_t number = string_to_uint64(encoded_value.substr(1, encoded_value.size() - 2));

        return json(number);
}

// transforms a bencoded list (e.g. l15:hellohellohelloi565582ee) into a json array (e.g. ["hellohellohello", 565582])
json decode_bencoded_list(std::string_view encoded_value)
{
        if (encoded_value.front() != 'l' || encoded_value.back() != 'e')
        {
                throw std::invalid_argument("Invalid bencode signaling");
        }

        json result = json::array();
        size_t current_index = {1};

        while (current_index < encoded_value.size() - 1) // -1 because we don't want to read the 'e' at the end
        {
                result.push_back(extract_and_decode(encoded_value, current_index, encoded_value[current_index]));
        }

        return result;
}


json extract_and_decode(std::string_view str, size_t &index, char type)
{
        size_t end_index;
        json result;

        switch (type)
        {
        case 'i': // bencoded integer
                end_index = find_next(str, 'e', index);
                result = decode_bencoded_integer(str.substr(index, end_index + 1 - index));
                index = end_index + 1;
                break;
        case 'l':                           // bencoded list
                end_index = str.rfind('e'); // find matching 'e'
                result = decode_bencoded_list(str.substr(index, end_index + 1 - index));
                index = end_index + 1;
                break;
        default: // bencoded string
                size_t colon_index = find_next(str, ':', index);
                uint64_t length = string_to_uint64(str.substr(index, colon_index - index));
                result = decode_bencoded_string(str.substr(index, colon_index + length + 1 - index));
                index = colon_index + length + 1;
                break;
        }

        return result;
}

