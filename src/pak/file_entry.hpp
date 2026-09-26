#pragma once

#include <cstddef>
#include <cstdint>

namespace Pak {

struct [[gnu::packed]] FileEntry {
    char name[256];
    std::uint32_t offsetInFile1;
    std::uint16_t offsetInFile2;
    std::byte archivePart;
    std::byte flags;
    std::uint32_t sizeOnDisk;
    std::uint32_t uncompressedSize;
};

} // namespace Pak

#include <format>

template <> struct std::formatter<Pak::FileEntry> {
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    auto format(const Pak::FileEntry &f, std::format_context &ctx) const {
        // Avoid potentially unaligned accesses from the packed struct.
        std::string_view name{f.name, sizeof(f.name)};
        std::uint32_t offsetInFile1 = f.offsetInFile1;
        std::uint16_t offsetInFile2 = f.offsetInFile2;
        std::uint8_t archivePart = static_cast<std::uint8_t>(f.archivePart);
        std::uint8_t flags = static_cast<std::uint8_t>(f.flags);
        std::uint32_t sizeOnDisk = f.sizeOnDisk;
        std::uint32_t uncompressedSize = f.uncompressedSize;

        return std::format_to(ctx.out(),
                              "FileEntry{{\n"
                              "  name: \"{}\",\n"
                              "  offsetInFile1: {},\n"
                              "  offsetInFile2: {},\n"
                              "  archivePart: {},\n"
                              "  flags: 0x{:02x},\n"
                              "  sizeOnDisk: {},\n"
                              "  uncompressedSize: {}\n"
                              "}}",
                              name, offsetInFile1, offsetInFile2, archivePart,
                              flags, sizeOnDisk, uncompressedSize);
    }
};
