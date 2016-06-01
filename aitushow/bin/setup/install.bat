@echo off
setlocal EnableDelayedExpansion
goto :start
:error
cls
echo 提示^:%1
echo ^-^>按任意键退出
pause>nul
exit

:start
set nsisexepath="C:\Program Files (x86)\NSIS\makensis.exe"
set nsispath="%~dp0nsis\install.nsi"
set exepath="%~dp0input_main\program\kuaikan.exe"
set exepath=%exepath:\=\\%
echo %exepath%
for /f "skip=1" %%i in ('wmic datafile where "Name='%exepath:~1,-1%'" get Version') do (
  echo %%i
  set ver=%%i
  echo !ver!
  set ver=!ver:~6,3!
  echo !ver!
  goto :nsis

)
:nsis
if not DEFINED ver (
  call :error 获取主程序版本失败
) else (
  %nsisexepath% /DBuildNum=!ver! %nsispath%
)

pause
@echo on