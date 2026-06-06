

#include "StringUtils.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <limits>
#include <sstream>

namespace StringUtils
{
std::string Trim(const std::string& value)
{
    size_t begin = 0;
    while (begin < value.size() && std::isspace(static_cast<unsigned char>(value[begin])) != 0)
    {
        ++begin;
    }

    size_t end = value.size();
    while (end > begin && std::isspace(static_cast<unsigned char>(value[end - 1])) != 0)
    {
        --end;
    }

    return value.substr(begin, end - begin);
}

std::string ToLower(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return value;
}

std::string RemoveUtf8Bom(const std::string& value)
{
    if (value.size() >= 3 &&
        static_cast<unsigned char>(value[0]) == 0xEF &&
        static_cast<unsigned char>(value[1]) == 0xBB &&
        static_cast<unsigned char>(value[2]) == 0xBF)
    {
        return value.substr(3);
    }
    return value;
}
/// <summary>
/// 字符串分割方法
/// </summary>
/// <param name="value">传入字符串</param>
/// <param name="delimiter">传入分隔符</param>
/// <param name="keepEmpty">是否保留空值</param>
/// <returns>返回分割后的字符串Vector</returns>
std::vector<std::string> Split(const std::string& value, char delimiter, bool keepEmpty)
{
    std::vector<std::string> result;
    std::string current;
    for (char c : value)
    {
        if (c == delimiter)
        {
            if (keepEmpty || !current.empty())
            {
                result.push_back(current);
            }
            current.clear();
        }
        else
        {
            current.push_back(c);
        }
    }

    if (keepEmpty || !current.empty())
    {
        result.push_back(current);
    }
    return result;
}
/// <summary>
/// 如果一行在去掉首尾空格后为空则为空白
/// </summary>
/// <param name="cells">传入字符</param>
/// <returns>返回是否为空白的判断</returns>
bool IsBlankRow(const std::vector<std::string>& cells)
{
    for (const std::string& cell : cells)
    {
        if (!Trim(cell).empty())
        {
            return false;
        }
    }
    return true;
}

bool TryParseInt(const std::string& value, int& outValue)
{
    const std::string text = Trim(value);
    if (text.empty())
    {
        return false;
    }

    size_t index = 0;
    try
    {
        long long parsed = std::stoll(text, &index, 10);
        if (index != text.size() ||
            parsed < std::numeric_limits<int>::min() ||
            parsed > std::numeric_limits<int>::max())
        {
            return false;
        }
        outValue = static_cast<int>(parsed);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool TryParseLongLong(const std::string& value, long long& outValue)
{
    const std::string text = Trim(value);
    if (text.empty())
    {
        return false;
    }

    size_t index = 0;
    try
    {
        long long parsed = std::stoll(text, &index, 10);
        if (index != text.size())
        {
            return false;
        }
        outValue = parsed;
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool IsValidInteger(const std::string& value)
{
    int ignored = 0;
    return TryParseInt(value, ignored);
}

bool IsValidLongInteger(const std::string& value)
{
    long long ignored = 0;
    return TryParseLongLong(value, ignored);
}

bool IsValidShortInteger(const std::string& value)
{
    long long parsed = 0;
    return TryParseLongLong(value, parsed) &&
           parsed >= std::numeric_limits<short>::min() &&
           parsed <= std::numeric_limits<short>::max();
}

bool IsValidByteInteger(const std::string& value)
{
    long long parsed = 0;
    return TryParseLongLong(value, parsed) &&
           parsed >= 0 &&
           parsed <= 255;
}
/// <summary>
/// 判断是否为数字
/// </summary>
bool IsValidNumber(const std::string& value)
{
    const std::string text = Trim(value);
    if (text.empty())
    {
        return false;
    }

    char* end = nullptr;
    //只解析数字同时使字符指针始终指向被解析的下一位
    std::strtod(text.c_str(), &end);
    //判断当前字符是否被解析完判断是否为纯数字
    return end != text.c_str() && end != nullptr && *end == '\0';
}

bool TryParseBool(const std::string& value, bool& outValue)
{
    const std::string text = ToLower(Trim(value));
    if (text == "true" || text == "1")
    {
        outValue = true;
        return true;
    }
    if (text == "false" || text == "0")
    {
        outValue = false;
        return true;
    }
    return false;
}
/// <summary>
/// 处理双引号、反斜杠、换行、Tab 等特殊字符，将其转译为Json合法字符
/// </summary>
std::string EscapeJson(const std::string& value)
{
    std::ostringstream oss;
    for (char c : value)
    {
        switch (c)
        {
        case '\"':
            oss << "\\\"";
            break;
        case '\\':
            oss << "\\\\";
            break;
        case '\b':
            oss << "\\b";
            break;
        case '\f':
            oss << "\\f";
            break;
        case '\n':
            oss << "\\n";
            break;
        case '\r':
            oss << "\\r";
            break;
        case '\t':
            oss << "\\t";
            break;
        default:
            oss << c;
            break;
        }
    }
    return oss.str();
}

std::string EscapeCSharpComment(const std::string& value)
{
    std::string result = value;
    std::replace(result.begin(), result.end(), '\r', ' ');
    std::replace(result.begin(), result.end(), '\n', ' ');
    return result;
}

std::string ToPascalCase(const std::string& value)
{
    std::string result;
    bool nextUpper = true;

    for (unsigned char c : value)
    {
        if (std::isalnum(c) == 0)
        {
            nextUpper = true;
            continue;
        }

        if (result.empty() && std::isdigit(c) != 0)
        {
            result += "Cfg";
        }

        if (nextUpper)
        {
            result.push_back(static_cast<char>(std::toupper(c)));
            nextUpper = false;
        }
        else
        {
            result.push_back(static_cast<char>(c));
        }
    }

    return result.empty() ? "Config" : result;
}

std::string ToCamelCase(const std::string& value)
{
    std::string pascal = ToPascalCase(value);
    if (!pascal.empty())
    {
        pascal[0] = static_cast<char>(std::tolower(static_cast<unsigned char>(pascal[0])));
    }
    return pascal;
}
/// <summary>
/// 简单判断字段名是否合法的工具
/// </summary>
bool IsCSharpIdentifier(const std::string& value)
{
    if (value.empty())
    {
        return false;
    }

    auto isStart = [](unsigned char c) {
        return std::isalpha(c) != 0 || c == '_';
    };
    auto isPart = [](unsigned char c) {
        return std::isalnum(c) != 0 || c == '_';
    };

    if (!isStart(static_cast<unsigned char>(value[0])))
    {
        return false;
    }

    for (char c : value)
    {
        if (!isPart(static_cast<unsigned char>(c)))
        {
            return false;
        }
    }
    return true;
}
}
