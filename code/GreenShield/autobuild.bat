@echo off
setlocal EnableDelayedExpansion
set vc="C:\Program Files (x86)\Microsoft Visual Studio 9.0\Common7\IDE\devenv.exe"
if not exist %vc% (
  echo file not exist:%vc%
  goto done
)
set basedir="%~dp0"
copy /y nul %basedir%\log.txt
echo vc_version = vc9>>%basedir%\log.txt

set build_date=%date:~0,10%
set build_date=%build_date:/=-%
set build_time=%time:~0,8%
set build_time=%build_time::=-%
set pdbdir=%basedir%\pdb__%build_date%__%build_time%__%random%
md %pdbdir%
echo about create commit_setup_log.txt
copy /y nul %pdbdir%\commit_setup_log.txt
set sln="%basedir:~1,-1%GreenShield.sln"
set outdir="%basedir:~1,-1%Release"
echo rebuilding %sln%
%vc% %sln% /Rebuild Release
if not %errorlevel%==0 (
	echo rebuild %sln% failed
	goto done
)
cd /d %outdir%
for /r %%i in (*.dll *.exe) do (
   call :printversion "%%i" %%~nxi
)
cd /d %basedir%
for %%i in (.dll,.map,.exe,pdb) do (
	copy /y %outdir%\*%%i %pdbdir%
)
goto done

:printversion
set bin_path=%1
set bin_path=%bin_path:\=\\%
set bin_path=%bin_path:~1,-1%
echo %bin_path%
for /f "skip=1 tokens=1 delims=+" %%i in ('wmic datafile where "Name='%bin_path%'" get Version') do (
	set bin_version=%%i 
	::set bin_version=%bin_version:, =.%
	echo %2=%bin_version%>>%pdbdir%\commit_setup_log.txt
	echo %2=%bin_version%>>%basedir%\log.txt
	goto :eof
)


:done
pause
@echo on