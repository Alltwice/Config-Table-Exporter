#pragma once

#include <string>
#include <vector>
/// <summary>
/// 单个文件导出报告
/// </summary>
struct TableExportResult
{
    //文件名称路径
    std::string fileName;
    std::string filePath;
    std::string tableName;
    //成功跳过是否支持
    bool success = false;
    bool skipped = false;
    bool unsupported = false;
    //跳过和不支持原因
    std::string skipReason;
    std::string unsupportedReason;
    //统计文件数量
    int originalFieldCount = 0;
    int validFieldCount = 0;
    int ignoredFieldCount = 0;
    int dataRowCount = 0;
    //输出位置
    std::string jsonOutputPath;
    std::string csharpOutputPath;
    //错误和警告类型
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};
/// <summary>
/// 文件导出过程的总体报告
/// </summary>
struct ExportSummary
{
    std::string inputPath;
    std::string jsonOutputPath;
    std::string csharpOutputPath;
    std::string managerOutputPath;
    std::string reportOutputPath;

    int scannedFileCount = 0;
    bool configManagerGenerated = false;
    bool reportGenerated = false;

    std::vector<TableExportResult> results;
    std::vector<std::string> globalErrors;
    std::vector<std::string> globalWarnings;

    int SuccessCount() const;
    int FailedCount() const;
    int SkippedCount() const;
    int UnsupportedCount() const;
    int ProcessableFileCount() const;
    std::string StatusText() const;
};

