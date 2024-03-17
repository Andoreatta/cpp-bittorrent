#include "encode.hpp"
#include <sstream>

std::string encode_to_bencoded_string(const json &json_value)
{
        std::stringstream bencoded_string;
        if (json_value.is_string())
        {
                std::stringstream temp;
                temp << json_value.get<std::string>().size() << ":" << json_value.get<std::string>();
                bencoded_string << temp.str();
        }
        else if (json_value.is_number_integer())
        {
                std::stringstream temp;
                temp << "i" << json_value.get<int64_t>() << "e";
                bencoded_string << temp.str();
        }
        else if (json_value.is_array())
        {
                bencoded_string << "l";
                for (const auto &element : json_value)
                {
                        bencoded_string << encode_to_bencoded_string(element);
                }
                bencoded_string << "e";
        }
        else if (json_value.is_object())
        {
                bencoded_string << "d";
                for (const auto &[key, value] : json_value.items())
                {
                        bencoded_string << encode_to_bencoded_string(key);
                        bencoded_string << encode_to_bencoded_string(value);
                }
                bencoded_string << "e";
        }
        else
        {
                throw std::invalid_argument("Invalid json type");
        }

        return bencoded_string.str();
}