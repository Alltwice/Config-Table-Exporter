#include "ReportGenerator.h"

#include <fstream>
#include <iostream>
#include <sstream>

std::string ReportGenerator::Generate(const ExportSummary& summary, bool includeExitPrompt) const
{
    std::ostringstream output;

    AppendSectionLine(output, '=');
    output << "             导出结果汇总\n";
    AppendSectionLine(output, '=');
    output << "\n";

    output << "导出状态：" << summary.StatusText() << "\n\n";
    output << "输入目录：" << summary.inputPath << "\n";
    output << "JSON 输出目录：" << summary.jsonOutputPath << "\n";
    output << "C# 输出目录：" << summary.csharpOutputPath << "\n";
    output << "ConfigManager 输出路径：" << summary.managerOutputPath << "\n";
    output << "导出报告路径：" << summary.reportOutputPath << "\n";
    if (summary.ProcessableFileCount() == 0)
    {
        output << "\n未找到可导出的配置表文件。\n";
    }

    output << "\n";
    AppendSectionLine(output, '-');
    output << "处理统计\n";
    AppendSectionLine(output, '-');
    output << "扫描到的文件数量：" << summary.scannedFileCount << "\n";
    output << "成功导出的表数量：" << summary.SuccessCount() << "\n";
    output << "失败的表数量：" << summary.FailedCount() << "\n";
    output << "被忽略的表数量：" << summary.SkippedCount() << "\n";
    output << "不支持的文件数量：" << summary.UnsupportedCount() << "\n";

    if (!summary.globalErrors.empty())
    {
        output << "\n";
        AppendSectionLine(output, '-');
        output << "全局错误\n";
        AppendSectionLine(output, '-');
        for (const std::string& error : summary.globalErrors)
        {
            output << "[Error] " << error << "\n";
        }
    }

    if (!summary.globalWarnings.empty())
    {
        output << "\n";
        AppendSectionLine(output, '-');
        output << "全局警告\n";
        AppendSectionLine(output, '-');
        for (const std::string& warning : summary.globalWarnings)
        {
            output << "[Warn] " << warning << "\n";
        }
    }

    output << "\n";
    AppendSectionLine(output, '-');
    output << "成功导出的表\n";
    AppendSectionLine(output, '-');
    bool hasSuccess = false;
    for (const TableExportResult& result : summary.results)
    {
        if (!result.success)
        {
            continue;
        }
        hasSuccess = true;
        output << "[OK] " << result.fileName << "\n";
        if (!result.tableName.empty())
        {
            output << "     表名：" << result.tableName << "\n";
        }
        output << "     有效字段数：" << result.validFieldCount << "\n";
        output << "     忽略字段数：" << result.ignoredFieldCount << "\n";
        output << "     数据行数：" << result.dataRowCount << "\n";
        output << "     生成 JSON：" << result.jsonOutputPath << "\n";
        output << "     生成 C#：" << result.csharpOutputPath << "\n";
        for (const std::string& warning : result.warnings)
        {
            output << "     [Warn] " << warning << "\n";
        }
        output << "\n";
    }
    if (!hasSuccess)
    {
        AppendEmptyList(output);
    }

    output << "\n";
    AppendSectionLine(output, '-');
    output << "失败的表\n";
    AppendSectionLine(output, '-');
    bool hasFailed = false;
    for (const TableExportResult& result : summary.results)
    {
        if (result.success || result.skipped || result.unsupported)
        {
            continue;
        }
        hasFailed = true;
        output << "[Error] " << result.fileName << "\n";
        output << "        错误数量：" << result.errors.size() << "\n";
        if (result.errors.empty())
        {
            output << "        未记录具体错误。\n";
        }
        for (const std::string& error : result.errors)
        {
            output << "        " << error << "\n";
        }
        for (const std::string& warning : result.warnings)
        {
            output << "        [Warn] " << warning << "\n";
        }
        output << "\n";
    }
    if (!hasFailed)
    {
        AppendEmptyList(output);
    }

    output << "\n";
    AppendSectionLine(output, '-');
    output << "被忽略的表\n";
    AppendSectionLine(output, '-');
    bool hasSkipped = false;
    for (const TableExportResult& result : summary.results)
    {
        if (!result.skipped)
        {
            continue;
        }
        hasSkipped = true;
        output << "[Skip] " << result.fileName << "\n";
        output << "       原因：" << result.skipReason << "\n\n";
    }
    if (!hasSkipped)
    {
        AppendEmptyList(output);
    }

    output << "\n";
    AppendSectionLine(output, '-');
    output << "不支持的文件\n";
    AppendSectionLine(output, '-');
    bool hasUnsupported = false;
    for (const TableExportResult& result : summary.results)
    {
        if (!result.unsupported)
        {
            continue;
        }
        hasUnsupported = true;
        output << "[Unsupported] " << result.fileName << "\n";
        output << "              原因：" << result.unsupportedReason << "\n\n";
    }
    if (!hasUnsupported)
    {
        AppendEmptyList(output);
    }

    output << "\n";
    AppendSectionLine(output, '-');
    output << "最终生成文件\n";
    AppendSectionLine(output, '-');
    output << "JSON 文件数量：" << summary.SuccessCount() << "\n";
    output << "C# 配置类数量：" << summary.SuccessCount() << "\n";
    output << "ConfigManager：" << (summary.configManagerGenerated ? "已生成" : "未生成") << "\n";
    output << "导出报告：" << (summary.reportGenerated ? "已生成" : "未生成") << "\n";

    if (includeExitPrompt)
    {
        output << "\n";
        AppendSectionLine(output, '=');
        output << "请按 Enter 键退出...\n";
        AppendSectionLine(output, '=');
    }

    return output.str();
}

void ReportGenerator::PrintToConsole(const ExportSummary& summary) const
{
    std::cout << Generate(summary, true);
    std::cout.flush();
}

bool ReportGenerator::WriteToFile(const ExportSummary& summary, const std::filesystem::path& outputPath) const
{
    std::error_code ec;
    const std::filesystem::path parentPath = outputPath.parent_path();
    if (!parentPath.empty())
    {
        std::filesystem::create_directories(parentPath, ec);
    }
    if (ec)
    {
        return false;
    }

    ExportSummary fileSummary = summary;
    fileSummary.reportGenerated = true;

    std::ofstream output(outputPath, std::ios::binary);
    if (!output.is_open())
    {
        return false;
    }

    output << Generate(fileSummary, false);
    return true;
}

void ReportGenerator::AppendSectionLine(std::ostream& output, char ch) const
{
    for (int i = 0; i < 40; ++i)
    {
        output << ch;
    }
    output << "\n";
}

void ReportGenerator::AppendEmptyList(std::ostream& output) const
{
    output << "- 无\n";
}
