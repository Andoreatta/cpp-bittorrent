// C++ 20
#include <iostream>
#include <string>
#include <string_view>
#include <charconv>
#include "lib/nlohmann/json.hpp"
#include "lib/bencode/decode.hpp"

int main(int argc, char *argv[])
{
        if (argc < 3)
        {
                std::cerr << "Usage: " << argv[0] << " <command> <encoded_value>" << std::endl;
                return 1;
        }

        std::string_view command(argv[1]);
        std::string_view encoded_value(argv[2]);

        if (command == "decode")
        {
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
                        case 'd':
                        {
                                auto [value, _] = decode_bencoded_dictionary(encoded_value);
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
        }
        else if (command == "info")
        {
                // Handle info command
        }
        else
        {
                std::cerr << "Unknown command: " << command << std::endl;
                std::cerr << "Usage: " << argv[0] << " <command> <argument>" << std::endl;
                return 1;
        }

        return 0;
}