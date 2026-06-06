#include "EncodingUtils.h"
//预处理编码，如果不是在Windows平台则不处理以下代码
#ifdef _WIN32
#include <windows.h>
#endif
/// <summary>
/// 开始前设定输入和输出格式为UTF8防止乱码
/// </summary>
void InitConsoleEncoding()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
}

