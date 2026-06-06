#pragma once

#include "TableData.h"

#include <filesystem>
/// <summary>
/// Json文件生成类
/// </summary>
class JsonExporter
{
public:
    bool Export(const TableData& table, const std::filesystem::path& outputDir, ExportReport& report) const;

private:
    void WriteCell(std::ostream& output, const DataCell& cell) const;
    void WriteVectorObject(std::ostream& output, const std::vector<std::string>& components) const;
};
