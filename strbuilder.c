#include "strbuilder.h"
#include <stdlib.h>
#include <string.h>

StringBuilder *sb_new(unsigned long cap)
{
    StringBuilder *sb = malloc(sizeof(StringBuilder));
    sb->cap = cap;
    sb->size = 0;
    sb->string = calloc(sb->cap, sizeof(char));
    return sb;
}

void sb_free(StringBuilder *sb)
{
    free(sb->string);
    free(sb);
}

unsigned long sb_cap(StringBuilder *sb)
{
    return sb->cap;
}

void sb_grow(StringBuilder *sb, unsigned long n)
{
    unsigned long new_cap = sb->cap + n;
    sb->cap = new_cap;
    char *string = realloc(sb->string, sizeof(char)*new_cap);
    memcpy(sb->string, string, new_cap);
}

unsigned long sb_len(StringBuilder *sb)
{
    return sb->size;
}

void sb_reset(StringBuilder *sb)
{
    memset(sb->string, 0, sb->cap);
}

char *sb_string(StringBuilder *sb)
{
    return sb->string;
}

void sb_write_char(StringBuilder *sb, char c)
{
    if (sb->size >= sb->cap/2)
        sb_grow(sb, sb->cap);
    sb->string[sb->size++] = c;
}

void sb_write_string(StringBuilder *sb, char *s)
{
    for (int i = 0; s[i]; ++i)
        sb_write_char(sb, s[i]);
}
