rem this batch file must be run in the current directory

@echo off

rmdir /S/Q $$$

mkdir $$$

for /f "usebackq" %%f in (`dir /b *.txt`) do (
	echo =========================== %%f =========================
	copy %%f $$$
	pushd $$$
	perl ..\..\..\atf\genprot.pl -genMoveConstructor %%f
	IF ERRORLEVEL 1 GOTO ErrorHandling
	perl ..\..\..\atf\genprot.pl -genMoveConstructor -thin %%f
	IF ERRORLEVEL 1 GOTO ErrorHandling
	popd
)

echo:
pushd $$$
for /f "usebackq" %%f in (`dir /b *.h *.cpp`) do (
	echo check %%f ...
	fc %%f ..\%%f > nul
	IF ERRORLEVEL 1 copy /Y %%f ..
)
popd

echo:
echo ^<^<^<^<^<^<^<^<^<^<^<^<^<^<^<^<^<^<^<^<^<^<^<^<^<^< All Done ^>^>^>^>^>^>^>^>^>^>^>^>^>^>^>^>^>^>^>^>^>^>^>^>^>^>^>^>
rmdir /S/Q $$$
GOTO END

:ErrorHandling
echo:
echo ************************ error found! exit!!! *************************

:END
echo.



