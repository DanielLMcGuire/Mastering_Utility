@echo off
cd /d "%~dp0"
dotnet tool install --global wix
wix extension add WixToolset.UI.wixext
mkdir "%1\dist"
wix build install.wxs -arch x64 -ext WixToolset.UI.wixext -o "%1\dist\masteringutil64.msi"
