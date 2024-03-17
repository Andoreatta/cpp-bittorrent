// C++ 20
#include <iostream>
#include <string>
#include <string_view>
#include <fstream>
#include <charconv>
#include "lib/nlohmann/json.hpp"
#include "lib/bencode/decode.hpp"
#include "lib/bencode/encode.hpp"
#include "lib/hash/sha1.hpp"

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
                        return 1;
                        std::cerr << "Error decoding bencoded value: " << e.what() << std::endl;
                }

                std::cout << decoded_value.dump() << std::endl;
        }
        else if (command == "info")
        {
                std::ifstream input_file{argv[2], std::ios::binary};
                if (!input_file)
                {
                        std::cerr << "Error opening torrent file: " << argv[2] << std::endl;
                        return 1;
                }

                std::vector<char> file_data((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
                std::string_view file_data_view(file_data.data(), file_data.size());
                try
                {
                        auto [decoded_info, _] = decode_bencoded_dictionary(file_data_view);
                        std::string bencoded_string = encode_to_bencoded_string(decoded_info.at("info"));
                        SHA1 sha1;

                        std::cout << "Tracker URL: " << decoded_info.at("announce").get<std::string>() << std::endl;
                        std::cout << "Length: " << decoded_info.at("info").at("length").get<int64_t>() << std::endl;
                        std::cout << "Info Hash: " << sha1(bencoded_string) << std::endl;
                }
                catch (const std::invalid_argument &e)
                {
                        std::cerr << "Error decoding bencoded info dictionary: " << e.what() << std::endl;
                        return 1;
                }
        }
        else
        {
                std::cerr << "Unknown command: " << command << std::endl;
                std::cerr << "Usage: " << argv[0] << " <command> <argument>" << std::endl;
                return 1;
        }

        return 0;
}
