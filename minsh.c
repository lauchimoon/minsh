#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("minsh: Missing program and arguments\n");
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("minsh");
        return 1;
    } else if (pid == 0) {
        if (execvp(argv[1], argv + 1) < 0) {
            perror("minsh");
            return 1;
        }
    } else {
        int status;
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 0;
}
