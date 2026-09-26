#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace Pak {

struct [[gnu::packed]] Header {
    char magic[4];
    std::uint32_t version;
    std::uint64_t fileListOffset;
    std::uint32_t fileListSize;
    std::byte flags;
    std::byte priority;
    std::byte md5[16];
    std::uint16_t numPart;
};

} // namespace Pak

#include <format>

template <> struct std::formatter<Pak::Header> {
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    auto format(const Pak::Header &h, std::format_context &ctx) const {
        // Avoid potentially unaligned accesses from the packed struct.
        std::string_view magic{h.magic, sizeof(h.magic)};
        std::uint32_t version = h.version;
        std::uint64_t fileListOffset = h.fileListOffset;
        std::uint32_t fileListSize = h.fileListSize;
        std::uint8_t flags = static_cast<std::uint8_t>(h.flags);
        std::uint8_t priority = static_cast<std::uint8_t>(h.priority);
        std::uint16_t numPart = h.numPart;

        auto out = std::format_to(ctx.out(),
                                  "Header{{\n"
                                  "  magic: \"{}\",\n"
                                  "  version: {},\n"
                                  "  fileListOffset: {},\n"
                                  "  fileListSize: {},\n"
                                  "  flags: 0x{:02x},\n"
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
