#pragma once

#include <filesystem>
#include <string>
#include <vector>
/// <summary>
/// 分割符号枚举
/// </summary>
enum class DelimiterMode
{
    Auto,
    Comma,
    Semicolon,
    Tab,
    Pipe
};
/// <summary>
/// 数据类型枚举
/// </summary>
enum class FieldType
{
    Int,
    Long,
    Short,
    Byte,
    Float,
    Double,
    String,
    Bool,
    Vector2,
    Vector3,
    IntArray,
    LongArray,
    ShortArray,
    ByteArray,
    FloatArray,
    DoubleArray,
    StringArray,
    BoolArray,
    Vector2Array,
    Vector3Array,
    Unknown
};
/// <summary>
/// 一行结构体
/// </summary>
struct RawRow
{
    int lineNumber = 0;
    //读取后的一行数据
    std::vector<std::string> cells;
};
/// <summary>
/// 一列结构体，一列严格按照名字+类型+数据
/// </summary>
struct ColumnInfo
{
    std::string name;
    std::string typeName;
    std::string description;
    FieldType type = FieldType::Unknown;
    bool ignored = false;
    size_t originalIndex = 0;
};
/// <summary>
/// 数据单元格结构体，表示有效数据
/// </summary>
struct DataCell
{
    FieldType type = FieldType::Unknown;
    std::string value;
    std::vector<std::string> arrayValues;
    std::vector<std::vector<std::string>> vectorArrayValues;
    bool boolValue = false;
};
/// <summary>
/// 一行真实数据，最终数据
/// </summary>
struct DataRecord
{
    int id = 0;
    std::vector<DataCell> cells;
};
/// <summary>
/// 整张表
/// </summary>
struct TableData
{
    std::filesystem::path sourcePath;
    std::string fileName;
    std::string tableName;
    std::string className;
    char delimiter = ',';
    char arrayDelimiter = '|';

    std::vector<RawRow> rawRows;
    std::vector<ColumnInfo> allColumns;
    std::vector<ColumnInfo> exportColumns;
    std::vector<DataRecord> records;
};
/// <summary>
/// 错误信息
/// </summary>
struct ExportError
{
    std::string file;
    int lineNumber = 0;
    std::string fieldName;
    std::string reason;
};
/// <summary>
/// 导出完整报告
/// </summary>
struct ExportReport
{
    std::vector<std::string> successTables;
    std::vector<std::string> skippedTables;
    std::vector<std::string> failedTables;
    std::vector<std::string> warnings;
    std::vector<ExportError> errors;
};

char DelimiterModeToChar(DelimiterMode mode);
std::string DelimiterModeToString(DelimiterMode mode);
DelimiterMode ParseDelimiterMode(const std::string& text);

FieldType ParseFieldType(const std::string& text);
std::string FieldTypeToString(FieldType type);
std::string FieldTypeToCSharpType(FieldType type);
bool IsArrayFieldType(FieldType type);
bool IsUnityVectorFieldType(FieldType type);
int GetVectorComponentCount(FieldType type);
