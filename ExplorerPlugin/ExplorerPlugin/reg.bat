@echo off
cd /d "%~dp0"
regsvr32 ExplorerPlugin.dll
@echo on