#include "ExcelReader.h"
#include "StringUtils.h"

#ifdef CONFIG_TABLE_EXPORTER_HAS_OPENXLSX
#include <OpenXLSX.hpp>
#include <iomanip>
#include <sstream>
#endif

#ifdef CONFIG_TABLE_EXPORTER_HAS_OPENXLSX
namespace
{
std::string CellValueToString(const OpenXLSX::XLCell& cell)
{
    const OpenXLSX::XLCellValue value = cell.value();
    switch (value.type())
    {
    case OpenXLSX::XLValueType::Empty:
        return "";
    case OpenXLSX::XLValueType::Boolean:
        return value.get<bool>() ? "true" : "false";
    case OpenXLSX::XLValueType::Integer:
        return std::to_string(value.get<int64_t>());
    case OpenXLSX::XLValueType::Float:
    {
        std::ostringstream oss;
        oss << std::setprecision(15) << value.get<double>();
        return oss.str();
    }
    case OpenXLSX::XLValueType::String:
        return value.get<std::string>();
    case OpenXLSX::XLValueType::Error:
    default:
        return "";
    }
}
}
#endif

bool ExcelReader::Read(const std::filesystem::path& filePath,
                       DelimiterMode delimiterMode,
                       TableData& outTable,
                       std::vector<ExportError>& errors)
{
    const std::string ext = StringUtils::ToLower(filePath.extension().string());

    if (ext == ".xls")
    {
        errors.push_back({
            filePath.filename().string(),
            0,
            "",
            "旧版二进制 .xls 当前未接入第三方库，暂不支持；建议另存为 .xlsx 或按 README 接入 libxls/libxlsxio"
        });
        return false;
    }

#ifndef CONFIG_TABLE_EXPORTER_HAS_OPENXLSX
    (void)delimiterMode;
    (void)outTable;
    // 未链接 OpenXLSX 时保留清晰降级路径，保证文本表导出工具仍可独立编译运行。
    errors.push_back({
        filePath.filename().string(),
        0,
        "",
        ".xlsx/.xlsm 需要 OpenXLSX 或 libxlsxio 支持；当前版本已保留 ExcelReader 接口，请按 README 的手动步骤接入"
    });
    return false;
#else
    try
    {
        OpenXLSX::XLDocument doc;
        doc.open(filePath.string());

        const std::vector<std::string> worksheetNames = doc.workbook().worksheetNames();
        if (worksheetNames.empty())
        {
            errors.push_back({ filePath.filename().string(), 0, "", "Excel 文件没有可读取的工作表" });
            doc.close();
            return false;
        }

        OpenXLSX::XLWorksheet worksheet = doc.workbook().worksheet(worksheetNames.front());
        const uint32_t rowCount = worksheet.rowCount();
        const uint16_t columnCount = worksheet.columnCount();

        outTable = TableData {};
        outTable.sourcePath = filePath;
        outTable.fileName = filePath.filename().string();
        outTable.tableName = filePath.stem().string();
        outTable.className = StringUtils::ToPascalCase(outTable.tableName) + "Config";
        outTable.delimiter = delimiterMode == DelimiterMode::Pipe ? '|' : ',';
        outTable.arrayDelimiter = outTable.delimiter == '|' ? ';' : '|';

        for (uint32_t rowIndex = 1; rowIndex <= rowCount; ++rowIndex)
        {
            std::vector<std::string> cells;
            cells.reserve(columnCount);
            for (uint16_t columnIndex = 1; columnIndex <= columnCount; ++columnIndex)
            {
                const std::string value = CellValueToString(worksheet.cell(rowIndex, columnIndex));
                cells.push_back(StringUtils::Trim(value));
            }

            if (StringUtils::IsBlankRow(cells))
            {
                continue;
            }
            outTable.rawRows.push_back({ static_cast<int>(rowIndex), cells });
        }

        doc.close();
        return true;
    }
    catch (const std::exception& ex)
    {
        errors.push_back({ filePath.filename().string(), 0, "", "读取 Excel 文件失败：" + std::string(ex.what()) });
        return false;
    }
#endif
}
