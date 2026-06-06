#pragma once

#include "TableData.h"

#include <filesystem>
#include <string>
#include <vector>
/// <summary>
/// 读取表格功能接口
/// </summary>
class ITableReader
{
public:
    //接口类一般都会写虚析构函数
    virtual ~ITableReader() = default;
    //=0为纯虚函数，使ITableReader本身无法直接创建对象，但是子类必须实现
    virtual bool Read(const std::filesystem::path& filePath,
                      DelimiterMode delimiterMode,
                      TableData& outTable,
                      std::vector<ExportError>& errors) = 0;
};

bool IsSupportedTableExtension(const std::filesystem::path& filePath);
bool IsTextTableExtension(const std::filesystem::path& filePath);
bool IsExcelTableExtension(const std::filesystem::path& filePath);

