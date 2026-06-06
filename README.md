# Config-Table-Exporter

Config-Table-Exporter 是一个基于 C++17 的 Unity 游戏配置表导出工具。它可以批量读取配置表，将 Excel/CSV/TSV/TXT 风格的表格数据导出为格式化 JSON，并自动生成 Unity C# 配置类和 `ConfigManager`，方便运行时通过 `id` 查询配置。

当前版本已经完整支持 `.csv`、`.tsv`、`.txt`。`.xlsx/.xlsm` 可通过 CMake + vcpkg + OpenXLSX 启用；如果构建时未找到 OpenXLSX，会保留清晰的降级错误，不影响文本表导出。`.xls` 是旧二进制格式，当前仍明确标记为暂不支持。

## 功能列表

- 控制台交互式输入导出路径和分隔符模式。
- 批量扫描输入目录中的配置表。
- 支持 `.csv`、`.tsv`、`.txt`。
- 支持分隔符模式：`auto`、`comma`、`semicolon`、`tab`、`pipe`。
- 文件名以 `#` 开头的表会被整体跳过。
- 字段名以 `#` 开头的列会被忽略，不导出 JSON，不生成 C# 字段。
- 支持类型：`int`、`long`、`short`、`byte`、`float`、`double`、`string`、`bool`、`Vector2`、`Vector3`。
- 支持数组：`int[]`、`long[]`、`short[]`、`byte[]`、`float[]`、`double[]`、`string[]`、`bool[]`、`Vector2[]`、`Vector3[]`。
- 支持数组字段，默认数组元素分隔符为 `|`；当主分隔符也是 `|` 时，数组元素分隔符自动改为 `;`。
- 校验字段名、字段类型、列数量、类型转换、`id` 主键、重复 `id`。
- 生成格式化 JSON。
- 生成 Unity `[Serializable]` C# 配置类。
- 生成 Unity `ConfigManager.cs`，使用 `Resources.Load<TextAsset>()` 加载 JSON。
- 生成 `Output/export_report.txt` 导出报告。

## 表格格式

配置表固定使用前三行作为表头：

```text
第 1 行：字段名
第 2 行：字段类型
第 3 行：字段说明
第 4 行开始：真实数据
```

示例：

```csv
id,name,damage,cooldown,isAOE,rewardIds,#note
int,string,int,float,bool,int[],string
技能ID,技能名,伤害,冷却时间,是否范围技能,奖励ID列表,备注
1001,FireBall,120,3.5,true,1|2|3,测试备注
1002,IceSpike,90,2.0,false,4|5,测试备注
```

有效字段的第一列必须是 `id`，且类型必须为 `int`。这是 Unity 运行时 `Dictionary<int, XxxConfig>` 查询的主键。

`Vector2` 使用 `x|y`，`Vector3` 使用 `x|y|z`。如果主分隔符选择 `pipe`，向量内部分隔符自动改为 `;`，例如 `1;2;3`。

`Vector2[]` / `Vector3[]` 使用 `/` 分隔多个向量元素，单个向量内部仍使用数组分隔符，例如：

```csv
waypoints
Vector3[]
路径点
0|0|0/1|1|1/2|2|2
```

## 控制台使用方式

当前版本不使用命令行参数，运行程序后按提示输入：

```text
请输入表格输入文件夹路径，直接回车使用默认 ./Tables：
请输入 JSON 输出文件夹路径，直接回车使用默认 ./Output/Json：
请输入 C# 输出文件夹路径，直接回车使用默认 ./Output/CSharp：
请输入 ConfigManager 输出路径，直接回车使用默认 ./Output/CSharp/ConfigManager.cs：
请选择分隔符模式，auto/comma/semicolon/tab/pipe，直接回车使用 auto：
```

全部直接回车时使用默认值：

```text
input = ./Tables
jsonOutput = ./Output/Json
csOutput = ./Output/CSharp
managerOutput = ./Output/CSharp/ConfigManager.cs
delimiter = auto
```

## 构建方式

### MinGW g++

当前开发环境检测到可用的 MinGW `g++`。在项目根目录运行：

```powershell
.\build_mingw.ps1
```

或手动运行：

```powershell
g++ -std=c++17 -Wall -Wextra -pedantic -finput-charset=UTF-8 -fexec-charset=UTF-8 src/*.cpp -o Config-Table-Exporter.exe
```

MinGW g++ 构建时请确保所有 `.h/.cpp` 源文件以 UTF-8 保存。`build_mingw.ps1` 已显式添加 `-finput-charset=UTF-8 -fexec-charset=UTF-8`，用于保证 UTF-8 源码字面量按 UTF-8 编译和输出。

