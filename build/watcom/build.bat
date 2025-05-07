:: Adjust these paths to suit your own environment.
set WATCOM_INSTALL_DIR=C:\WATCOM

:: You need to setup the environment before executing the compiler.
call "%WATCOM_INSTALL_DIR%\owsetenv.bat"

:: Use wcl386 to compile for 32-bit. If you were wanting to do a 16-bit build, you would do "wcl", but
:: since c89atomic does not support 16-bit architectures this is irrelevant for us.
::
:: The "-zq" option is to enable quite builds. Not doing this will result in the compiler printing things
:: like version and copyright information.
wcl386 -zq ../../tests/c89atomic_basic.c