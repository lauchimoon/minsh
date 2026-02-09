CC=gcc
SRC="minsh.c strbuilder.c"
CFLAGS="-Wall -Wextra -Werror"
LDLIBS="-lreadline"
OUT="minsh"

$CC -o $OUT $SRC $CFLAGS $LDLIBS
