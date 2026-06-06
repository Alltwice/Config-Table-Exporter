#pragma once

#include <string>
#include <vector>

namespace StringUtils
{
std::string Trim(const std::string& value);
std::string ToLower(std::string value);
std::string RemoveUtf8Bom(const std::string& value);
std::vector<std::string> Split(const std::string& value, char delimiter, bool keepEmpty = true);
bool IsBlankRow(const std::vector<std::string>& cells);

bool TryParseInt(const std::string& value, int& outValue);
bool TryParseLongLong(const std::string& value, long long& outValue);
bool IsValidInteger(const std::string& value);
bool IsValidLongInteger(const std::string& value);
bool IsValidShortInteger(const std::string& value);
bool IsValidByteInteger(const std::string& value);
bool IsValidNumber(const std::string& value);
bool TryParseBool(const std::string& value, bool& outValue);

std::string EscapeJson(const std::string& value);
std::string EscapeCSharpComment(const std::string& value);

std::string ToPascalCase(const std::string& value);
std::string ToCamelCase(const std::string& value);
bool IsCSharpIdentifier(const std::string& value);
}
