@echo off
cd /d "%~dp0"
wix extension add WixToolset.UI.wixext
wix build install.wxs -arch x64 -ext WixToolset.UI.wixext -o "masteringutil64.msi"