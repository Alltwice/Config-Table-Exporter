$ErrorActionPreference = "Stop"

$sourceFiles = Get-ChildItem -Path "$PSScriptRoot\src" -Filter "*.cpp" | ForEach-Object { $_.FullName }
$output = Join-Path $PSScriptRoot "Config-Table-Exporter.exe"

g++ -std=c++17 -Wall -Wextra -pedantic -finput-charset=UTF-8 -fexec-charset=UTF-8 $sourceFiles -o $output
if ($LASTEXITCODE -ne 0) {
    throw "Build failed with exit code $LASTEXITCODE"
}
Write-Host "Build succeeded: $output"
