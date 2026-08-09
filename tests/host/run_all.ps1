param(
    [string]$CC = "gcc"
)

$ErrorActionPreference = "Stop"

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..\..")

& powershell -ExecutionPolicy Bypass -File "$repoRoot\driver_pack\log\tests\host\run_host_syntax.ps1" -CC $CC
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

& powershell -ExecutionPolicy Bypass -File "$repoRoot\tests\host\touch\run_touch_tests.ps1" -CC $CC
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

Write-Host "all host tests passed"
