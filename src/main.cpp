// C++ 20
#include <iostream>
#include <string>
#include <string_view>
#include <charconv>
#include "lib/nlohmann/json.hpp"
#include "lib/bencode/decode.hpp"

using json = nlohmann::json;

int main(int argc, char *argv[])
{
        // If there are less than 3 arguments or the first argument is not "decode", print usage and exit
        if (argc < 3 || std::string_view(argv[1]) != "decode")
        {
                std::cerr << "Usage: " << argv[0] << " decode <encoded_value>" << std::endl;
                return 1;
        }

        std::string_view encoded_value(argv[2]);

        try
        {
                size_t index = {0};
                json decoded_value = extract_and_decode(encoded_value, index, encoded_value[0]);
                std::cout << decoded_value.dump() << std::endl;
        }
        catch (const std::invalid_argument &e)
        {
                std::cerr << "Error: " << e.what() << std::endl;
                return 1;
        }

        return 0;
}
