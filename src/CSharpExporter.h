#pragma once

#include "TableData.h"

#include <filesystem>
/// <summary>
/// C#脚本文件生成类
/// </summary>
class CSharpExporter
{
public:
    bool Export(const TableData& table, const std::filesystem::path& outputDir, ExportReport& report) const;
};

