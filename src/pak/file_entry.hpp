#pragma once

#include <cstddef>
#include <cstdint>
#include <utility>

namespace Pak {

struct [[gnu::packed]] FileEntry {
    enum class CompressionType : std::uint8_t {
        None = 0x00,
        Zlib = 0x01,
        Lz4 = 0x02,
        Zstd = 0x03,
    };
    enum class CompressionLevel : std::uint8_t {
        Fast = 0x10,
        Default = 0x20,
        Max = 0x40,
    };

    char name[256];
    std::uint32_t offsetInFile1;
    std::uint16_t offsetInFile2;

    std::uint64_t offsetInFile() const {
        return static_cast<std::uint64_t>(this->offsetInFile1) |
               (static_cast<std::uint64_t>(this->offsetInFile2) << 32);
    }

    std::byte archivePart;
    std::byte flags;
    std::pair<CompressionType, CompressionLevel> compression() const {
        return {static_cast<CompressionType>(
                    static_cast<std::uint8_t>(this->flags) & 0x0F),
                static_cast<CompressionLevel>(
                    static_cast<std::uint8_t>(this->flags) & 0xF0)};
    }
    std::uint32_t sizeOnDisk;
    std::uint32_t uncompressedSize;
};

} // namespace Pak

#include <format>

template <> struct std::formatter<Pak::FileEntry::CompressionType> {
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    auto format(const Pak::FileEntry::CompressionType &f,
                std::format_context &ctx) const {
        switch (f) {
        case Pak::FileEntry::CompressionType::None:
            return std::format_to(ctx.out(), "None");
        case Pak::FileEntry::CompressionType::Zlib:
            return std::format_to(ctx.out(), "Zlib");
        case Pak::FileEntry::CompressionType::Lz4:
            return std::format_to(ctx.out(), "LZ4");
        case Pak::FileEntry::CompressionType::Zstd:
            return std::format_to(ctx.out(), "Zstd");
        default:
            return std::format_to(ctx.out(), "Unknown");
        }
    }
};

template <> struct std::formatter<Pak::FileEntry::CompressionLevel> {
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    auto format(const Pak::FileEntry::CompressionLevel &f,
                std::format_context &ctx) const {
        switch (f) {
        case Pak::FileEntry::CompressionLevel::Default:
            return std::format_to(ctx.out(), "Default");
        case Pak::FileEntry::CompressionLevel::Fast:
            return std::format_to(ctx.out(), "Fast");
        case Pak::FileEntry::CompressionLevel::Max:
            return std::format_to(ctx.out(), "Max");
        default:
            return std::format_to(ctx.out(), "Unknown");
        }
    }
};

template <>
struct std::formatter<std::pair<Pak::FileEntry::CompressionType,
                                Pak::FileEntry::CompressionLevel>> {
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    auto format(const std::pair<Pak::FileEntry::CompressionType,
                                Pak::FileEntry::CompressionLevel> &p,
                std::format_context &ctx) const {
        return std::format_to(ctx.out(), "{{{}, {}}}", p.first, p.second);
    }
};

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
                              "  offsetInFile (computed): {},\n"
                              "  archivePart: {},\n"
                              "  flags: 0x{:02x},\n"
                              "  compression (computed): {},\n"
                              "  sizeOnDisk: {},\n"
                              "  uncompressedSize: {}\n"
                              "}}",
                              name, offsetInFile1, offsetInFile2,
                              f.offsetInFile(), archivePart, flags,
                              f.compression(), sizeOnDisk, uncompressedSize);
    }
};
