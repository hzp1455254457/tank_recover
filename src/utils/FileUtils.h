#pragma once

#include <string>
#include <vector>

namespace BattleCity {

class FileUtils {
public:
    static std::string readTextFile(const std::string& path) { return ""; }
    static bool writeTextFile(const std::string& path, const std::string& content) { return true; }
    static std::vector<uint8_t> readBinaryFile(const std::string& path) { return {}; }
    static bool writeBinaryFile(const std::string& path, const std::vector<uint8_t>& data) { return true; }
};

} // namespace BattleCity