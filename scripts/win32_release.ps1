## Copyright 2025 Daniel McGuire
##
## Licensed under the Apache License, Version 2.0 (the "License");
## you may not use this file except in compliance with the License.
## You may obtain a copy of the License at
##
##     http://www.apache.org/licenses/LICENSE-2.0
##
## Unless required by applicable law or agreed to in writing, software
## distributed under the License is distributed on an "AS IS" BASIS,
## WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
## See the License for the specific language governing permissions and
## limitations under the License.

Write-Host "Building Mastering Utility...`n" -ForegroundColor Cyan
try {
    Write-Host "Generating..." -ForegroundColor Cyan
    cmake -S .. -B ..\build -DCMAKE_BUILD_TYPE=Release -G Ninja
    if ($LASTEXITCODE -ne 0) { throw "cmake generation failed" }
    Write-Host "Generated." -ForegroundColor Green

    Write-Host "Building..." -ForegroundColor Cyan
    cmake --build ..\build --config Release
    if ($LASTEXITCODE -ne 0) { throw "cmake build failed" }
    Write-Host "Built" -ForegroundColor Green

    Write-Host "Copying files..." -ForegroundColor Cyan
    cp ..\build\MasteringUtility.exe .
    cp ..\build\MasteringWizard.exe .
    Write-Host "Copied files." -ForegroundColor Green

    Write-Host "Building MSI Installer..." -ForegroundColor Cyan
    .\makemsi.cmd
    if ($LASTEXITCODE -ne 0) { throw "makemsi.cmd failed" }
    Write-Host "Built MSI Installer." -ForegroundColor Green

    cd ..
    Write-Host "Generating Documentation..." -ForegroundColor Cyan
    doxygen
    if ($LASTEXITCODE -ne 0) { throw "doxygen failed" }
    Write-Host "Generated Documentation." -ForegroundColor Green

    cd scripts
    Write-Host "Building EXE Installer..." -ForegroundColor Cyan
    .\makensi.cmd
    if ($LASTEXITCODE -ne 0) { throw "makensi.cmd failed" }
    Write-Host "Built EXE Installer." -ForegroundColor Green

    Write-Host "`nFinished Building Mastering Utility." -ForegroundColor Green
}
catch {
    Write-Host "`n[ERROR] $($_.Exception.Message)" -ForegroundColor Red
    Write-Host "Build aborted." -ForegroundColor Yellow
    exit 1
}
