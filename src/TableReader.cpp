#include "TableReader.h"
#include "StringUtils.h"

namespace
{
/// <summary>
/// 读取文件后缀的小工具类
/// </summary>
std::string ExtensionOf(const std::filesystem::path& filePath)
{
    return StringUtils::ToLower(filePath.extension().string());
}
}
/// <summary>
/// 判断是否为纯文本格式
/// </summary>
bool IsSupportedTableExtension(const std::filesystem::path& filePath)
{
    return IsTextTableExtension(filePath) || IsExcelTableExtension(filePath);
}
/// <summary>
/// 判断是否为文本类
/// </summary>
bool IsTextTableExtension(const std::filesystem::path& filePath)
{
    const std::string ext = ExtensionOf(filePath);
    return ext == ".csv" || ext == ".tsv" || ext == ".txt";
}
/// <summary>
/// 判断是否为表格类
/// </summary>
bool IsExcelTableExtension(const std::filesystem::path& filePath)
{
    const std::string ext = ExtensionOf(filePath);
    return ext == ".xlsx" || ext == ".xlsm" || ext == ".xls";
}