该脚本不链接 OpenXLSX，适合验证 CSV/TSV/TXT 和所有字段类型。需要 `.xlsx/.xlsm` 时请使用下面的 CMake + vcpkg 方式。

### CMake

如果本机已安装 CMake：

```powershell
cmake -S . -B build
cmake --build build --config Release
```

CMake 工程在 MSVC 下会自动添加 `/utf-8` 编译选项，避免中文源码字面量被按本地 ANSI 代码页解释。

如果 CMake 找到 OpenXLSX，会自动定义 `CONFIG_TABLE_EXPORTER_HAS_OPENXLSX` 并启用 `.xlsx/.xlsm` 读取；如果没有找到，程序仍可编译运行，Excel 文件会在报告中显示为不支持。

## 示例 JSON 输出

`Tables/Skill.csv` 会生成 `Output/Json/SkillConfig.json`：

```json
{
  "items": [
    {
      "id": 1001,
      "name": "FireBall",
      "damage": 120,
      "cooldown": 3.5,
      "isAOE": true,
      "rewardIds": [1, 2, 3]
    }
  ]
}
```

`#note` 不会出现在 JSON 中。

## 示例 Unity 使用代码

将生成的 JSON 放到 Unity 项目：

```text
Assets/Resources/Configs/SkillConfig.json
Assets/Resources/Configs/MonsterConfig.json
```

将生成的 C# 文件放到 Unity 项目：

```text
Assets/Scripts/GeneratedConfigs/SkillConfig.cs
Assets/Scripts/GeneratedConfigs/MonsterConfig.cs
Assets/Scripts/GeneratedConfigs/ConfigManager.cs
```

运行时查询：

```csharp
SkillConfig skill = ConfigManager.Instance.GetSkillConfig(1001);
Debug.Log(skill.name);

MonsterConfig monster = ConfigManager.Instance.GetMonsterConfig(2001);
Debug.Log(monster.name);
```

## 忽略规则

- 表文件名以 `#` 开头时整表跳过，例如 `#DraftSkill.csv`、`#Monster.xlsx`。
- 字段名以 `#` 开头时忽略该列，例如 `#note`、`#comment`、`#designerMemo`。
- 忽略列允许在数据行中存在，但不参与类型校验、JSON 导出、C# 类生成和 ConfigManager 生成。
- 数据行的原始列数量仍然必须和原始字段名数量一致，用于发现漏填列或多填列。

## Excel 支持说明

当前代码已为 OpenXLSX 预留条件编译接入：构建时找到 OpenXLSX 后，`.xlsx/.xlsm` 会读取第一个工作表并进入和 CSV 相同的校验/导出流程。未找到 OpenXLSX 时，`.xlsx/.xlsm` 会输出清晰错误并写入导出报告，不会静默跳过。

推荐启用方式：

1. 安装 CMake。
2. 安装 vcpkg。
3. 使用 vcpkg 安装 OpenXLSX：

```powershell
git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
C:\vcpkg\bootstrap-vcpkg.bat
C:\vcpkg\vcpkg.exe install openxlsx
```

如果需要指定 64 位 Windows triplet，可以使用 `openxlsx:x64-windows`。

4. 使用 vcpkg toolchain 构建：

```powershell
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake
cmake --build build --config Release
```

`.xls` 是旧二进制 Excel 格式，OpenXLSX 通常不支持。建议将 `.xls` 另存为 `.xlsx`，或额外接入 libxls/libxlsxio 并在 README 中标明限制。

## 项目亮点

- C++17 标准库实现主流程，工程结构清晰，适合课程设计和简历展示。
- 读取、校验、导出、Unity 代码生成分层独立，便于扩展。
- 校验失败的表不会生成错误 JSON 和 C#。
- 导出报告包含成功、跳过、失败和详细错误。
- 生成的 ConfigManager 不写死 Skill/Monster，会根据成功导出的表动态生成加载和查询代码。

## 后续可扩展方向

- 支持命令行参数和 CI 自动导表。
- 支持选择 Excel 指定 Sheet，而不是固定读取第一个 Sheet。
- 支持 enum 字段类型和对应 C# 枚举生成。
- 支持二进制导出，减少 Unity 运行时 JSON 解析成本。
- 支持 Unity 菜单一键导表。
- 支持表间引用校验，例如技能表引用道具表。
- 支持字段默认值、枚举类型、嵌套对象和多语言表。
- 支持生成 Addressables 加载版本的 ConfigManager。
