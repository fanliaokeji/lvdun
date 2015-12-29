@echo off
cd /d "%~dp0"
regsvr32 /U ExplorerPlugin.dll
@echo on