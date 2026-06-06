#include "ConfigManagerExporter.h"
#include "StringUtils.h"

#include <fstream>

bool ConfigManagerExporter::Export(const std::vector<TableData>& tables,
                                   const std::filesystem::path& outputPath,
                                   ExportReport& report) const
{
    std::error_code ec;
    const std::filesystem::path parentPath = outputPath.parent_path();
    if (!parentPath.empty())
    {
        std::filesystem::create_directories(parentPath, ec);
    }
    if (ec)
    {
        report.errors.push_back({ "ConfigManager", 0, "", "创建 ConfigManager 输出目录失败：" + ec.message() });
        return false;
    }

    std::ofstream output(outputPath, std::ios::binary);
    if (!output.is_open())
    {
        report.errors.push_back({ "ConfigManager", 0, "", "无法写入 ConfigManager 文件：" + outputPath.string() });
        return false;
    }

    output << "using System.Collections.Generic;\n";
    output << "using UnityEngine;\n\n";
    output << "public class ConfigManager\n";
    output << "{\n";
    output << "    private const string ConfigRoot = \"Configs/\";\n";
    output << "    private static ConfigManager _instance;\n";
    output << "    private bool _initialized;\n\n";
    output << "    public static ConfigManager Instance\n";
    output << "    {\n";
    output << "        get\n";
    output << "        {\n";
    output << "            if (_instance == null)\n";
    output << "            {\n";
    output << "                _instance = new ConfigManager();\n";
    output << "            }\n";
    output << "            return _instance;\n";
    output << "        }\n";
    output << "    }\n\n";

    for (const TableData& table : tables)
    {
        const std::string memberName = StringUtils::ToCamelCase(table.className) + "Dict";
        output << "    private readonly Dictionary<int, " << table.className << "> " << memberName
               << " = new Dictionary<int, " << table.className << ">();\n";
    }
    output << "\n";

    output << "    private ConfigManager()\n";
    output << "    {\n";
    output << "        LoadAll();\n";
    output << "    }\n\n";

    output << "    public void LoadAll()\n";
    output << "    {\n";
    output << "        if (_initialized)\n";
    output << "        {\n";
    output << "            return;\n";
    output << "        }\n\n";
    for (const TableData& table : tables)
    {
        output << "        Load" << table.className << "();\n";
    }
    output << "        _initialized = true;\n";
    output << "    }\n\n";

    for (const TableData& table : tables)
    {
        const std::string memberName = StringUtils::ToCamelCase(table.className) + "Dict";
        output << "    private void Load" << table.className << "()\n";
        output << "    {\n";
        output << "        TextAsset textAsset = Resources.Load<TextAsset>(ConfigRoot + \"" << table.className << "\");\n";
        output << "        if (textAsset == null)\n";
        output << "        {\n";
        output << "            Debug.LogError(\"[ConfigManager] JSON 文件不存在: \" + ConfigRoot + \"" << table.className << "\");\n";
        output << "            return;\n";
        output << "        }\n\n";
        output << "        " << table.className << "List list = JsonUtility.FromJson<" << table.className
               << "List>(textAsset.text);\n";
        output << "        if (list == null || list.items == null)\n";
        output << "        {\n";
        output << "            Debug.LogError(\"[ConfigManager] JSON 解析失败: " << table.className << "\");\n";
        output << "            return;\n";
        output << "        }\n\n";
        output << "        " << memberName << ".Clear();\n";
        output << "        foreach (" << table.className << " item in list.items)\n";
        output << "        {\n";
        output << "            if (item == null)\n";
        output << "            {\n";
        output << "                continue;\n";
        output << "            }\n";
        output << "            if (" << memberName << ".ContainsKey(item.id))\n";
        output << "            {\n";
        output << "                Debug.LogError($\"[ConfigManager] " << table.className << " 存在重复 id: {item.id}\");\n";
        output << "                continue;\n";
        output << "            }\n";
        output << "            " << memberName << ".Add(item.id, item);\n";
        output << "        }\n";
        output << "    }\n\n";

        output << "    public " << table.className << " Get" << table.className << "(int id)\n";
        output << "    {\n";
        output << "        LoadAll();\n";
        output << "        if (" << memberName << ".TryGetValue(id, out " << table.className << " config))\n";
        output << "        {\n";
        output << "            return config;\n";
        output << "        }\n";
        output << "        Debug.LogError($\"[ConfigManager] " << table.className << " 未找到 id: {id}\");\n";
        output << "        return null;\n";
        output << "    }\n\n";
    }

    output << "}\n";
    return true;
}
