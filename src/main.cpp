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
#include "sys/socket.h"
#include <arpa/inet.h>

int main(const int argc, const char *argv[])
{
        if (argc < 3)
        {
                std::cerr << "Usage: " << argv[0] << " <command> <encoded_value>" << "\n";
                return 1;
        }

        const std::string_view command(argv[1]);
        const std::string_view encoded_value(argv[2]);

        if (command == "decode")
        {
                json decoded_value;
                try
                {
                        switch (encoded_value.front())
                        {
                        case 'i':
                        {
                                const auto [value, _] = decode_bencoded_integer(encoded_value);
                                decoded_value = value;
                                break;
                        }
                        case 'l':
                        {
                                const auto [value, _] = decode_bencoded_list(encoded_value);
                                decoded_value = value;
                                break;
                        }
                        case 'd':
                        {
                                const auto [value, _] = decode_bencoded_dictionary(encoded_value);
                                decoded_value = value;
                                break;
                        }
                        default:
                                if (std::isdigit(encoded_value.front()))
                                {
                                        const auto [value, _] = decode_bencoded_string(encoded_value);
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
                        std::cerr << "Error decoding bencoded value: " << e.what() << "\n";
                }

                std::cout << decoded_value.dump() << "\n";
        }
        else if (command == "info")
        {
                std::ifstream input_file{argv[2], std::ios::binary};
                if (!input_file)
                {
                        std::cerr << "Error opening torrent file: " << argv[2] << "\n";
                        return 1;
                }

                const std::vector<char> file_data((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
                const std::string_view file_data_view(file_data.data(), file_data.size());
                try
                {
                        SHA1 sha1;
                        auto [decoded_info, _] = decode_bencoded_dictionary(file_data_view);
                        const std::string bencoded_string = encode_to_bencoded_string(decoded_info.at("info"));
                        const std::string pieces = decoded_info.at("info").at("pieces").get<std::string>();

                        std::cout << "Tracker URL: " << decoded_info.at("announce").get<std::string>() << "\n";
                        std::cout << "Length: " << decoded_info.at("info").at("length").get<int64_t>() << "\n";
                        std::cout << "Info Hash: " << sha1(bencoded_string) << "\n";
                        std::cout << "Piece Length: " << decoded_info.at("info").at("piece length").get<int64_t>() << "\n";
                        std::cout << "Piece Hashes: " << "\n";
                        for (size_t i = 0; i < pieces.length(); i += 20)
                        {
                                const std::string piece_hash = pieces.substr(i, 20);
                                std::cout << hash_to_hex_string(piece_hash) << "\n";
                        }
                }
                catch (const std::invalid_argument &e)
                {
                        std::cerr << "Error decoding bencoded info dictionary: " << e.what() << "\n";
                        return 1;
                }
        }
        else if (command == "peers")
        {
                std::ifstream input_file{argv[2], std::ios::binary};
                if (!input_file)
                {
                        std::cerr << "Error opening torrent file: " << argv[2] << "\n";
                        return 1;
                }

                const std::vector<char> file_data((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
                const std::string_view file_data_view(file_data.data(), file_data.size());
                try
                {
                        SHA1 sha1;
                        auto [decoded_info, _1] = decode_bencoded_dictionary(file_data_view);
                        const std::string bencoded_string = encode_to_bencoded_string(decoded_info.at("info"));
                        const std::string url = decoded_info.at("announce").get<std::string>();
                        const std::string encoded_info_hash = url_encode(sha1(bencoded_string));
                        const std::string left = std::to_string(file_data.size()); // Convert size_t to string
                        http::Request request{url + "?info_hash=" + encoded_info_hash + "&peer_id=00112233445566778899&port=6881&uploaded=0&downloaded=0&left=" + left + "&compact=1"};
                        const auto response = request.send("GET");
                        const std::string response_body{response.body.begin(), response.body.end()};
                        const std::string_view response_body_view(response_body.data(), response_body.size());
                        auto [decoded_response, _2] = decode_bencoded_dictionary(response_body_view);
                        const std::string peers = decoded_response.at("peers").get<std::string>();
                        for (size_t i = 0; i < peers.length(); i += 6)
                        {
                                const std::string ip = std::to_string(static_cast<unsigned char>(peers[i])) + "." +
                                                       std::to_string(static_cast<unsigned char>(peers[i + 1])) + "." +
                                                       std::to_string(static_cast<unsigned char>(peers[i + 2])) + "." +
                                                       std::to_string(static_cast<unsigned char>(peers[i + 3]));
                                const uint16_t port = (static_cast<uint16_t>(static_cast<unsigned char>(peers[i + 4]) << 8)) | static_cast<uint16_t>(static_cast<unsigned char>(peers[i + 5]));
                                std::cout << ip << ":" << port << "\n";
                        }
                }
                catch (const std::invalid_argument &e)
                {
                        std::cerr << "Error decoding bencoded info dictionary: " << e.what() << "\n";
                        return 1;
                }
        }
        else if (command == "handshake")
        {
                // 165.232.33.77:51467
                // 178.62.85.20:51489
                // 178.62.82.89:51448
                std::ifstream input_file{argv[2], std::ios::binary};
                if (!input_file)
                {
                        std::cerr << "Error opening torrent file: " << argv[2] << "\n";
                        return 1;
                }

                const std::string peer(argv[3]);
                const size_t pos = peer.find_first_of(':');
                if (pos == std::string::npos)
                {
                        std::cerr << "Invalid peer format. Expected IP:PORT" << "\n";
                        return 1;
                }

                const std::string ip(peer.substr(0, pos));
                const std::string port(peer.substr(pos + 1));

                const int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
                if (socket_fd <= 0)
                {
                        std::cerr << "Error creating socket" << "\n";
                        return 1;
                }

                struct sockaddr_in server_address = {};
                server_address.sin_family = AF_INET;
                server_address.sin_port = htons(std::stoi(port.data()));
                if (int result = inet_pton(AF_INET, ip.data(), &server_address.sin_addr); result <= 0)
                {
                        std::cerr << "Invalid or unsupported IP address" << "\n";
                        close(socket_fd);
                        return 1;
                }

                if (connect(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
                {
                        std::cerr << "Error connecting to peer" << "\n";
                        return 1;
                }

                const std::vector<char> file_data((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
                const std::string_view file_data_view(file_data.data(), file_data.size());
                try
                {
                        SHA1 sha1;
                        const auto [decoded_info, _] = decode_bencoded_dictionary(file_data_view);
                        const std::string bencoded_string = encode_to_bencoded_string(decoded_info.at("info"));
                        const std::string self_id = "00112233445566778899";
                        sha1.add(bencoded_string.c_str(), bencoded_string.size());
                        unsigned char buffer[SHA1::HashBytes]; // 20 bytes sha1
                        sha1.getHash(buffer);
                        std::vector<unsigned char> info_hash(buffer, buffer + SHA1::HashBytes); // creating a vector from the buffer of chars

                        // length of protocol string + protocol string + reserved bytes + info hash + self id
                        std::vector<unsigned char> handshake;
                        handshake.push_back(19);
                        std::copy("BitTorrent protocol", "BitTorrent protocol" + 19, std::back_inserter(handshake));
                        handshake.insert(handshake.end(), 8, 0);
                        handshake.insert(handshake.end(), info_hash.begin(), info_hash.end());
                        handshake.insert(handshake.end(), self_id.begin(), self_id.end());
                        int bytes_sent = send(socket_fd, handshake.data(), handshake.size(), 0);

                        if (bytes_sent == -1)
                        {
                                std::cerr << "Error sending handshake" << "\n";
                                close(socket_fd);
                                return 1;
                        }

                        unsigned char response[68];
                        int bytes_received = recv(socket_fd, response, sizeof(response), 0);
                        if (bytes_received == -1)
                        {
                                std::cerr << "Error receiving handshake response" << "\n";
                                close(socket_fd);
                                return 1;
                        }

                        // extract 20 last bytes from the response, peer id
                        std::vector<unsigned char> peer_id(response + 48, response + 68);
                        std::stringstream ss;
                        for (unsigned char c : peer_id)
                        {
                                ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
                        }
                        std::string printable_peer_id = ss.str();
                        std::cout << "Peer ID: " << printable_peer_id << "\n";
                        close(socket_fd);
                }
                catch (const std::exception &e)
                {
                        std::cerr << "Error: " << e.what() << "\n";
                        close(socket_fd);
                        return 1;
                }
        }
        else
        {
                std::cerr << "Unknown command: " << command << "\n";
                std::cerr << "Usage: " << argv[0] << " <command> <argument>" << "\n";
                return 1;
        }

        return 0;
}
