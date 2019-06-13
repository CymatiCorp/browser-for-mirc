@echo off

SET MCHEF_DIR=%~dp1
SET DLL_MCHEF="%~dp1Release\mChef.dll"
SET DLL_CEF="%~dp1Release\libcef.dll"
SET DLL_ICU="%~dp1Release\icudt42.dll"
SET DEP_DIR=%~dp1Deployment

echo -----------------------------------------------------------------
echo mChef Deployment Tool
echo.
echo   Deployment Directory: %DEP_DIR%
echo.
echo     Checking %DLL_MCHEF%
if not exist %DLL_MCHEF% goto MissingFile
echo     Checking %DLL_CEF%
if not exist %DLL_CEF% goto MissingFile
echo     Checking %DLL_ICU%
if not exist %DLL_ICU% goto MissingFile
echo     Checking %DEP_DIR%
if not exist "%DEP_DIR%" mkdir "%DEP_DIR%"
echo.
echo  Copying Files...
echo      %DLL_MCHEF%
copy /B /Y %DLL_MCHEF% %DEP_DIR%
echo      %DLL_CEF%
copy /B /Y %DLL_CEF% %DEP_DIR%
echo      %DLL_ICU%
copy /B /Y %DLL_ICU% %DEP_DIR%
echo      %MCHEF_DIR%License.txt%
copy /A /Y "%MCHEF_DIR%License.txt" "%DEP_DIR%"
echo      %MCHEF_DIR%Credits.txt%
copy /A /Y "%MCHEF_DIR%Credits.txt" "%DEP_DIR%"
echo      %MCHEF_DIR%\docs
mkdir "%DEP_DIR%\docs"
xcopy "%MCHEF_DIR%docs" "%DEP_DIR%\docs" /Y /E
echo  All files have been copied over to "%DEP_DIR%"
echo.
echo Compressing icudt42.dll...
"%MCHEF_DIR%src\Tools\upx.exe" --ultra-brute "%DEP_DIR%\icudt42.dll"
echo Compressing mChef.dll...
"%MCHEF_DIR%src\Tools\upx.exe" --ultra-brute "%DEP_DIR%\mChef.dll"
echo .
goto end

:MissingFile
echo Error: The file is missing. Did Build mChef under the Release configuration?

:end
echo.
echo Done.
echo -----------------------------------------------------------------