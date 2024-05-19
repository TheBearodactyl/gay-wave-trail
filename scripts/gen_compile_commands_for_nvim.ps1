if ($null -eq $Env:NDK_ROOT) {
  throw "Please install the Android NDK"
}

geode.exe build -p android64 | Out-Null

if (Test-Path -Path .\build-android64) {
  Write-Host "Generated android64 build files"
}

cmake -A win32 -B build | Out-Null

if (Test-Path -Path .\build) {
  Write-Host "Generated win32 build files"
}

if (Test-Path -Path .\build-android64\compile_commands.json && Test-Path -Path .\build\ && Test-Path -Path .\build\compile_commands.json -ErrorAction Stop) {
  Copy-Item -Path .\build-android64\compile_commands.json -Destination .\build\ -Force
}

Remove-Item -Recurse -Force -Path .\build-android64\

if (Test-Path -Path .\build\compile_commands.json) {
  Write-Host " Successfuly generated compile_commands.json"
  Write-Host "-----------------------------------------------"
  Write-Host "NeoVim should now be 100% compatible with Geode"
}
