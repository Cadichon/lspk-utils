#pragma once

#include <cstddef>
#include <cstring>
#include <filesystem>
#include <memory>

class MemoryMappedFile {
  public:
    MemoryMappedFile(const std::filesystem::path &filePath);
    ~MemoryMappedFile();
    std::size_t size() const;

    template <typename T>
        requires(alignof(T) == 1)
    const T *get(std::size_t offset) const {
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
