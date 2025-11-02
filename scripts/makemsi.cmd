@echo off
cd /d "%~1"
where /q wix >NUL
if ERRORLEVEL 1 (
	dotnet tool install --global wix
)
if not exist ".wix" (
wix extension add WixToolset.UI.wixext
)
if not exist "dist" (
mkdir "dist"
)
wix build install.wxs -arch x64 -ext WixToolset.UI.wixext -o "dist\masteringutil64.msi"

