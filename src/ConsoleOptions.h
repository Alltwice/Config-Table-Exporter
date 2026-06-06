#pragma once

#include "TableData.h"

#include <filesystem>

struct ConsoleOptions
{
    std::filesystem::path inputDir = "./Tables";
    std::filesystem::path jsonOutputDir = "./Output/Json";
    std::filesystem::path csharpOutputDir = "./Output/CSharp";
    std::filesystem::path managerOutputPath = "./Output/CSharp/ConfigManager.cs";
    DelimiterMode delimiterMode = DelimiterMode::Auto;
};

class ConsoleOptionsReader
{
public:
    ConsoleOptions Read() const;

private:
    std::string Prompt(const std::string& message, const std::string& defaultValue) const;
};

