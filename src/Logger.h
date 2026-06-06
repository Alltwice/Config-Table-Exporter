#pragma once

#include <string>
/// <summary>
/// 用于在不同情况时在控制台显示不同信息
/// </summary>
class Logger
{
public:
    static void Info(const std::string& message);
    static void Warn(const std::string& message);
    static void Error(const std::string& message);
};

