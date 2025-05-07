:: This script assumes you are compiling from this directory. The "-I" part needs to come
:: before the source file.
::
:: Borland is very annoying because it doesn't handle relative include paths properly. From
:: what I can tell, it looks like it only considers the current directory and paths explicitly
:: added to the command line via the "-I" switch.
::
:: This script has been built with the assumption that it'll be run from this directory. If
:: you want to compile from another directory you'll need to adjust the "-I" parts.
::
:: The `-I"../../"` switch is to add the root directory to the include search paths. This is
:: required for doing `#include "c89atomic.h"`.
::
:: The `-I"../"` switch is to allow tests/c89atomic_basic.c to do `#include "../c89atomic.c"`.

bcc32 -I"../../" -I"../" ../../tests/c89atomic_basic.c