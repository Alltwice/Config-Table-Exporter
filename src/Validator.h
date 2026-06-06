#pragma once

#include "TableData.h"
/// <summary>
/// 数据校验类
/// </summary>
class Validator
{
public:
    /// <summary>
    /// 校验整张表
    /// </summary>
    /// <param name="table">表格</param>
    /// <param name="report">完整报告</param>
    /// <returns>校验结果</returns>
    bool Validate(TableData& table, ExportReport& report) const;

private:
    /// <summary>
    /// 添加统一的错误信息
    /// </summary>
    void AddError(const TableData& table,
                  ExportReport& report,
                  int lineNumber,
                  const std::string& fieldName,
                  const std::string& reason) const;
    /// <summary>
    /// 校验一个单元格数据
    /// </summary>
    bool ValidateCell(const TableData& table,
                      const ColumnInfo& column,
                      const std::string& rawValue,
                      int lineNumber,
                      DataCell& outCell,
                      ExportReport& report) const;
};

