#pragma once

#include "TableReader.h"

class ExcelReader : public ITableReader
{
public:
    bool Read(const std::filesystem::path& filePath,
              DelimiterMode delimiterMode,
              TableData& outTable,
              std::vector<ExportError>& errors) override;
};

