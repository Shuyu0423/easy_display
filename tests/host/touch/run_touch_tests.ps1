param(
    [string]$CC = "gcc"
)

$ErrorActionPreference = "Stop"

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..\..\..")
$buildDir = Join-Path $PSScriptRoot "build"
$cst816ExePath = Join-Path $buildDir "test_touch_cst816.exe"
$xpt2046ExePath = Join-Path $buildDir "test_touch_xpt2046.exe"

New-Item -ItemType Directory -Force $buildDir | Out-Null

& $CC -std=c99 `
    "-I$repoRoot\driver_pack\input\touch\core\inc" `
    "-I$repoRoot\driver_pack\input\touch\controller\cst816\inc" `
    "$repoRoot\tests\host\touch\test_touch_cst816.c" `
    "$repoRoot\driver_pack\input\touch\core\src\touch_driver.c" `
    "$repoRoot\driver_pack\input\touch\controller\cst816\src\cst816.c" `
    -o $cst816ExePath

if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

& $cst816ExePath
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

& $CC -std=c99 `
    "-I$repoRoot\driver_pack\input\touch\core\inc" `
    "-I$repoRoot\driver_pack\input\touch\controller\xpt2046\inc" `
    "$repoRoot\tests\host\touch\test_touch_xpt2046.c" `
    "$repoRoot\driver_pack\input\touch\core\src\touch_driver.c" `
    "$repoRoot\driver_pack\input\touch\controller\xpt2046\src\xpt2046.c" `
    -o $xpt2046ExePath

if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

& $xpt2046ExePath
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

Write-Host "touch CST816 host tests passed"
Write-Host "touch XPT2046 host tests passed"
