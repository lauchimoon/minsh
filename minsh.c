#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <linux/limits.h>
#include <signal.h>

#define BUFFER_SIZE 1024

char *username = NULL;
char *hostname = NULL;

void handle_ctrl_c(int sig);
void cleanup(void);
void print_ps1(void);
char *read_string(void);
char **split(char *str, int *ntok);
void execute(char **cmd);

int main(int argc, char **argv)
{
    signal(SIGINT, handle_ctrl_c);
    atexit(cleanup);
    username = getenv("USER");
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
    char *dup_str = strdup(str);
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

    pid_t pid = fork();
    if (pid < 0) {
        perror("minsh");
        exit(1);
    } else if (pid == 0) {
        if (execvp(cmd[0], cmd) < 0)
            perror("minsh");
        exit(1);
    } else {
        int status;
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
}
