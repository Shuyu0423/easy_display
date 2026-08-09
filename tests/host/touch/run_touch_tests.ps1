param(
    [string]$CC = "gcc"
)

$ErrorActionPreference = "Stop"

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..\..\..")
$buildDir = Join-Path $PSScriptRoot "build"
$exePath = Join-Path $buildDir "test_touch_cst816.exe"

New-Item -ItemType Directory -Force $buildDir | Out-Null

& $CC -std=c99 `
    "-I$repoRoot\driver_pack\input\touch\core\inc" `
    "-I$repoRoot\driver_pack\input\touch\controller\cst816\inc" `
    "$repoRoot\tests\host\touch\test_touch_cst816.c" `
    "$repoRoot\driver_pack\input\touch\core\src\touch_driver.c" `
    "$repoRoot\driver_pack\input\touch\controller\cst816\src\cst816.c" `
    -o $exePath

if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

& $exePath
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

Write-Host "touch CST816 host tests passed"
