#include "CSharpExporter.h"
#include "ConfigManagerExporter.h"
#include "ConsoleOptions.h"
#include "CsvReader.h"
#include "EncodingUtils.h"
#include "ExcelReader.h"
#include "ExportSummary.h"
#include "JsonExporter.h"
#include "Logger.h"
#include "ReportGenerator.h"
#include "StringUtils.h"
#include "TableReader.h"
#include "Validator.h"
#include <algorithm>
#include <filesystem>
#include <iostream>
//命名空间存在的意义在于减少重名函数
//C++不会自动将脚本编译为程序集
//每一个脚本文件都需要带有其对应的申明（头文件）
//在其他脚本中被引用其头文件后才能被看到
//匿名命名空间，仅在当前脚本中可被使用
//不适用全局命名空间的目的是防止污染程序
namespace
{
/// <summary>
/// 处理忽略行/列
/// </summary>
/// <param name="path">传入文件路径</param>
/// <returns>文件存在且被标记为忽略</returns>
bool StartsWithIgnorePrefix(const std::filesystem::path& path)
{
    //filename为取出路径中的最后的文件名
    const std::string fileName = path.filename().string();
    return !fileName.empty() && fileName[0] == '#';
}
/// <summary>
/// 给出文件报告路径
/// </summary>
/// <param name="options">传入设置结构体</param>
/// <returns>返回文件路径</returns>
std::filesystem::path GetReportPath(const ConsoleOptions& options)
{
    std::filesystem::path parent = options.jsonOutputDir.parent_path();
    if (parent.empty())
    {
        parent = ".";
    }
    return parent / "export_report.txt";
}

std::string PathToDisplayString(const std::filesystem::path& path)
{
    //将转译符号变为/
    return path.generic_string();
}

std::string FormatErrorDetail(const ExportError& error)
{
    std::string message;
    if (error.lineNumber > 0)
    {
        message += "第 " + std::to_string(error.lineNumber) + " 行";
    }
    if (!error.fieldName.empty())
    {
        if (!message.empty())
        {
            message += " ";
        }
        if (error.reason.rfind("字段", 0) == 0)
        {
            message += error.fieldName + " " + error.reason;
            return message;
        }
        if (error.reason.rfind(error.fieldName, 0) == 0)
        {
            message += error.reason;
            return message;
        }
        message += error.fieldName + " 字段";
    }
    if (!error.reason.empty())
    {
        if (!message.empty())
        {
            message += " ";
        }
        message += error.reason;
    }
    return message;
}

std::string FormatErrorForLog(const ExportError& error)
{
    std::string message = error.file;
    const std::string detail = FormatErrorDetail(error);
    if (!detail.empty())
    {
        message += " " + detail;
    }
    return message;
}
/// <summary>
/// 给最终报告填入基础信息
/// </summary>
TableExportResult CreateBaseResult(const std::filesystem::path& filePath)
{
    TableExportResult result;
    result.fileName = filePath.filename().string();
    result.filePath = PathToDisplayString(filePath);
    result.tableName = filePath.stem().string();
    return result;
}
/// <summary>
/// 统一压入错误信息
/// </summary>
void AppendReadErrors(TableExportResult& result, const std::vector<ExportError>& errors)
{
    for (const ExportError& error : errors)
    {
        result.errors.push_back(FormatErrorDetail(error));
    }
}

void AppendErrorsFromReportRange(TableExportResult& result,
                                 const ExportReport& report,
                                 size_t beginIndex)
{
    for (size_t i = beginIndex; i < report.errors.size(); ++i)
    {
        if (report.errors[i].file == result.fileName)
        {
            result.errors.push_back(FormatErrorDetail(report.errors[i]));
        }
    }
}

void LogReadErrors(const std::vector<ExportError>& errors)
{
    for (const ExportError& error : errors)
    {
        Logger::Error(FormatErrorForLog(error));
    }
}

void LogReportErrorsForFile(const ExportReport& report,
                            const std::string& fileName,
                            size_t beginIndex)
{
    for (size_t i = beginIndex; i < report.errors.size(); ++i)
    {
        if (report.errors[i].file == fileName)
        {
            Logger::Error(FormatErrorForLog(report.errors[i]));
        }
    }
}
/// <summary>
/// 不支持当前文件的原因报告
/// </summary>
std::string GetUnsupportedExtensionReason(const std::filesystem::path& filePath)
{
    const std::string ext = filePath.extension().string();
    if (ext.empty())
    {
        return "当前版本不支持无扩展名文件";
    }
    return "当前版本不支持 " + ext + " 文件";
}
/// <summary>
/// 判断读取失败的原因/xlsx/xls/其他
/// </summary>
bool ShouldTreatReadFailureAsUnsupported(const std::filesystem::path& filePath,
                                         const std::vector<ExportError>& readErrors)
{
    if (!IsExcelTableExtension(filePath))
    {
        return false;
    }

    const std::string ext = StringUtils::ToLower(filePath.extension().string());
    if (ext == ".xls")
    {
        return true;
    }

    for (const ExportError& error : readErrors)
    {
        if (error.reason.find("暂不支持") != std::string::npos ||
            error.reason.find("需要 OpenXLSX") != std::string::npos)
        {
            return true;
        }
    }
    return false;
}

std::string FirstErrorReasonOrDefault(const std::vector<ExportError>& errors,
                                      const std::string& fallback)
{
    if (!errors.empty() && !errors.front().reason.empty())
    {
        return errors.front().reason;
    }
    return fallback;
}

void FillSuccessResult(TableExportResult& result,
                       const TableData& table,
                       const ConsoleOptions& options)
{
    result.success = true;
    result.tableName = table.tableName;
    result.originalFieldCount = static_cast<int>(table.allColumns.size());
    result.validFieldCount = static_cast<int>(table.exportColumns.size());
    result.ignoredFieldCount = 0;
    for (const ColumnInfo& column : table.allColumns)
    {
        if (column.ignored)
        {
            ++result.ignoredFieldCount;
        }
    }
    result.dataRowCount = static_cast<int>(table.records.size());
    result.jsonOutputPath = PathToDisplayString(options.jsonOutputDir / (table.className + ".json"));
    result.csharpOutputPath = PathToDisplayString(options.csharpOutputDir / (table.className + ".cs"));
}

void AppendGlobalErrorsFromReport(ExportSummary& summary,
                                  const ExportReport& report,
                                  size_t beginIndex)
{
    for (size_t i = beginIndex; i < report.errors.size(); ++i)
    {
        summary.globalErrors.push_back(FormatErrorForLog(report.errors[i]));
    }
}

void WaitForExit()
{
    std::cin.clear();
    std::string ignored;
    std::getline(std::cin, ignored);
}
}

