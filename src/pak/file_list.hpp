#pragma once

#include <cstdint>

namespace Pak {

struct [[gnu::packed]] FileList {
    std::uint32_t numFilesEntry;
    std::uint32_t compressedSize;
    // VLA not supported in C++, here for documentation
    // std::byte compressedData[/* .compressedSize */];
};

// Needed to for use with std::start_lifetime_as + open + mmap
static_assert(alignof(Pak::FileList) == 1);

} // namespace Pak

#include <format>

template <> struct std::formatter<Pak::FileList> {
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    auto format(const Pak::FileList &f, std::format_context &ctx) const {
        // Avoid potentially unaligned accesses from the packed struct.
        std::uint32_t numFilesEntry = f.numFilesEntry;
        std::uint32_t compressedSize = f.compressedSize;

        return std::format_to(ctx.out(),
                              "FileList{{\n"
                              "  numFilesEntry: {},\n"
                              "  compressedSize: {}\n"
                              "}}",
                              numFilesEntry, compressedSize);
    }
};
