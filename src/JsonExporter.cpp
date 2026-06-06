#include "JsonExporter.h"
#include "StringUtils.h"

#include <fstream>
#include <iostream>
/// <summary>
/// 负责导出Json文件错误报告处理
/// </summary>
bool JsonExporter::Export(const TableData& table, const std::filesystem::path& outputDir, ExportReport& report) const
{
    std::error_code ec;
    std::filesystem::create_directories(outputDir, ec);
    if (ec)
    {
        report.errors.push_back({ table.fileName, 0, "", "创建 JSON 输出目录失败：" + ec.message() });
        return false;
    }

    const std::filesystem::path outputPath = outputDir / (table.className + ".json");
    std::ofstream output(outputPath, std::ios::binary);
    if (!output.is_open())
    {
        report.errors.push_back({ table.fileName, 0, "", "无法写入 JSON 文件：" + outputPath.string() });
        return false;
    }

    output << "{\n";
    output << "  \"items\": [\n";
    for (size_t rowIndex = 0; rowIndex < table.records.size(); ++rowIndex)
    {
        const DataRecord& record = table.records[rowIndex];
        output << "    {\n";
        for (size_t colIndex = 0; colIndex < table.exportColumns.size(); ++colIndex)
        {
            const ColumnInfo& column = table.exportColumns[colIndex];
            output << "      \"" << StringUtils::EscapeJson(column.name) << "\": ";
            WriteCell(output, record.cells[colIndex]);
            if (colIndex + 1 < table.exportColumns.size())
            {
                output << ",";
            }
            output << "\n";
        }
        output << "    }";
        if (rowIndex + 1 < table.records.size())
        {
            output << ",";
        }
        output << "\n";
    }
    output << "  ]\n";
    output << "}\n";

    return true;
}
/// <summary>
/// 辅助函数，通过处理不同的数据类型给予不同的包裹方式
/// </summary>
void JsonExporter::WriteCell(std::ostream& output, const DataCell& cell) const
{
    switch (cell.type)
    {
    case FieldType::String:
        output << "\"" << StringUtils::EscapeJson(cell.value) << "\"";
        break;
    case FieldType::Bool:
        output << (cell.boolValue ? "true" : "false");
        break;
    case FieldType::Vector2:
    case FieldType::Vector3:
        WriteVectorObject(output, cell.arrayValues);
        break;
    case FieldType::IntArray:
    case FieldType::LongArray:
    case FieldType::ShortArray:
    case FieldType::ByteArray:
    case FieldType::FloatArray:
    case FieldType::DoubleArray:
    case FieldType::StringArray:
    case FieldType::BoolArray:
        output << "[";
        for (size_t i = 0; i < cell.arrayValues.size(); ++i)
        {
            if (i > 0)
            {
                output << ", ";
            }
            if (cell.type == FieldType::StringArray)
            {
                output << "\"" << StringUtils::EscapeJson(cell.arrayValues[i]) << "\"";
            }
            else
            {
                output << cell.arrayValues[i];
            }
        }
        output << "]";
        break;
    case FieldType::Vector2Array:
    case FieldType::Vector3Array:
        output << "[";
        for (size_t i = 0; i < cell.vectorArrayValues.size(); ++i)
        {
            if (i > 0)
            {
                output << ", ";
            }
            WriteVectorObject(output, cell.vectorArrayValues[i]);
        }
        output << "]";
        break;
    case FieldType::Int:
    case FieldType::Long:
    case FieldType::Short:
    case FieldType::Byte:
    case FieldType::Float:
    case FieldType::Double:
    default:
        output << cell.value;
        break;
    }
}
/// <summary>
/// 辅助函数，处理Vector对象
/// </summary>
void JsonExporter::WriteVectorObject(std::ostream& output, const std::vector<std::string>& components) const
{
    output << "{";
    if (!components.empty())
    {
        output << "\"x\": " << components[0];
    }
    if (components.size() >= 2)
    {
        output << ", \"y\": " << components[1];
    }
    if (components.size() >= 3)
    {
        output << ", \"z\": " << components[2];
    }
    output << "}";
}
