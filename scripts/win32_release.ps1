# Copyright 2025 Daniel McGuire
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

Write-Host "Building Mastering Utility...`n" -ForegroundColor Cyan
try {
    Write-Host "Generating..." -ForegroundColor Cyan
    cmake -S .. -B ..\build -DCMAKE_BUILD_TYPE=Release -G Ninja -DCMAKE_C_COMPILER="clang-cl.exe" -DCMAKE_CXX_COMPILER="clang-cl.exe"
    if ($LASTEXITCODE -ne 0) { throw "cmake generation failed" }
    Write-Host "Generated." -ForegroundColor Green

    Write-Host "Building..." -ForegroundColor Cyan
    cmake --build ..\build --config Release
    if ($LASTEXITCODE -ne 0) { throw "cmake build failed" }
    Write-Host "Built" -ForegroundColor Green

    Write-Host "Running Unit Tests..." -ForegroundColor Cyan
    ..\build\MasteringTests.exe
    if ($LASTEXITCODE -ne 0) { throw "unit tests failed" }
    Write-Host "Unit Tests ran successfully!" -ForegroundColor Green

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