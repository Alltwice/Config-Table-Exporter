#include "TableData.h"
#include "StringUtils.h"
/// <summary>
/// 分割模式枚举
/// </summary>
/// <param name="mode">模式</param>
/// <returns>分割符号</returns>
char DelimiterModeToChar(DelimiterMode mode)
{
    switch (mode)
    {
    case DelimiterMode::Comma:
        return ',';
    case DelimiterMode::Semicolon:
        return ';';
    case DelimiterMode::Tab:
        return '\t';
    case DelimiterMode::Pipe:
        return '|';
        //与其说是Auto不如说是默认“,”
    case DelimiterMode::Auto:
    default:
        return ',';
    }
}
/// <summary>
/// 映射为字符串
/// </summary>
/// <param name="mode">模式</param>
/// <returns>字符串名称</returns>
std::string DelimiterModeToString(DelimiterMode mode)
{
    switch (mode)
    {
    case DelimiterMode::Comma:
        return "comma";
    case DelimiterMode::Semicolon:
        return "semicolon";
    case DelimiterMode::Tab:
        return "tab";
    case DelimiterMode::Pipe:
        return "pipe";
    case DelimiterMode::Auto:
    default:
        return "auto";
    }
}
/// <summary>
/// 将分割类型的字符串重新转译为枚举供使用
/// </summary>
/// <param name="text">分割类型字符串</param>
/// <returns>分割类型枚举</returns>
DelimiterMode ParseDelimiterMode(const std::string& text)
{
    //去掉前后空白后转为小写
    const std::string mode = StringUtils::ToLower(StringUtils::Trim(text));
    if (mode == "comma")
    {
        return DelimiterMode::Comma;
    }
    if (mode == "semicolon")
    {
        return DelimiterMode::Semicolon;
    }
    if (mode == "tab")
    {
        return DelimiterMode::Tab;
    }
    if (mode == "pipe")
    {
        return DelimiterMode::Pipe;
    }
    return DelimiterMode::Auto;
}
/// <summary>
/// 数据类型映射为枚举类型
/// </summary>
/// <param name="text"></param>
/// <returns></returns>
FieldType ParseFieldType(const std::string& text)
{
    const std::string type = StringUtils::ToLower(StringUtils::Trim(text));
    if (type == "int")
    {
        return FieldType::Int;
    }
    if (type == "long")
    {
        return FieldType::Long;
    }
    if (type == "short")
    {
        return FieldType::Short;
    }
    if (type == "byte")
    {
        return FieldType::Byte;
    }
    if (type == "float")
    {
        return FieldType::Float;
    }
    if (type == "double")
    {
        return FieldType::Double;
    }
    if (type == "string")
    {
        return FieldType::String;
    }
    if (type == "bool")
    {
        return FieldType::Bool;
    }
    if (type == "vector2")
    {
        return FieldType::Vector2;
    }
    if (type == "vector3")
    {
        return FieldType::Vector3;
    }
    if (type == "int[]")
    {
        return FieldType::IntArray;
    }
    if (type == "long[]")
    {
        return FieldType::LongArray;
    }
    if (type == "short[]")
    {
        return FieldType::ShortArray;
    }
    if (type == "byte[]")
    {
        return FieldType::ByteArray;
    }
    if (type == "float[]")
    {
        return FieldType::FloatArray;
    }
    if (type == "double[]")
    {
        return FieldType::DoubleArray;
    }
    if (type == "string[]")
    {
        return FieldType::StringArray;
    }
    if (type == "bool[]")
    {
        return FieldType::BoolArray;
    }
    if (type == "vector2[]")
    {
        return FieldType::Vector2Array;
    }
    if (type == "vector3[]")
    {
        return FieldType::Vector3Array;
    }
    return FieldType::Unknown;
}
/// <summary>
/// 枚举类型映射为数据类型
/// </summary>
/// <param name="type"></param>
/// <returns></returns>
std::string FieldTypeToString(FieldType type)
{
    switch (type)
    {
    case FieldType::Int:
        return "int";
    case FieldType::Long:
        return "long";
    case FieldType::Short:
        return "short";
    case FieldType::Byte:
        return "byte";
    case FieldType::Float:
        return "float";
    case FieldType::Double:
        return "double";
    case FieldType::String:
        return "string";
    case FieldType::Bool:
        return "bool";
    case FieldType::Vector2:
        return "Vector2";
    case FieldType::Vector3:
        return "Vector3";
    case FieldType::IntArray:
        return "int[]";
    case FieldType::LongArray:
        return "long[]";
    case FieldType::ShortArray:
        return "short[]";
    case FieldType::ByteArray:
        return "byte[]";
    case FieldType::FloatArray:
        return "float[]";
    case FieldType::DoubleArray:
        return "double[]";
    case FieldType::StringArray:
        return "string[]";
    case FieldType::BoolArray:
        return "bool[]";
    case FieldType::Vector2Array:
        return "Vector2[]";
    case FieldType::Vector3Array:
        return "Vector3[]";
    case FieldType::Unknown:
    default:
        return "unknown";
    }
}
/// <summary>
/// 枚举类型映射为C#/Unity脚本类型
/// </summary>
/// <param name="type"></param>
/// <returns></returns>
std::string FieldTypeToCSharpType(FieldType type)
{
    switch (type)
    {
    case FieldType::Int:
        return "int";
    case FieldType::Long:
        return "long";
    case FieldType::Short:
        return "short";
    case FieldType::Byte:
        return "byte";
    case FieldType::Float:
        return "float";
    case FieldType::Double:
        return "double";
    case FieldType::String:
        return "string";
    case FieldType::Bool:
        return "bool";
    case FieldType::Vector2:
        return "Vector2";
    case FieldType::Vector3:
        return "Vector3";
    case FieldType::IntArray:
        return "List<int>";
    case FieldType::LongArray:
        return "List<long>";
    case FieldType::ShortArray:
        return "List<short>";
    case FieldType::ByteArray:
        return "List<byte>";
    case FieldType::FloatArray:
        return "List<float>";
    case FieldType::DoubleArray:
        return "List<double>";
    case FieldType::StringArray:
        return "List<string>";
    case FieldType::BoolArray:
        return "List<bool>";
    case FieldType::Vector2Array:
        return "List<Vector2>";
    case FieldType::Vector3Array:
        return "List<Vector3>";
    case FieldType::Unknown:
    default:
        return "object";
    }
}
/// <summary>
/// 判断是否为数组
/// </summary>
/// <param name="type"></param>
/// <returns></returns>
bool IsArrayFieldType(FieldType type)
{
    return type == FieldType::IntArray ||
           type == FieldType::LongArray ||
           type == FieldType::ShortArray ||
           type == FieldType::ByteArray ||
           type == FieldType::FloatArray ||
           type == FieldType::DoubleArray ||
           type == FieldType::StringArray ||
           type == FieldType::BoolArray ||
           type == FieldType::Vector2Array ||
           type == FieldType::Vector3Array;
}
/// <summary>
/// 判断是否为Unity特殊数据类型
/// </summary>
/// <param name="type"></param>
/// <returns></returns>
bool IsUnityVectorFieldType(FieldType type)
{
    return type == FieldType::Vector2 ||
           type == FieldType::Vector3 ||
           type == FieldType::Vector2Array ||
           type == FieldType::Vector3Array;
}
/// <summary>
/// 对于UnityVector类型判断其参数数量
/// </summary>
/// <param name="type"></param>
/// <returns></returns>
int GetVectorComponentCount(FieldType type)
{
    switch (type)
    {
    case FieldType::Vector2:
    case FieldType::Vector2Array:
        return 2;
    case FieldType::Vector3:
    case FieldType::Vector3Array:
        return 3;
    default:
        return 0;
    }
}
