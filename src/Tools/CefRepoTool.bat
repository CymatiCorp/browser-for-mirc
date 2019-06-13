ECHO OFF
cls

SET MCHEF_DIR=E:\dev\mChef
SET CEF_DIR=E:\chromiumtrunk\src\cef

:MENU
echo.
echo _____________________________________________________________________________
echo mChef Hacked Up Menu v1.0
echo.
echo Which configuration of CEF do you want to copy the libs/symbolics from?
echo.
echo 1 - CEF Debug
echo 2 - CEF Release
echo 3 - Copy the "include" directory from CEF
echo 4 - Exit
echo.
echo _____________________________________________________________________________

SET /P M=Type 1, 2, or 3, then press ENTER:
IF %M%==1 GOTO DEBUG
IF %M%==2 GOTO RELEASE
IF %M%==3 GOTO COPYDIR
IF %M%==4 GOTO BUBYE

:DEBUG
copy /B /Y "%CEF_DIR%\Debug\libcef.dll" "%MCHEF_DIR%\src\ThirdParty\cef\build\Debug"
copy /B /Y "%CEF_DIR%\Debug\libcef.pdb" "%MCHEF_DIR%\Debug"
copy /B /Y "%CEF_DIR%\Debug\lib\libcef.lib" "%MCHEF_DIR%\src\ThirdParty\cef\lib\Debug"
copy /B /Y "%CEF_DIR%\Debug\lib\libcef_dll_wrapper.lib" "%MCHEF_DIR%\src\ThirdParty\cef\lib\Debug"
GOTO MENU

:COPYDIR
xcopy "%CEF_DIR%\include" "%MCHEF_DIR%\src\ThirdParty\cef\include" /E /R /Y
GOTO MENU

:RELEASE
copy /B /Y "%CEF_DIR%\Release\libcef.dll" "%MCHEF_DIR%\src\ThirdParty\cef\build\Release"
copy /B /Y "%CEF_DIR%\Release\libcef.pdb" "%MCHEF_DIR%\Release"
copy /B /Y "%CEF_DIR%\Release\lib\libcef.lib" "%MCHEF_DIR%\src\ThirdParty\cef\lib\Release"
copy /B /Y "%CEF_DIR%\Release\lib\libcef_dll_wrapper.lib" "%MCHEF_DIR%\src\ThirdParty\cef\lib\Release"
GOTO MENU

:BUBYE

