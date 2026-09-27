#pragma once

#include <filesystem>

bool dump(const std::filesystem::path &inputPak);
bool unpak(const std::filesystem::path &inputPak,
           const std::filesystem::path &outputDir);
bool pak(const std::filesystem::path &inputDir,
         const std::filesystem::path &outputPak);
