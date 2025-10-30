@echo off
cd /d "%~dp0"
wix extension add WixToolset.UI.wixext
mkdir "..\build\dist"
wix build install.wxs -arch x64 -ext WixToolset.UI.wixext -o "..\build\dist\masteringutil64.msi"