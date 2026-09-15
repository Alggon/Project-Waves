@echo off

REM Deletes previous files
RD /S /Q "%cd%\.vs"
RD /S /Q "%cd%\Binaries"
RD /S /Q "%cd%\DerivedDataCache"
RD /S /Q "%cd%\Intermediate"
del /f "%cd%\*.sln"
del /f "%cd%\.idea"
del /f "%cd%\.vsconfig"

REM Clean Saved except Config (Delete files)
for %%f in ("%cd%\Saved\*") do (
    if not "%%~nxf"=="Config" (
        del /F /Q "%%~f"
    )
)

REM Clean Saved except Config (Delete folders)
for /d %%d in ("%cd%\Saved\*") do (
    if /I not "%%~nxd"=="Config" (
        rmdir /S /Q "%%~d"
    )
)

REM Find MSBuild
for /f "usebackq delims=" %%i in (
  `"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`
) do set "MSBUILD_PATH=%%i"

REM Setup Unreal Build Tool
set UVS="C:\Program Files (x86)\Epic Games\Launcher\Engine\Binaries\Win64\UnrealVersionSelector.exe"
set UBT="C:\Program Files\Epic Games\UE_5.7\Engine\Binaries\DotNET\UnrealBuildTool.exe"

REM Find .uproject
for %%f in ("%~dp0*.uproject") do (
   set PROJECT=%%f
)

REM Find .sln
for %%f in ("%~dp0*.sln") do (
   set SOLUTION=%%f
)

%UVS% /projectfiles "%PROJECT%"

REM Generate Project Files
%UBT% Development Win64 -Project=%PROJECT% -TargetType=Editor -Progress -NoEngineChanges -NoHotReloadFromIDE

REM Restauration NuGet
"%MSBUILD_PATH%" "%SOLUTION%" /t:Restore /p:Configuration="Development Editor" /p:Platform="Win64" /p:NuGetAudit=false /m

REM Build Solution
"%MSBUILD_PATH%" "%SOLUTION%" /t:Build /p:Configuration="Development Editor" /p:Platform="Win64" /p:NuGetAudit=false /p:WarningLevel=0 /m

pause