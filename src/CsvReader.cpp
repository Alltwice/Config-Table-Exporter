#include "CsvReader.h"
#include "StringUtils.h"
#include <fstream>
/// <summary>
/// 读取CSV文件
/// </summary>
/// <param name="filePath">文件路径</param>
/// <param name="delimiterMode">分割模式</param>
/// <param name="outTable">修改外部的表格文件</param>
/// <param name="errors">存储错误信息</param>
/// <returns>返回成功信息</returns>
bool CsvReader::Read(const std::filesystem::path& filePath,
                     DelimiterMode delimiterMode,
                     TableData& outTable,
                     std::vector<ExportError>& errors)
{
    //正常文件读取操作
    std::ifstream input(filePath);
    if (!input.is_open())
    {
        errors.push_back({ filePath.filename().string(), 0, "", "无法打开文件" });
        return false;
    }
    //保存每一行以及其行数标
    //pair是一个二元组，将其理解为一个通用两组数据的结构体即可
    std::vector<std::pair<int, std::string>> lines;
    //保存第一个非空行用于处理数据
    std::string firstDataLine;
    //每一行
    std::string line;
    int lineNumber = 0;
    //按CSV文件的特性只需要按行读取即可
    while (std::getline(input, line))
    {
        //行数增加
        ++lineNumber;
        //处理windows可能会被留在末尾的\r分割符避免出现问题
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }
        if (lineNumber == 1)
        {
            //去掉BOM头防止读取失败
            line = StringUtils::RemoveUtf8Bom(line);
        }
        //在去掉前后空格的情况下如果还是空行就跳过
        if (StringUtils::Trim(line).empty())
        {
            continue;
        }
        if (firstDataLine.empty())
        {
            firstDataLine = line;
        }
        //将行数和行信息统一存放到vector中
        lines.emplace_back(lineNumber, line);
    }
    //处理分割符
    const char delimiter = ResolveDelimiter(filePath, delimiterMode, firstDataLine);
    //开始填写表格信息
    outTable = TableData {};
    outTable.sourcePath = filePath;
    outTable.fileName = filePath.filename().string();
    outTable.tableName = filePath.stem().string();
    outTable.className = StringUtils::ToPascalCase(outTable.tableName) + "Config";
    outTable.delimiter = delimiter;
    outTable.arrayDelimiter = delimiter == '|' ? ';' : '|';

    for (const auto& item : lines)
    {
        std::vector<std::string> cells = ParseLine(item.second, delimiter);
        //跳过空白行
        if (StringUtils::IsBlankRow(cells))
        {
            continue;
        }
        //压入一行数据
        outTable.rawRows.push_back({ item.first, cells });
    }

    return true;
}
/// <summary>
/// 识别分隔符信息
/// </summary>
char CsvReader::ResolveDelimiter(const std::filesystem::path& filePath,
                                 DelimiterMode delimiterMode,
                                 const std::string& firstDataLine) const
{
    if (delimiterMode != DelimiterMode::Auto)
    {
        return DelimiterModeToChar(delimiterMode);
    }
    //转小写拿后缀
    const std::string ext = StringUtils::ToLower(filePath.extension().string());
    //依据不同文件确定不同后缀
    if (ext == ".tsv")
    {
        return '\t';
    }
    if (ext == ".csv")
    {
        return ',';
    }
    //如果都不是进一步处理
    return DetectDelimiter(firstDataLine);
}
/// <summary>
/// 记录出现最多的分隔符作为最终结果
/// </summary>
char CsvReader::DetectDelimiter(const std::string& line) const
{
    const char candidates[] = { ',', ';', '\t', '|' };
    char bestDelimiter = ',';
    size_t bestCount = 0;
    for (char delimiter : candidates)
    {
        size_t count = 0;
        for (char c : line)
        {
            if (c == delimiter)
            {
                ++count;
            }
        }
        if (count > bestCount)
        {
            bestCount = count;
            bestDelimiter = delimiter;
        }
    }
    return bestDelimiter;
}
/// <summary>
/// 分割方法
/// </summary>
std::vector<std::string> CsvReader::ParseLine(const std::string& line, char delimiter) const
{
    std::vector<std::string> cells;
    std::string current;
    //是否在""内部
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); ++i)
    {
        const char c = line[i];
        if (c == '"')
        {
            //如果在引号内部检测到了""会替换为一个"用于表示双引号
            if (inQuotes && i + 1 < line.size() && line[i + 1] == '"')
            {
                current.push_back('"');
                ++i;
            }
            //如果遇到引号开启内部/外部提示
            else
            {
                inQuotes = !inQuotes;
            }
        }
        else if (c == delimiter && !inQuotes)
        {
            cells.push_back(StringUtils::Trim(current));
            current.clear();
        }
        else
        {
            current.push_back(c);
        }
    }
    //压入最后一个单元格内的数据，因为最后一格不会遇见分割符了
    cells.push_back(StringUtils::Trim(current));
    return cells;
}

