#include "ExportSummary.h"
/// <summary>
/// 统计成功数量
/// </summary>
/// <returns>成功数量</returns>
int ExportSummary::SuccessCount() const
{
    int count = 0;
    for (const TableExportResult& result : results)
    {
        if (result.success)
        {
            ++count;
        }
    }
    return count;
}
/// <summary>
/// 统计失败数量
/// </summary>
/// <returns>失败数量</returns>
int ExportSummary::FailedCount() const
{
    int count = 0;
    for (const TableExportResult& result : results)
    {
        if (!result.success && !result.skipped && !result.unsupported)
        {
            ++count;
        }
    }
    return count;
}
/// <summary>
/// 统计跳过数量
/// </summary>
/// <returns>跳过数量</returns>
int ExportSummary::SkippedCount() const
{
    int count = 0;
    for (const TableExportResult& result : results)
    {
        if (result.skipped)
        {
            ++count;
        }
    }
    return count;
}
/// <summary>
/// 统计不受支持数量
/// </summary>
/// <returns>不受支持</returns>
int ExportSummary::UnsupportedCount() const
{
    int count = 0;
    for (const TableExportResult& result : results)
    {
        if (result.unsupported)
        {
            ++count;
        }
    }
    return count;
}
/// <summary>
/// 处理文件数量
/// </summary>
/// <returns>返回处理文件数量</returns>
int ExportSummary::ProcessableFileCount() const
{
    int count = 0;
    for (const TableExportResult& result : results)
    {
        if (!result.skipped && !result.unsupported)
        {
            ++count;
        }
    }
    return count;
}
/// <summary>
/// 定义统计结构，返回处理结果文字
/// </summary>
/// <returns>处理结果</returns>
std::string ExportSummary::StatusText() const
{
    const int successCount = SuccessCount();
    const int failedCount = FailedCount();
    const bool hasGlobalErrors = !globalErrors.empty();

    if (ProcessableFileCount() == 0)
    {
        return "失败";
    }
    if (successCount > 0 && failedCount == 0 && !hasGlobalErrors)
    {
        return "成功";
    }
    if (successCount > 0 && (failedCount > 0 || hasGlobalErrors))
    {
        return "部分成功";
    }
    return "失败";
}

