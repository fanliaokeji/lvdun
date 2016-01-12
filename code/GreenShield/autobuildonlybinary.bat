@echo off
setlocal EnableDelayedExpansion
set vc="C:\Program Files (x86)\Microsoft Visual Studio 9.0\Common7\IDE\devenv.exe"
if not exist %vc% (
  echo devenv.exe路径错误:%vc%
  goto done
)
set basedir="%~dp0"
copy /y nul %basedir%\log.txt
echo vc_version = vc9>>%basedir%\log.txt

set sln="%basedir:~1,-1%YBYL.sln"
echo 正在编译 %sln%...
rem %vc% %sln% /Rebuild Release
if not %errorlevel%==0 (
	echo rebuild %sln% failed
	goto done
)
cd /d "%basedir:~1,-1%\Release"
for /r %%i in (*.exe *.dll) do (
   call :printversion "%%i" %%~nxi
)

for %%b in (DsSetUpHelper,GreenShield,GsFilterHelp,GsNetFilter,GSPre,GsSvc) do (
	md !pdbdir!\%%b_cod
	copy /y "%basedir:~1,-1%%%b\Release\*.cod" !pdbdir!\%%b_cod\
)
for %%i in (.dll,.map,.exe,.pdb) do (
	copy /y "%basedir:~1,-1%\Release\*%%i" !pdbdir!
)
for %%i in (.dll,.exe) do (
	copy /y "%basedir:~1,-1%\Release\*%%i" !unsigdir!
)

echo 编译完成
(
  echo @echo off
  echo cd /d %%~dp0
  echo for /r %%%%i in ^(*.dll *.exe^) do ^(
  echo if %%%%~ni == DsSetUpHelper ^(
  echo copy /y %%%%i "..\..\..\..\..\bin\setup\bin\"
  echo ^) else ^(
  echo copy /y %%%%i "..\..\..\..\..\bin\setup\input_main\program\"
  echo ^)
  echo ^)
  echo pause
  echo @echo on
)> !unsigdir!\copyfile.bat
goto done
echo 开始打包...
"C:\Program Files (x86)\NSIS\makensisw.exe" "%basedir:~1,-1%..\..\bin\setup\pack_ansi.nsi"
start explorer.exe "%basedir:~1,-1%..\..\bin\setup\bin"
goto done

:printversion
set build_date=%date:~0,10%
set build_date=%build_date:/=-%
set build_time=%time:~0,8%
set build_time=%build_time::=-%
set bin_path=%1
set bin_path=%bin_path:\=\\%
set bin_path=%bin_path:~1,-1%
echo printversion bin_path=%bin_path%
for /f "skip=1" %%i in ('wmic datafile where "Name='%bin_path%'" get Version') do (
	if "%bin_path:~-15,1000%" == "GreenShield.exe" (
		set pdbdir=%basedir:~1,-1%\%%i\pdb__!build_date!__!build_time!__!random!\
		echo begin make pdbdir= !pdbdir!
		md !pdbdir!
		if errorlevel 0 ( echo made pdbdir success,pdbdir=!pdbdir!)
		set unsigdir=!pdbdir!binary_unsigned%%i
		md !unsigdir!
		echo about create commit_setup_log.txt
		copy /y nul !pdbdir!\commit_setup_log.txt
	)
	echo %2=%%i>>!pdbdir!\commit_setup_log.txt
	echo %2=%%i>>%basedir%\log.txt
	rem copy /y %1 ..\..\..\bin\setup\input_main\program\
	goto :eof
)

:done
@echo on