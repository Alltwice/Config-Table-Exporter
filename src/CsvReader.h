#pragma once

#include "TableReader.h"
/// <summary>
/// CSV文件解析工具
/// </summary>
class CsvReader : public ITableReader
{
public:
    bool Read(const std::filesystem::path& filePath,
              DelimiterMode delimiterMode,
              TableData& outTable,
              std::vector<ExportError>& errors) override;

private:
    char ResolveDelimiter(const std::filesystem::path& filePath,
                          DelimiterMode delimiterMode,
                          const std::string& firstDataLine) const;
    char DetectDelimiter(const std::string& line) const;
    std::vector<std::string> ParseLine(const std::string& line, char delimiter) const;
};

