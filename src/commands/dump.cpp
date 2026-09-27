#include <filesystem>
#include <print>
#include <vector>

#include <lz4.h>

#include "memory_map/file.hpp"
#include "pak/file_entry.hpp"
#include "pak/file_list.hpp"
#include "pak/header.hpp"

bool dump(const std::filesystem::path &inputPak) {
    MemoryMappedFile file{inputPak};

    if (file.size() < sizeof(Pak::Header)) {
        std::println("{} is smaller than LSPK header, invalid file",
                     inputPak.string());
        return false;
    }

    const Pak::Header *header = file.get<Pak::Header>(0);

    if (!Pak::Header::isValid(*header)) {
        std::println("{} is not a LSPK supported by this tool",
                     inputPak.string());
        return false;
    }
    std::println("{}", inputPak.string());
    std::println("{}", *header);
    if (file.size() < (header->fileListOffset + sizeof(Pak::FileList))) {
        std::println("fileListOffset is after the end of {}, invalid file",
                     inputPak.string());
        return false;
    }
    const Pak::FileList *fileList =
        file.get<Pak::FileList>(header->fileListOffset);
    std::println("{}", *fileList);

    if (file.size() < (header->fileListOffset + sizeof(Pak::FileList) +
                       fileList->compressedSize)) {
        return false;
    }
    std::vector<Pak::FileEntry> fileEntries{fileList->numFilesEntry};
    const char *inputMemory = reinterpret_cast<const char *>(
        file.getRaw(header->fileListOffset + sizeof(Pak::FileList)));
    char *outputMemory = reinterpret_cast<char *>(fileEntries.data());

    // After FileList, there is a LZ4 compressed array of FileEntry of size
    // FileList::numFilesEntry
    int ret =
        LZ4_decompress_safe(inputMemory, outputMemory, fileList->compressedSize,
                            sizeof(Pak::FileEntry) * fileList->numFilesEntry);

    if (ret != (sizeof(Pak::FileEntry) * fileList->numFilesEntry)) {
        std::println("extraction of fileEntries failed, invalid file");
        return false;
    }
    for (const auto &fileEntry : fileEntries) {
        std::println("{}", fileEntry);
    }
    return true;
}
