#include "Validator.h"
#include "StringUtils.h"

#include <set>
#include <unordered_set>

namespace
{
constexpr char VectorArrayItemDelimiter = '/';

bool IsIntegerArrayType(FieldType type)
{
    return type == FieldType::IntArray ||
           type == FieldType::LongArray ||
           type == FieldType::ShortArray ||
           type == FieldType::ByteArray;
}

bool IsNumberArrayType(FieldType type)
{
    return type == FieldType::FloatArray || type == FieldType::DoubleArray;
}

bool IsValidIntegerByType(FieldType type, const std::string& value)
{
    switch (type)
    {
    case FieldType::Int:
    case FieldType::IntArray:
        return StringUtils::IsValidInteger(value);
    case FieldType::Long:
    case FieldType::LongArray:
        return StringUtils::IsValidLongInteger(value);
    case FieldType::Short:
    case FieldType::ShortArray:
        return StringUtils::IsValidShortInteger(value);
    case FieldType::Byte:
    case FieldType::ByteArray:
        return StringUtils::IsValidByteInteger(value);
    default:
        return false;
    }
}

std::string ExpectedIntegerTypeName(FieldType type)
{
    switch (type)
    {
    case FieldType::Int:
    case FieldType::IntArray:
        return "int";
    case FieldType::Long:
    case FieldType::LongArray:
        return "long";
    case FieldType::Short:
    case FieldType::ShortArray:
        return "short";
    case FieldType::Byte:
    case FieldType::ByteArray:
        return "byte(0-255)";
    default:
        return FieldTypeToString(type);
    }
}
/// <summary>
/// Vector数据类型校验
/// </summary>
bool ParseVectorComponents(const TableData& table,
                           const ColumnInfo& column,
                           const std::string& rawValue,
                           int lineNumber,
                           std::vector<std::string>& outComponents,
                           ExportReport& report)
{
    //得到当前容器参数量
    const int componentCount = GetVectorComponentCount(column.type);
    //做字符串分割操作
    std::vector<std::string> components = StringUtils::Split(rawValue, table.arrayDelimiter, true);
    //数量不匹配时报错
    if (static_cast<int>(components.size()) != componentCount)
    {
        report.errors.push_back({
            table.fileName,
            lineNumber,
            column.name,
            "字段应为 " + FieldTypeToString(column.type) + "，需要 " +
                std::to_string(componentCount) + " 个数字分量，实际为 " + rawValue
        });
        return false;
    }
    //校验数据类型
    for (std::string& component : components)
    {
        component = StringUtils::Trim(component);
        //如果数据类型不匹配返回错误信息
        if (!StringUtils::IsValidNumber(component))
        {
            report.errors.push_back({
                table.fileName,
                lineNumber,
                column.name,
                "字段应为 " + FieldTypeToString(column.type) + "，分量应为数字，实际为 " + component
            });
            return false;
        }
    }
    //将校验完毕的数据放入最终结果
    outComponents = components;
    return true;
}
}
/// <summary>
/// 整张表格数据校验
/// </summary>
bool Validator::Validate(TableData& table, ExportReport& report) const
{
    bool ok = true;

    if (table.rawRows.size() < 3)
    {
        AddError(table, report, 0, "", "表格至少需要 3 行表头：字段名、字段类型、字段说明");
        return false;
    }

    const RawRow& nameRow = table.rawRows[0];
    const RawRow& typeRow = table.rawRows[1];
    const RawRow& descRow = table.rawRows[2];

    if (nameRow.cells.size() != typeRow.cells.size())
    {
        AddError(table, report, typeRow.lineNumber, "", "字段名数量和字段类型数量必须一致");
        return false;
    }

    table.allColumns.clear();
    table.exportColumns.clear();
    table.records.clear();
    //set：查找基于红黑树，不允许字段类型重复
    std::set<std::string> fieldNames;
    for (size_t i = 0; i < nameRow.cells.size(); ++i)
    {
        ColumnInfo column;
        column.name = StringUtils::Trim(nameRow.cells[i]);
        column.typeName = StringUtils::Trim(typeRow.cells[i]);
        column.description = i < descRow.cells.size() ? StringUtils::Trim(descRow.cells[i]) : "";
        column.ignored = !column.name.empty() && column.name[0] == '#';
        column.originalIndex = i;

        if (column.name.empty())
        {
            AddError(table, report, nameRow.lineNumber, "", "字段名不能为空");
            ok = false;
        }
        if (column.typeName.empty())
        {
            AddError(table, report, typeRow.lineNumber, column.name, "字段类型不能为空");
            ok = false;
        }

        if (!column.ignored)
        {
            column.type = ParseFieldType(column.typeName);
            if (column.type == FieldType::Unknown)
            {
                AddError(table, report, typeRow.lineNumber, column.name, "不支持的字段类型：" + column.typeName);
                ok = false;
            }
            if (!StringUtils::IsCSharpIdentifier(column.name))
            {
                AddError(table, report, nameRow.lineNumber, column.name, "字段名必须是合法 C# 标识符");
                ok = false;
            }
            if (fieldNames.find(column.name) != fieldNames.end())
            {
                AddError(table, report, nameRow.lineNumber, column.name, "有效字段名重复");
                ok = false;
            }
            fieldNames.insert(column.name);
            table.exportColumns.push_back(column);
        }

        table.allColumns.push_back(column);
    }

    if (table.exportColumns.empty())
    {
        AddError(table, report, nameRow.lineNumber, "", "没有可导出的有效字段");
        ok = false;
    }
    else
    {
        const ColumnInfo& idColumn = table.exportColumns.front();
        if (idColumn.name != "id")
        {
            AddError(table, report, nameRow.lineNumber, idColumn.name, "第一列有效字段必须为 id");
            ok = false;
        }
        if (idColumn.type != FieldType::Int)
        {
            AddError(table, report, typeRow.lineNumber, idColumn.name, "id 字段类型必须为 int");
            ok = false;
        }
    }

    if (!ok)
    {
        return false;
    }
    //用于存储数据ID，无法重复
    std::unordered_set<int> ids;
    for (size_t rowIndex = 3; rowIndex < table.rawRows.size(); ++rowIndex)
    {
        const RawRow& row = table.rawRows[rowIndex];
        if (row.cells.size() != nameRow.cells.size())
        {
            AddError(table,
                     report,
                     row.lineNumber,
                     "",
                     "数据列数量必须和原始字段名数量一致，期望 " + std::to_string(nameRow.cells.size()) +
                         " 列，实际 " + std::to_string(row.cells.size()) + " 列");
            ok = false;
            continue;
        }

        DataRecord record;
        for (const ColumnInfo& column : table.exportColumns)
        {
            const std::string rawValue = row.cells[column.originalIndex];
            DataCell cell;
            if (!ValidateCell(table, column, rawValue, row.lineNumber, cell, report))
            {
                ok = false;
            }
            record.cells.push_back(cell);
        }

        if (!record.cells.empty())
        {
            int id = 0;
            if (StringUtils::TryParseInt(record.cells.front().value, id))
            {
                record.id = id;
                if (ids.find(id) != ids.end())
                {
                    AddError(table, report, row.lineNumber, "id", "id 不能重复：" + std::to_string(id));
                    ok = false;
                }
                ids.insert(id);
            }
        }

        table.records.push_back(record);
    }

    if (!ok)
    {
        table.records.clear();
    }
    return ok;
}

