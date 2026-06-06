#pragma once

#include "TableData.h"

#include <filesystem>
/// <summary>
/// 生成对应的C#读取管理器脚本
/// </summary>
class ConfigManagerExporter
{
public:
    bool Export(const std::vector<TableData>& tables,
                const std::filesystem::path& outputPath,
                ExportReport& report) const;
};

