#pragma once

#include <cstddef>
#include <cstring>
#include <filesystem>
#include <memory>
#include <stdexcept>

#ifdef _WIN32
#include <windows.h>
#endif

class MemoryMappedFile {
  public:
    MemoryMappedFile(const std::filesystem::path &filePath);
    ~MemoryMappedFile();
    MemoryMappedFile(const MemoryMappedFile &) = delete;
    MemoryMappedFile &operator=(const MemoryMappedFile &) = delete;
    MemoryMappedFile(MemoryMappedFile &&other) noexcept = delete;
    MemoryMappedFile &operator=(MemoryMappedFile &&other) noexcept = delete;

    std::size_t size() const { return this->fileSize; }

    template <typename T>
        requires(alignof(T) == 1)
    const T *get(std::size_t offset) const {
        if (offset > this->fileSize || sizeof(T) > this->fileSize - offset) {
            throw std::out_of_range(
                "Out of range access in MemoryMappedFile::get");
        }
        return std::start_lifetime_as<T>(
            static_cast<const std::byte *>(this->memory) + offset);
    }

    const void *getRaw(std::size_t offset = 0) const {
        return (static_cast<const std::byte *>(this->memory) + offset);
    }

  private:
#ifdef _WIN32
    HANDLE fileHandle = INVALID_HANDLE_VALUE;
    HANDLE mappingHandle = nullptr;
#else
    int fd;
#endif
    std::size_t fileSize;
    void *memory;
};
