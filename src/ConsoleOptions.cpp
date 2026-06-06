#include "ConsoleOptions.h"
#include "StringUtils.h"

#include <iostream>
/// <summary>
/// 处理文件路径
/// </summary>
/// <returns>返回选项结构体供使用</returns>
ConsoleOptions ConsoleOptionsReader::Read() const
{
    ConsoleOptions options;

    const std::string input = Prompt("请输入表格输入文件夹路径，直接回车使用默认 ./Tables：", options.inputDir.string());
    const std::string jsonOutput = Prompt("请输入 JSON 输出文件夹路径，直接回车使用默认 ./Output/Json：", options.jsonOutputDir.string());
    const std::string csharpOutput = Prompt("请输入 C# 输出文件夹路径，直接回车使用默认 ./Output/CSharp：", options.csharpOutputDir.string());
    const std::string managerOutput = Prompt("请输入 ConfigManager 输出路径，直接回车使用默认 ./Output/CSharp/ConfigManager.cs：",
                                             options.managerOutputPath.string());
    const std::string delimiter = Prompt("请选择分隔符模式，auto/comma/semicolon/tab/pipe，直接回车使用 auto：",
                                         DelimiterModeToString(options.delimiterMode));

    options.inputDir = input.empty() ? options.inputDir : std::filesystem::path(input);
    options.jsonOutputDir = jsonOutput.empty() ? options.jsonOutputDir : std::filesystem::path(jsonOutput);
    options.csharpOutputDir = csharpOutput.empty() ? options.csharpOutputDir : std::filesystem::path(csharpOutput);
    options.managerOutputPath = managerOutput.empty() ? options.managerOutputPath : std::filesystem::path(managerOutput);
    options.delimiterMode = delimiter.empty() ? DelimiterMode::Auto : ParseDelimiterMode(delimiter);

    return options;
}
//拆解以下这份函数：
//std::string为引用string类，返回ConsoleOptionsReader类型的结果::Promot为类内申明类外实现必带

/// <summary>
/// 控制台显示提示以及接收输入/默认信息
/// </summary>
/// <param name="message">各类输入信息</param>
/// <param name="">默认信息</param>
/// <returns>返回</returns>
std::string ConsoleOptionsReader::Prompt(const std::string& message, const std::string&) const
{
    std::cout << message << std::endl;
    std::string value;
    std::getline(std::cin, value);
    //去掉Utf8的Bom头防止无法解析文件路径
    return StringUtils::Trim(StringUtils::RemoveUtf8Bom(value));
}
