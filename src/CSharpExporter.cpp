#include "CSharpExporter.h"
#include "StringUtils.h"

#include <fstream>

bool CSharpExporter::Export(const TableData& table, const std::filesystem::path& outputDir, ExportReport& report) const
{
    std::error_code ec;
    std::filesystem::create_directories(outputDir, ec);
    if (ec)
    {
        report.errors.push_back({ table.fileName, 0, "", "创建 C# 输出目录失败：" + ec.message() });
        return false;
    }

    const std::filesystem::path outputPath = outputDir / (table.className + ".cs");
    std::ofstream output(outputPath, std::ios::binary);
    if (!output.is_open())
    {
        report.errors.push_back({ table.fileName, 0, "", "无法写入 C# 文件：" + outputPath.string() });
        return false;
    }

    bool needsUnityEngine = false;
    for (const ColumnInfo& column : table.exportColumns)
    {
        if (IsUnityVectorFieldType(column.type))
        {
            needsUnityEngine = true;
            break;
        }
    }

    output << "using System;\n";
    output << "using System.Collections.Generic;\n";
    if (needsUnityEngine)
    {
        output << "using UnityEngine;\n";
    }
    output << "\n";
    output << "[Serializable]\n";
    output << "public class " << table.className << "\n";
    output << "{\n";
    for (const ColumnInfo& column : table.exportColumns)
    {
        if (!column.description.empty())
        {
            output << "    // " << StringUtils::EscapeCSharpComment(column.description) << "\n";
        }
        output << "    public " << FieldTypeToCSharpType(column.type) << " " << column.name << ";\n\n";
    }
    output << "}\n\n";
    output << "[Serializable]\n";
    output << "public class " << table.className << "List\n";
    output << "{\n";
    output << "    public List<" << table.className << "> items;\n";
    output << "}\n";

    return true;
}
