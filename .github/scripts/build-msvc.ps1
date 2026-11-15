# Run from the repository root in an x64 Visual Studio developer PowerShell.
# Tcl and Tcllib source trees must be at _ci/tcl and _ci/tcllib.
$ErrorActionPreference = 'Stop'
$root = (Get-Location).Path
$prefix = Join-Path $root '_ci/runtime'
function Invoke-Nmake {
    & nmake /nologo /f makefile.vc @args
    if ($LASTEXITCODE -ne 0) { throw "nmake failed with exit code $LASTEXITCODE" }
}
Push-Location _ci/tcl/win
try {
    Invoke-Nmake "INSTALLDIR=$prefix" release
    Invoke-Nmake "INSTALLDIR=$prefix" install
} finally { Pop-Location }

$env:TCLLIBPATH = '{' + (Join-Path $root '_ci/tcllib/modules').Replace('\', '/') + '}'
Push-Location win
try {
    Invoke-Nmake "INSTALLDIR=$prefix"
    Invoke-Nmake "INSTALLDIR=$prefix" test
    Invoke-Nmake "INSTALLDIR=$prefix" install
} finally { Pop-Location }

$package = Join-Path $root '_ci/package'
New-Item -ItemType Directory -Force $package | Out-Null
Copy-Item "$prefix/lib/argparse*" $package -Recurse
Copy-Item LICENSE $package
