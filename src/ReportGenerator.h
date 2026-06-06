#pragma once

#include "ExportSummary.h"

#include <filesystem>
#include <iosfwd>
#include <string>
/// <summary>
/// 显示最终的读取结果
/// </summary>
class ReportGenerator
{
public:
    std::string Generate(const ExportSummary& summary, bool includeExitPrompt) const;
    void PrintToConsole(const ExportSummary& summary) const;
    bool WriteToFile(const ExportSummary& summary, const std::filesystem::path& outputPath) const;

private:
    void AppendSectionLine(std::ostream& output, char ch) const;
    void AppendEmptyList(std::ostream& output) const;
};

