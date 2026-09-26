#include <cstddef>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <system_error>
#include <unistd.h>

#include "memory_map/file.hpp"

MemoryMappedFile::MemoryMappedFile(const std::filesystem::path &filePath) {
    this->fd = open(filePath.c_str(), O_RDONLY);

    if (this->fd == -1) {
        throw std::system_error();
    }
    struct stat st;
    if (fstat(this->fd, &st) != 0) {
        close(this->fd);
        throw std::system_error();
    }
    this->fileSize = st.st_size;
    this->memory = mmap(nullptr, this->fileSize, PROT_READ,
                        MAP_FILE | MAP_PRIVATE, this->fd, 0);
    if (this->memory == MAP_FAILED) {
        close(this->fd);
        throw std::system_error();
    }
}

MemoryMappedFile::~MemoryMappedFile() {
    munmap(this->memory, this->fileSize);
    close(this->fd);
}
