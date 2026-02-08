CC=gcc
SRC="minsh.c strbuilder.c"
CFLAGS="-Wall -Wextra -Werror"
OUT="minsh"

$CC -o $OUT $SRC $CFLAGS
