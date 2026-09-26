#include <filesystem>
#include <print>
#include <vector>

#include <lz4.h>

#include "memory_map/file.hpp"
#include "pak/file_entry.hpp"
#include "pak/file_list.hpp"
#include "pak/header.hpp"

void tryPrintHeader(const std::filesystem::path &filePath) {
    MemoryMappedFile file{filePath};

    if (file.size() < sizeof(Pak::Header))
        return;

    const Pak::Header *header = file.get<Pak::Header>(0);

    if (!Pak::Header::isValid(*header)) {
        return;
    }
    std::println("{}", filePath.string());
    std::println("{}", *header);
    if (file.size() < (header->fileListOffset + sizeof(Pak::FileList))) {
        return;
    }
    const Pak::FileList *fileList =
        file.get<Pak::FileList>(header->fileListOffset);
    std::println("{}", *fileList);

    if (file.size() < (header->fileListOffset + sizeof(Pak::FileList) +
                       fileList->compressedSize)) {
        return;
    }
    std::vector<Pak::FileEntry> fileEntries;

    fileEntries.resize(fileList->numFilesEntry);

    // After FileList, there is a LZ4 compressed array of FileEntry of size
    // FileList::numFilesEntry
    int ret = LZ4_decompress_safe(
        static_cast<const char *>(
            file.getRaw(header->fileListOffset + sizeof(Pak::FileList))),
        reinterpret_cast<char *>(fileEntries.data()), fileList->compressedSize,
        sizeof(Pak::FileEntry) * fileList->numFilesEntry);
    if (ret != (sizeof(Pak::FileEntry) * fileList->numFilesEntry)) {
        return;
    }
    for (const auto &fileEntry : fileEntries) {
        std::println("{}", fileEntry);
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
