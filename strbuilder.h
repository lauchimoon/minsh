#ifndef STRBUILDER_H
#define STRBUILDER_H

typedef struct StringBuilder {
    char *string;
    unsigned long size;
    unsigned long cap;
} StringBuilder;

#define SB_FMT(sb) (sb->string)

StringBuilder *sb_new(unsigned long cap);
void sb_free(StringBuilder *sb);
unsigned long sb_cap(StringBuilder *sb);
void sb_grow(StringBuilder *sb, unsigned long n);
unsigned long sb_len(StringBuilder *sb);
void sb_reset(StringBuilder *sb);
char *sb_string(StringBuilder *sb);
void sb_write_char(StringBuilder *sb, char c);
void sb_write_string(StringBuilder *sb, char *s);

#endif // STRBUILDER_H
