#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string_view>

namespace Pak {

struct [[gnu::packed]] Header {
    enum class PackageFlag : std::uint8_t {
        AllowMemoryMapping = 0x02,
        Solid = 0x04,
        Preload = 0x08
    };

    char magic[4];
    std::uint32_t version;
    std::uint64_t fileListOffset;
    std::uint32_t fileListSize;
    PackageFlag flags;
    std::byte priority;
    std::byte md5[16];
    std::uint16_t numPart;

    static constexpr bool isValid(const Header &h) {
        if (std::memcmp(h.magic, Header::LSPKMagic,
                        sizeof(Header::LSPKMagic)) != 0)
            return false;

        /* Only support version 18, last BG3 format */
        if (h.version != 18)
            return false;
        return true;
    }

    static constexpr char LSPKMagic[] = {'L', 'S', 'P', 'K'};
};

// Needed to for use with std::start_lifetime_as + open + mmap
static_assert(alignof(Pak::Header) == 1);

} // namespace Pak

#include <format>

template <> struct std::formatter<Pak::Header::PackageFlag> {
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    auto format(const Pak::Header::PackageFlag &f,
                std::format_context &ctx) const {
        // TODO: Handle bit flag
        return std::format_to(ctx.out(), "0x{:02x}",
                              static_cast<std::uint8_t>(f));
    }
};

template <> struct std::formatter<Pak::Header> {
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    auto format(const Pak::Header &h, std::format_context &ctx) const {
        // Avoid potentially unaligned accesses from the packed struct.
        std::string_view magic{h.magic, sizeof(h.magic)};
        std::uint32_t version = h.version;
        std::uint64_t fileListOffset = h.fileListOffset;
        std::uint32_t fileListSize = h.fileListSize;
        Pak::Header::PackageFlag flags = h.flags;
        std::uint8_t priority = static_cast<std::uint8_t>(h.priority);
        std::uint16_t numPart = h.numPart;

        auto out = std::format_to(ctx.out(),
                                  "Header{{\n"
                                  "  magic: \"{}\",\n"
                                  "  version: {},\n"
                                  "  fileListOffset: {},\n"
                                  "  fileListSize: {},\n"
                                  "  flags: {},\n"
                                  "  priority: {},\n"
                                  "  md5: ",
                                  magic, version, fileListOffset, fileListSize,
                                  flags, priority);

        for (std::byte b : h.md5) {
            out = std::format_to(out, "{:02x}", static_cast<std::uint8_t>(b));
        }

        out = std::format_to(out, ",\n  numPart: {}", numPart);

        return std::format_to(out, "\n}}");
    }
};
