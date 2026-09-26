#include <filesystem>
#include <fstream>
#include <memory>
#include <print>

#include "pak/header.hpp"

void tryPrintHeader(const std::filesystem::path &filePath) {
    std::ifstream file{filePath};

    if (std::filesystem::file_size(filePath) >= sizeof(Pak::Header)) {
        alignas(Pak::Header) char buffer[sizeof(Pak::Header)];
        Pak::Header *header;

        file.read(buffer, sizeof(Pak::Header));
        header = std::start_lifetime_as<Pak::Header>(buffer);

        std::println("{}", filePath.string());
        std::println("{}", *header);
    }
}

int main(int argc, char **argv) {
    for (auto i = 0; i < argc; i += 1) {
        bool exists = std::filesystem::exists(argv[i]);
        if (!exists)
            continue;

        tryPrintHeader(argv[i]);
    }
}
