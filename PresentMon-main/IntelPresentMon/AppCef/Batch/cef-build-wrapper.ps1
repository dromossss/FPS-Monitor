<#
.SYNOPSIS
  Out-of-source build of the CEF C++ wrapper.

.DESCRIPTION
  1. Takes a single, positional parameter: the path to your CEF redist root (the dir with CMakeLists.txt).  
  2. If `<redist>/build` already exists, exits immediately.  
  3. Finds VS via vswhere and locates vcvarsall.bat.  
  4. Creates and cds into `<redist>/build`.  
  5. Invokes CMake **directly**.  
  6. Invokes msbuild for Release and then Debug (each in its own vcvarsall/msbuild invocation).

.PARAMETER RedistPath
  (Positional) Path to the root of your CEF redist directory.
#>

param(
    [Parameter(Mandatory=$true, Position=0)]
    [string]$RedistPath
)

# Resolve & verify
try { $RedistPath = (Resolve-Path $RedistPath).ProviderPath } catch {
    Write-Error "Cannot resolve path: $RedistPath"; exit 1
}
if (-not (Test-Path $RedistPath -PathType Container)) {
    Write-Error "Not a valid directory: $RedistPath"; exit 1
}

# Skip if already built
$buildDir = Join-Path $RedistPath "build"
if (Test-Path $buildDir) {
    Write-Host "Found existing build directory: $buildDir"
    Write-Host "→ Skipping configuration & build."
    exit 0
}

# Locate VS / vcvarsall
$vswhere = "C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $vswhere)) { Write-Error "vswhere.exe not found"; exit 1 }
$vsRoot = & $vswhere -latest -products * -prerelease -requires Microsoft.Component.MSBuild -property installationPath
if (-not $vsRoot) { Write-Error "No Visual Studio with MSBuild found"; exit 1 }
$vcvars = Join-Path $vsRoot "VC\Auxiliary\Build\vcvarsall.bat"
if (-not (Test-Path $vcvars)) { Write-Error "vcvarsall.bat not found"; exit 1 }
Write-Host "Using vcvarsall.bat at: $vcvars`n"

# Make build dir & enter
Write-Host "Creating build directory at $buildDir"
New-Item -ItemType Directory -Path $buildDir | Out-Null
Push-Location $buildDir

# Settings
$Arch      = "x64"
$Platform  = "x64"
$Generator = "Visual Studio 17"
$Solution  = Join-Path $buildDir "cef.sln"

# 1) Configure with CMake (no vcvars needed)
Write-Host "→ Running CMake configure..."
cmake -G "$Generator" -A $Platform -DUSE_SANDBOX=OFF "$RedistPath"
if ($LASTEXITCODE -ne 0) {
    Write-Error "CMake configuration failed (exit code $LASTEXITCODE)"; Pop-Location; exit $LASTEXITCODE
}

# 2) Build Release
Write-Host "`n→ Building Release..."
& cmd.exe /c "`"$vcvars`" $Arch && msbuild `"$Solution`" /m /p:Configuration=Release;Platform=$Platform"
if ($LASTEXITCODE -ne 0) {
    Write-Error "Release build failed (exit code $LASTEXITCODE)"; Pop-Location; exit $LASTEXITCODE
}

# 3) Build Debug
Write-Host "`n→ Building Debug..."
& cmd.exe /c "`"$vcvars`" $Arch && msbuild `"$Solution`" /m /p:Configuration=Debug;Platform=$Platform"
if ($LASTEXITCODE -ne 0) {
    Write-Error "Debug build failed (exit code $LASTEXITCODE)"; Pop-Location; exit $LASTEXITCODE
}

Pop-Location
Write-Host "`n✅ All done: CEF wrapper built in $buildDir"
