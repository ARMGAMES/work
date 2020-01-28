@rem This batch file must be run in protocols/atfgenerated directory
@echo off

rem Command line options:
rem --verbose - Verbose output

rem Error codes (please refer to 'atfverify help' for more info):
rem     0 - OK
rem     1 - generation failure
rem     2 - verification failure
rem     3 - genprot.pl script not found
rem     10 - invaid usage
rem     20 - unknown error (most likely IO exception)
rem     100 - Perl is not installed

rem Checking if Python/pip installed
pip>nul
if errorlevel 1 (
	echo "pip is not installed, please install Python >= 2.7.13"
	exit /b 100
)

rem Checking if atfverify Python module is installed
pip freeze|findstr /c:"atfverify==1.0">nul
if errorlevel 1 (
	echo Installing atfverify Python module...
	pip install T:\AtfVerify\atfverify-1.0-py2-none-any.whl
)

atfverify generate -l ATFVerify.ini %1 %2 %3