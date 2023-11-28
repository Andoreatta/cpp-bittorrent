// C++ 20
#include <iostream>
#include <string>
#include <string_view>
#include <charconv>
#include "lib/nlohmann/json.hpp"
#include "lib/bencode/decode.hpp"

int main(int argc, char *argv[])
{
        // If there are less than 3 arguments or the first argument is not "decode", print usage and exit
        if (argc < 3 || std::string_view(argv[1]) != "decode")
        {
                std::cerr << "Usage: " << argv[0] << " decode <encoded_value>" << std::endl;
                return 1;
        }

        std::string_view encoded_value(argv[2]);

        json decoded_value;
        try
        {
                switch (encoded_value.front())
                {
                case 'i':
                {
                        auto [value, _] = decode_bencoded_integer(encoded_value);
                        decoded_value = value;
                        break;
                }
                case 'l':
                {
                        auto [value, _] = decode_bencoded_list(encoded_value);
                        decoded_value = value;
                        break;
                }
                default:
                        if (std::isdigit(encoded_value.front()))
                        {
                                auto [value, _] = decode_bencoded_string(encoded_value);
                                decoded_value = value;
                        }
                        else
                        {
                                throw std::invalid_argument("Invalid bencode type");
                        }
                }
        }
        catch (const std::invalid_argument &e)
        {
                std::cerr << "Error decoding bencoded value: " << e.what() << std::endl;
                return 1;
        }

        std::cout << decoded_value.dump() << std::endl;

        return 0;
}