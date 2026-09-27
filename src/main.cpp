#include <filesystem>
#include <print>

#include <argparse/argparse.hpp>

#include "commands.hpp"

template <> struct std::formatter<argparse::ArgumentParser> {
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    auto format(const argparse::ArgumentParser &p,
                std::format_context &ctx) const {
        std::stringstream stream;

        stream << p;

        return std::format_to(ctx.out(), "{}", stream.str());
    }
};

static_assert(std::endian::native == std::endian::little,
              "This program only works on little-endian OS (for now)");

int main(int argc, const char *const *argv) {
    argparse::ArgumentParser prog{argv[0]};

    argparse::ArgumentParser dumpCmd{"dump"};
    dumpCmd.add_argument("input").help("input .pak file");

    argparse::ArgumentParser pakCmd{"pak"};
    pakCmd.add_argument("input").help("input folder");
    pakCmd.add_argument("output").help("output .pak file name");

    argparse::ArgumentParser unpakCmd{"unpak"};
    unpakCmd.add_argument("input").help("input .pak file");
    unpakCmd.add_argument("output")
        .help("output folder, default to \"./out\"")
        .default_value("./out");

    prog.add_subparser(pakCmd);
    prog.add_subparser(unpakCmd);
    prog.add_subparser(dumpCmd);

    try {
        prog.parse_args(argc, argv);
    } catch (const std::exception &e) {
        std::println(stderr, "{}", e.what());
        std::print(stderr, "{}", prog);
        return 1;
    }

    if (prog.is_subcommand_used("dump")) {
        std::filesystem::path inputPak{
            prog.at<argparse::ArgumentParser>("dump").get("input")};

        return dump(inputPak) ? 0 : 1;
    } else if (prog.is_subcommand_used("unpak")) {
        std::filesystem::path inputPak{
            prog.at<argparse::ArgumentParser>("unpak").get("input")};
        std::filesystem::path outpurDir{
            prog.at<argparse::ArgumentParser>("unpak").get("output")};

        return unpak(inputPak, outpurDir) ? 0 : 1;
    } else if (prog.is_subcommand_used("pak")) {
        std::filesystem::path inputDir{
            prog.at<argparse::ArgumentParser>("pak").get("input")};
        std::filesystem::path outputPak{
            prog.at<argparse::ArgumentParser>("pak").get("output")};

        return pak(inputDir, outputPak) ? 0 : 1;
    } else {
        std::print("{}", prog);
        return 0;
    }
}
