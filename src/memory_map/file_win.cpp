#include <limits>
#include <system_error>
#include <utility>

#include <windows.h>

#include "memory_map/file.hpp"

MemoryMappedFile::MemoryMappedFile(const std::filesystem::path &filePath) {

    this->fileHandle =
        CreateFileW(filePath.c_str(), GENERIC_READ,
                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                    nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (this->fileHandle == INVALID_HANDLE_VALUE) {
        throw std::system_error(static_cast<int>(GetLastError()),
                                std::system_category(), "CreateFileW");
    }

    LARGE_INTEGER size;

    if (!GetFileSizeEx(this->fileHandle, &size)) {
        CloseHandle(this->fileHandle);
        throw std::system_error(static_cast<int>(GetLastError()),
                                std::system_category(), "GetFileSizeEx");
    }

    if (size.QuadPart < 0 || static_cast<unsigned long long>(size.QuadPart) >
                                 std::numeric_limits<std::size_t>::max()) {

        CloseHandle(this->fileHandle);
        throw std::system_error(
            std::make_error_code(std::errc::value_too_large),
            "File is too large to map");
    }

    this->fileSize = static_cast<std::size_t>(size.QuadPart);

    // Windows cannot create a file mapping for an empty file.
    // Represent it as an empty mapping instead.
    if (this->fileSize == 0) {
        return;
    }

    this->mappingHandle = CreateFileMappingW(this->fileHandle, nullptr,
                                             PAGE_READONLY, 0, 0, nullptr);

    if (this->mappingHandle == nullptr) {
        CloseHandle(this->fileHandle);
        throw std::system_error(static_cast<int>(GetLastError()),
                                std::system_category(), "CreateFileMappingW");
    }

    this->memory = MapViewOfFile(this->mappingHandle, FILE_MAP_READ, 0, 0, 0);

    if (this->memory == nullptr) {
        CloseHandle(this->mappingHandle);
        CloseHandle(this->fileHandle);
        throw std::system_error(static_cast<int>(GetLastError()),
                                std::system_category(), "MapViewOfFile");
    }
}

MemoryMappedFile::~MemoryMappedFile() {
    UnmapViewOfFile(this->memory);
    CloseHandle(this->mappingHandle);
    CloseHandle(this->fileHandle);
}