int main()
{
    //优先处理UTF8编码格式
    InitConsoleEncoding();
    //处理设置路径/分割符的设置类
    ConsoleOptionsReader optionsReader;
    //设置结构体调用读取设置函数
    ConsoleOptions options = optionsReader.Read();
    //得到文件报告路径
    const std::filesystem::path reportPath = GetReportPath(options);
    //处理结果报告
    ExportSummary summary;
    //转移转移符号为较为通用的/
    summary.inputPath = PathToDisplayString(options.inputDir);
    summary.jsonOutputPath = PathToDisplayString(options.jsonOutputDir);
    summary.csharpOutputPath = PathToDisplayString(options.csharpOutputDir);
    summary.managerOutputPath = PathToDisplayString(options.managerOutputPath);
    summary.reportOutputPath = PathToDisplayString(reportPath);
    //在得到所有对应信息后，在控制台得到最终结果
    std::cout << "\n本次导出配置：" << std::endl;
    std::cout << "  表格输入目录: " << summary.inputPath << std::endl;
    std::cout << "  JSON 输出目录: " << summary.jsonOutputPath << std::endl;
    std::cout << "  C# 输出目录: " << summary.csharpOutputPath << std::endl;
    std::cout << "  ConfigManager 输出: " << summary.managerOutputPath << std::endl;
    std::cout << "  分隔符模式: " << DelimiterModeToString(options.delimiterMode) << std::endl;
    std::cout << std::endl;
    //创建错误报告
    ExportReport legacyReport;
    //创建成功表
    std::vector<TableData> successfulTables;
    //创建错误码对象
    std::error_code ec; 
    //如果成功创建JSON文件，如果失败则将失败信息写入ec
    std::filesystem::create_directories(options.jsonOutputDir, ec);
    if (ec)
    {
        //失败原因+信息
        const std::string message = "创建 JSON 输出目录失败：" + ec.message();
        //输出错误信息
        Logger::Error(message);
        //将错误信息存储
        summary.globalErrors.push_back(message);
    }
    //情况错误信息
    ec.clear();
    //创建C#脚本同理
    std::filesystem::create_directories(options.csharpOutputDir, ec);
    if (ec)
    {
        const std::string message = "创建 C# 输出目录失败：" + ec.message();
        Logger::Error(message);
        summary.globalErrors.push_back(message);
    }

    ec.clear();
    //如果上级目录为空
    if (!options.managerOutputPath.parent_path().empty())
    {
        //尝试创建上级目录
        std::filesystem::create_directories(options.managerOutputPath.parent_path(), ec);
    }
    if (ec)
    {
        const std::string message = "创建 ConfigManager 输出目录失败：" + ec.message();
        Logger::Error(message);
        summary.globalErrors.push_back(message);
    }
    //文件
    std::vector<std::filesystem::path> files;
    //查找输入文件路径，不存在报错
    if (!std::filesystem::exists(options.inputDir))
    {
        const std::string message = "输入目录不存在：" + options.inputDir.string();
        Logger::Error(message);
        summary.globalErrors.push_back(message);
    }
    else
    {
        ec.clear();
        //遍历文件夹，用auto存储未知文件类型
        for (const auto& entry : std::filesystem::directory_iterator(options.inputDir, ec))
        {
            //判断如果是普通文件则加入集合
            if (entry.is_regular_file())
            {
                files.push_back(entry.path());
            }
        }
        if (ec)
        {
            const std::string message = "扫描输入目录失败：" + ec.message();
            Logger::Error(message);
            summary.globalErrors.push_back(message);
        }
    }

    std::sort(files.begin(), files.end());
    //让文件大小的size_t类型转为int类型
    summary.scannedFileCount = static_cast<int>(files.size());

    CsvReader csvReader;
    ExcelReader excelReader;
    Validator validator;
    JsonExporter jsonExporter;
    CSharpExporter csharpExporter;
    //这里时最终调用方法开始导出的部位
    for (const std::filesystem::path& filePath : files)
    {
        TableExportResult result = CreateBaseResult(filePath);
        const std::string fileName = result.fileName;

        if (!IsSupportedTableExtension(filePath))
        {
            result.unsupported = true;
            result.unsupportedReason = GetUnsupportedExtensionReason(filePath);
            Logger::Warn(fileName + " " + result.unsupportedReason);
            summary.results.push_back(result);
            continue;
        }

        if (StartsWithIgnorePrefix(filePath))
        {
            result.skipped = true;
            result.skipReason = "文件名以 # 开头";
            Logger::Info("跳过 # 前缀表：" + fileName);
            legacyReport.skippedTables.push_back(fileName);
            summary.results.push_back(result);
            continue;
        }

        Logger::Info("开始导出：" + fileName);
        TableData table;
        std::vector<ExportError> readErrors;
        bool readOk = false;
        //如果是纯文本类
        if (IsTextTableExtension(filePath))
        {
            readOk = csvReader.Read(filePath, options.delimiterMode, table, readErrors);
        }
        //如果是表格类
        else if (IsExcelTableExtension(filePath))
        {
            readOk = excelReader.Read(filePath, options.delimiterMode, table, readErrors);
        }

        if (!readOk)
        {
            LogReadErrors(readErrors);
            if (ShouldTreatReadFailureAsUnsupported(filePath, readErrors))
            {
                result.unsupported = true;
                result.unsupportedReason = FirstErrorReasonOrDefault(readErrors, "当前版本暂不支持该文件");
            }
            else
            {
                AppendReadErrors(result, readErrors);
                if (result.errors.empty())
                {
                    result.errors.push_back("读取失败，未返回具体错误");
                }
                legacyReport.failedTables.push_back(fileName);
            }
            summary.results.push_back(result);
            continue;
        }

        size_t errorStart = legacyReport.errors.size();
        if (!validator.Validate(table, legacyReport))
        {
            AppendErrorsFromReportRange(result, legacyReport, errorStart);
            LogReportErrorsForFile(legacyReport, fileName, errorStart);
            if (result.errors.empty())
            {
                result.errors.push_back("校验失败，未记录具体错误");
            }
            Logger::Error(fileName + " 校验失败，跳过生成 JSON 和 C#");
            legacyReport.failedTables.push_back(fileName);
            summary.results.push_back(result);
            continue;
        }

        errorStart = legacyReport.errors.size();
        if (!jsonExporter.Export(table, options.jsonOutputDir, legacyReport) ||
            !csharpExporter.Export(table, options.csharpOutputDir, legacyReport))
        {
            AppendErrorsFromReportRange(result, legacyReport, errorStart);
            LogReportErrorsForFile(legacyReport, fileName, errorStart);
            if (result.errors.empty())
            {
                result.errors.push_back("导出文件失败，未记录具体错误");
            }
            Logger::Error(fileName + " 导出文件失败");
            legacyReport.failedTables.push_back(fileName);
            summary.results.push_back(result);
            continue;
        }

        FillSuccessResult(result, table, options);
        successfulTables.push_back(table);
        legacyReport.successTables.push_back(fileName);
        summary.results.push_back(result);
        Logger::Info("导出成功：" + fileName);
    }

    ConfigManagerExporter managerExporter;
    const size_t managerErrorStart = legacyReport.errors.size();
    summary.configManagerGenerated = managerExporter.Export(successfulTables, options.managerOutputPath, legacyReport);
    if (!summary.configManagerGenerated)
    {
        Logger::Error("ConfigManager 生成失败");
        AppendGlobalErrorsFromReport(summary, legacyReport, managerErrorStart);
        if (legacyReport.errors.size() == managerErrorStart)
        {
            summary.globalErrors.push_back("ConfigManager 生成失败，未记录具体错误");
        }
    }

    for (const std::string& warning : legacyReport.warnings)
    {
        summary.globalWarnings.push_back(warning);
    }

    ReportGenerator reportGenerator;
    summary.reportGenerated = reportGenerator.WriteToFile(summary, reportPath);
    if (!summary.reportGenerated)
    {
        const std::string message = "导出报告写入失败：" + summary.reportOutputPath;
        Logger::Error(message);
        summary.globalErrors.push_back(message);
    }

    std::cout << "\n";
    reportGenerator.PrintToConsole(summary);
    WaitForExit();

    return summary.StatusText() == "成功" ? 0 : 2;
}
