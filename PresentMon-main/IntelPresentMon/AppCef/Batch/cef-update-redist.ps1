<#
.SYNOPSIS
  Full CEF redist workflow: build wrapper, pull binaries, generate WiX harvest.

.DESCRIPTION
  1. Builds the CEF C++ wrapper via cef-build-wrapper.ps1  
  2. Pulls in CEF binaries, libs, includes, and resources via pull-cef.bat  
  3. Runs the WiX harvest script wix-cef-redist.ps1 (no args; works relative to its own folder)

.PARAMETER RedistPath
  (Positional) Path to the root of your CEF redist directory (the folder containing build\cef.sln).

.EXAMPLE
  .\manage-cef-redist.ps1 "C:\path\to\cef\redist"
#>

param(
    [Parameter(Mandatory = $true, Position = 0)]
    [string]$RedistPath
)

# Resolve and verify the redist path
try {
    $RedistPath = (Resolve-Path $RedistPath).ProviderPath
} catch {
    Write-Error "Cannot resolve path: $RedistPath"
    exit 1
}
if (-not (Test-Path $RedistPath -PathType Container)) {
    Write-Error "Not a valid directory: $RedistPath"
    exit 1
}

# Locate helper scripts next to this script
$scriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Definition
$cefBuild   = Join-Path $scriptRoot 'cef-build-wrapper.ps1'
$pullBat    = Join-Path $scriptRoot 'pull-cef.bat'
$wixHarvest = Join-Path $scriptRoot 'wix-cef-redist.ps1'

# Ensure they all exist
foreach ($f in @($cefBuild, $pullBat, $wixHarvest)) {
    if (-not (Test-Path $f)) {
        Write-Error "Required script not found: $f"
        exit 1
    }
}

# 1) Build the CEF wrapper
Write-Host "=== Step 1/3: Building CEF C++ wrapper ==="
& $cefBuild $RedistPath
if ($LASTEXITCODE -ne 0) {
    Write-Error "cef-build-wrapper.ps1 failed (exit code $LASTEXITCODE)."
    exit $LASTEXITCODE
}

# Steps 2 & 3 need to run from the scripts folder
Push-Location $scriptRoot

# 2) Pull in CEF redist files
Write-Host "`n=== Step 2/3: Pulling in CEF redist files ==="
& $pullBat $RedistPath
if ($LASTEXITCODE -ne 0) {
    Write-Error "pull-cef.bat failed (exit code $LASTEXITCODE)."
    Pop-Location; exit $LASTEXITCODE
}

# 3) Generate WiX fragments
Write-Host "`n=== Step 3/3: Generating WiX harvest ==="
& $wixHarvest
if ($LASTEXITCODE -ne 0) {
    Write-Error "wix-cef-redist.ps1 failed (exit code $LASTEXITCODE)."
    Pop-Location; exit $LASTEXITCODE
}

Pop-Location

Write-Host "`n✅ All steps completed successfully."
