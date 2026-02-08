#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

char *read_string(void);
char **split(char *str, int *ntok);

int main(int argc, char **argv)
{
    char *prompt = read_string();
    int n_args = 0;
    char **args = split(prompt, &n_args);

    pid_t pid = fork();
    if (pid < 0) {
        perror("minsh");
        return 1;
    } else if (pid == 0) {
        if (execvp(args[0], args) < 0) {
            perror("minsh");
            return 1;
        }
    } else {
        int status;
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    free(args);
    free(prompt);
    return 0;
}

char *read_string(void)
{
    int c;
    int pos = 0;
    int size = BUFFER_SIZE;
    char *buffer = malloc(sizeof(char)*size);

    while (1) {
        c = getchar();
        if (c == EOF)
            exit(0);
        else if (c == '\n') {
            buffer[pos] = '\0';
            return buffer;
        } else {
            if (pos >= size/2) {
                size *= 2;
                buffer = realloc(buffer, sizeof(char)*size);
            }
            buffer[pos++] = c;
        }
    }
}

char **split(char *str, int *ntok)
{
    int pos = 0;
    int size = BUFFER_SIZE;
    char *dup_str = strdup(str);
    char **tokens = malloc(sizeof(char *)*size);
    char *tok = strtok(str, " ");
    while (tok) {
        tokens[pos++] = tok;
        tok = strtok(NULL, " ");
    }
    *ntok = pos;
    return tokens;
}
