$hash = Get-FileHash -Path "masteringutil64.msi" -Algorithm SHA256
"$($hash.Hash)  masteringutil64.msi" | Out-File -FilePath "masteringutil64.msi.sha256" -Encoding ASCII
$hash = Get-FileHash -Path "masteringutil64_setup.exe" -Algorithm SHA256
"$($hash.Hash)  masteringutil64_setup.exe" | Out-File -FilePath "masteringutil64-setup.exe.sha256" -Encoding ASCII