void Validator::AddError(const TableData& table,
                         ExportReport& report,
                         int lineNumber,
                         const std::string& fieldName,
                         const std::string& reason) const
{
    report.errors.push_back({ table.fileName, lineNumber, fieldName, reason });
}

bool Validator::ValidateCell(const TableData& table,
                             const ColumnInfo& column,
                             const std::string& rawValue,
                             int lineNumber,
                             DataCell& outCell,
                             ExportReport& report) const
{
    const std::string value = StringUtils::Trim(rawValue);
    outCell.type = column.type;

    if (column.name == "id" && value.empty())
    {
        AddError(table, report, lineNumber, column.name, "id 不能为空");
        return false;
    }

    switch (column.type)
    {
    case FieldType::Int:
    case FieldType::Long:
    case FieldType::Short:
    case FieldType::Byte:
        if (!IsValidIntegerByType(column.type, value))
        {
            AddError(table,
                     report,
                     lineNumber,
                     column.name,
                     "字段应为 " + ExpectedIntegerTypeName(column.type) + "，实际为 " + rawValue);
            return false;
        }
        outCell.value = value;
        return true;
    case FieldType::Float:
    case FieldType::Double:
        if (!StringUtils::IsValidNumber(value))
        {
            AddError(table,
                     report,
                     lineNumber,
                     column.name,
                     "字段应为 " + FieldTypeToString(column.type) + "，实际为 " + rawValue);
            return false;
        }
        outCell.value = value;
        return true;
    case FieldType::String:
        outCell.value = value;
        return true;
    case FieldType::Bool:
    {
        bool boolValue = false;
        if (!StringUtils::TryParseBool(value, boolValue))
        {
            AddError(table, report, lineNumber, column.name, "字段应为 bool(true/false/1/0)，实际为 " + rawValue);
            return false;
        }
        outCell.boolValue = boolValue;
        outCell.value = boolValue ? "true" : "false";
        return true;
    }
    case FieldType::Vector2:
    case FieldType::Vector3:
    {
        std::vector<std::string> components;
        if (!ParseVectorComponents(table, column, value, lineNumber, components, report))
        {
            return false;
        }
        outCell.arrayValues = components;
        return true;
    }
    case FieldType::IntArray:
    case FieldType::LongArray:
    case FieldType::ShortArray:
    case FieldType::ByteArray:
    case FieldType::FloatArray:
    case FieldType::DoubleArray:
    case FieldType::StringArray:
    case FieldType::BoolArray:
    {
        if (value.empty())
        {
            return true;
        }

        std::vector<std::string> parts = StringUtils::Split(value, table.arrayDelimiter, false);
        for (std::string part : parts)
        {
            part = StringUtils::Trim(part);
            if (IsIntegerArrayType(column.type) && !IsValidIntegerByType(column.type, part))
            {
                AddError(table,
                         report,
                         lineNumber,
                         column.name,
                         "数组元素应为 " + ExpectedIntegerTypeName(column.type) + "，实际为 " + part);
                return false;
            }
            if (IsNumberArrayType(column.type) && !StringUtils::IsValidNumber(part))
            {
                AddError(table,
                         report,
                         lineNumber,
                         column.name,
                         "数组元素应为 " + FieldTypeToString(column.type).substr(0, FieldTypeToString(column.type).size() - 2) +
                             "，实际为 " + part);
                return false;
            }
            if (column.type == FieldType::BoolArray)
            {
                bool boolValue = false;
                if (!StringUtils::TryParseBool(part, boolValue))
                {
                    AddError(table, report, lineNumber, column.name, "数组元素应为 bool(true/false/1/0)，实际为 " + part);
                    return false;
                }
                part = boolValue ? "true" : "false";
            }
            outCell.arrayValues.push_back(part);
        }
        return true;
    }
    case FieldType::Vector2Array:
    case FieldType::Vector3Array:
    {
        if (value.empty())
        {
            return true;
        }

        const std::vector<std::string> vectorItems = StringUtils::Split(value, VectorArrayItemDelimiter, false);
        for (std::string item : vectorItems)
        {
            item = StringUtils::Trim(item);
            std::vector<std::string> components;
            if (!ParseVectorComponents(table, column, item, lineNumber, components, report))
            {
                return false;
            }
            outCell.vectorArrayValues.push_back(components);
        }
        return true;
    }
    case FieldType::Unknown:
    default:
        AddError(table, report, lineNumber, column.name, "未知字段类型");
        return false;
    }
}
