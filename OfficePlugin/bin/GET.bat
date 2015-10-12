@echo off
cd /d %~dp0
copy /y "..\code\x64\Release\WordEncLock64.dll" ".\release\"
copy /y "..\code\Release\WordEncLock.dll" ".\release\"
@echo on