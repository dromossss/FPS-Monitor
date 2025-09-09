<#
.SYNOPSIS
    Harvest redistributable dependency binaries for Cef\Bin and Cef\Resources into WiX .wxs fragments.
#>

[CmdletBinding()]
param()

# Ensure the WIX installation base is defined via the WIX environment variable
if (-not $env:WIX) {
    Write-Error 'Environment variable WIX is not set. Please install WIX or set WIX to the WiX installation directory.'
    exit 1
}

# Locate heat.exe in the WiX bin folder
$heatExe = Join-Path $env:WIX 'bin\heat.exe'
if (-not (Test-Path $heatExe)) {
    Write-Error "heat.exe not found at '$heatExe'. Verify your WIX environment variable points to the correct location."
    exit 1
}

# Switch to the script directory so relative paths resolve correctly
Push-Location $PSScriptRoot

# Define the harvest jobs: source folder, component group ID, and output .wxs file name
$jobs = @(
    @{ Source = '..\Cef\Bin';       ComponentGroup = 'CefBinaries'; Out = '..\..\PMInstaller\CefBinaries.wxs' },
    @{ Source = '..\Cef\Resources'; ComponentGroup = 'CefResources'; Out = '..\..\PMInstaller\CefResources.wxs' }
)

foreach ($job in $jobs) {
    Write-Host "Harvesting '$($job.Source)' → '$($job.Out)'"
    & $heatExe dir $job.Source `
         -srd -sreg -scom `
         -dr pm_app_folder `
         -cg $job.ComponentGroup `
         -var var.PresentMon.TargetDir `
         -suid -g1 -ag `
         -out $job.Out

    if ($LASTEXITCODE -ne 0) {
        Write-Error "heat.exe failed for '$($job.Source)' (exit code $LASTEXITCODE)"
        Pop-Location
        exit $LASTEXITCODE
    }
}

Pop-Location
Write-Host 'Harvesting completed successfully.'