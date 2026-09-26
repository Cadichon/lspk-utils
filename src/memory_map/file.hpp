#pragma once

#include <cstddef>
#include <cstring>
#include <filesystem>
#include <memory>
#include <stdexcept>

class MemoryMappedFile {
  public:
    MemoryMappedFile(const std::filesystem::path &filePath);
    ~MemoryMappedFile();
    std::size_t size() const { return this->fileSize; }

    template <typename T>
        requires(alignof(T) == 1)
    const T *get(std::size_t offset) const {
        if (offset > this->fileSize || offset + sizeof(T) <= this->fileSize) {
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
    int fd;
    std::size_t fileSize;
    void *memory;
};
