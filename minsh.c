#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <linux/limits.h>
#include <signal.h>
#include <errno.h>

#include "strbuilder.h"

#define NUM_BUILTIN 3
#define BUFFER_SIZE 1024
#define streq(a, b) (strcmp((a), (b)) == 0)
#define PROGRAM_NAME "minsh"

char *username = NULL;
char *hostname = NULL;
char *home_dir = NULL;
char *builtin_names[NUM_BUILTIN] = {
    "help", "exit", "cd",
};

void handle_ctrl_c(int sig);
void cleanup(void);
void print_ps1(void);
char *read_string(void);
char **split(char *str, int *ntok);
void execute(char **cmd);
bool is_builtin(char **cmd);
void execute_builtin(char **cmd);
char *replace_all(char *s, char c, char *replace);

void builtin_help(void);
void builtin_exit(void);
void builtin_cd(char *dstdir);

int main()
{
    signal(SIGINT, handle_ctrl_c);
    atexit(cleanup);
    username = getenv("USER");
    home_dir = getenv("HOME");
    hostname = malloc(sizeof(char)*PATH_MAX);
    gethostname(hostname, PATH_MAX);
    while (1) {
        print_ps1();
        char *prompt = read_string();
        int n_args = 0;
        char **args = split(prompt, &n_args);
        execute(args);
        free(args);
        free(prompt);
    }
    return 0;
}

void handle_ctrl_c(int sig)
{
    write(STDOUT_FILENO, "\n", 1);
    print_ps1();
    fflush(stdout);

    (void)sig;
}

void cleanup(void)
{
    free(hostname);
}

void print_ps1(void)
{
    char cwd_buffer[PATH_MAX] = { 0 };
    getcwd(cwd_buffer, PATH_MAX);
    printf("[%s@%s %s]$ ", username, hostname, cwd_buffer);
}

char *read_string(void)
{
    int c;
    int pos = 0;
    int size = BUFFER_SIZE;
    char *buffer = malloc(sizeof(char)*size);

    while (1) {
        c = getchar();
        if (c == EOF) {
            printf("\n");
            exit(0);
        }
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
    char **tokens = malloc(sizeof(char *)*size);
    char *tok = strtok(str, " ");
    while (tok) {
        tokens[pos++] = tok;
        tok = strtok(NULL, " ");
    }
    tokens[pos] = NULL;
    *ntok = pos;
    return tokens;
}

void execute(char **cmd)
{
    if (!cmd[0])
        return;

    if (is_builtin(cmd)) {
        execute_builtin(cmd);
        return;
    }
    pid_t pid = fork();
    if (pid < 0) {
        printf("%s: %s\n", PROGRAM_NAME, strerror(errno));
        exit(1);
    } else if (pid == 0) {
        char *name = cmd[0];
        if (execvp(name, cmd) < 0) {
            printf("%s: %s: %s\n", PROGRAM_NAME, name, strerror(errno));
            exit(1);
        }
        exit(1);
    } else {
        int status;
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
}

bool is_builtin(char **cmd)
{
    char *name = cmd[0];
    for (int i = 0; i < NUM_BUILTIN; ++i)
        if (streq(name, builtin_names[i]))
            return true;
    return false;
}

void execute_builtin(char **cmd)
{
    char *name = cmd[0];
    if (streq(name, "help"))
        builtin_help();
    else if (streq(name, "exit"))
        builtin_exit();
    else if (streq(name, "cd")) {
        if (cmd[2]) {
            printf("%s: cd: too many arguments\n", PROGRAM_NAME);
            return;
        }
        char *new_arg = replace_all(cmd[1], '~', home_dir);
        builtin_cd(new_arg);
        free(new_arg);
    }
}

char *replace_all(char *s, char c, char *replace)
{
    if (!s)
        return NULL;

    int c_count = 0;
    for (int i = 0; s[i]; ++i)
        c_count += (s[i] == c);

    StringBuilder *sb = sb_new(strlen(s));

    for (int i = 0; s[i]; ++i) {
        if (s[i] == c)
            sb_write_string(sb, replace);
        else
            sb_write_char(sb, s[i]);
    }
    char *result = calloc(sb->size, sizeof(char));
    memcpy(result, sb->string, sb->size);
    sb_free(sb);
    return result;
}

void builtin_help(void)
{
    printf("%s: simple shell\n", PROGRAM_NAME);
    printf("run commands by typing them into the prompt\n");
}

void builtin_exit(void)
{
    exit(0);
}

void builtin_cd(char *dstdir)
{
    if (!dstdir) {
        chdir(home_dir);
        return;
    }
    if (chdir(dstdir) != 0)
        printf("%s: cd: %s: %s\n", PROGRAM_NAME, dstdir, strerror(errno));
}
