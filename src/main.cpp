#include <iostream>
#include <string>
#include <string_view>
#include <fstream>
#include <charconv>
#include "lib/nlohmann/json.hpp"
#include "lib/bencode/decode.hpp"
#include "lib/bencode/encode.hpp"
#include "lib/bencode/utils.hpp"
#include "lib/hash/sha1.hpp"
#include "lib/http/HTTPRequest.hpp"
#include "lib/http/utils.hpp"

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
                        SHA1 sha1;
                        auto [decoded_info, _] = decode_bencoded_dictionary(file_data_view);
                        std::string bencoded_string = encode_to_bencoded_string(decoded_info.at("info"));
                        std::string pieces = decoded_info.at("info").at("pieces").get<std::string>();

                        std::cout << "Tracker URL: " << decoded_info.at("announce").get<std::string>() << std::endl;
                        std::cout << "Length: " << decoded_info.at("info").at("length").get<int64_t>() << std::endl;
                        std::cout << "Info Hash: " << sha1(bencoded_string) << std::endl;
                        std::cout << "Piece Length: " << decoded_info.at("info").at("piece length").get<int64_t>() << std::endl;
                        std::cout << "Piece Hashes: " << std::endl;
                        for (size_t i = 0; i < pieces.length(); i += 20)
                        {
                                std::string piece_hash = pieces.substr(i, 20);
                                std::cout << hash_to_hex_string(piece_hash) << std::endl;
                        }
                }
                catch (const std::invalid_argument &e)
                {
                        std::cerr << "Error decoding bencoded info dictionary: " << e.what() << std::endl;
                        return 1;
                }
        }
        else if (command == "peers")
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
                        SHA1 sha1;
                        auto [decoded_info, _] = decode_bencoded_dictionary(file_data_view);
                        std::string bencoded_string = encode_to_bencoded_string(decoded_info.at("info"));
                        std::string url = decoded_info.at("announce").get<std::string>();
                        std::string encoded_info_hash = url_encode(sha1(bencoded_string));
                        std::string left = std::to_string(file_data.size()); // Convert size_t to string
                        http::Request request{url + "?info_hash=" + encoded_info_hash + "&peer_id=00112233445566778899&port=6881&uploaded=0&downloaded=0&left=" + left + "&compact=1"};
                        const auto response = request.send("GET");
                        std::string response_body{response.body.begin(), response.body.end()};
                        std::string_view response_body_view(response_body.data(), response_body.size());
                        auto [decoded_response, _] = decode_bencoded_dictionary(response_body_view);
                        std::string peers = decoded_response.at("peers").get<std::string>();
                        for (size_t i = 0; i < peers.length(); i += 6)
                        {
                                std::string ip = std::to_string(static_cast<unsigned char>(peers[i])) + "." +
                                                 std::to_string(static_cast<unsigned char>(peers[i + 1])) + "." +
                                                 std::to_string(static_cast<unsigned char>(peers[i + 2])) + "." +
                                                 std::to_string(static_cast<unsigned char>(peers[i + 3]));
                                uint16_t port = (static_cast<uint16_t>(static_cast<unsigned char>(peers[i + 4]) << 8)) | static_cast<uint16_t>(static_cast<unsigned char>(peers[i + 5]));
                                std::cout << ip << ":" << port << std::endl;
                        }
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
