#include "popen2.h"

pid_t
popen2(const char *command, int *infp, int *outfp)
{
    int p_stdin[2], p_stdout[2];
    pid_t pid;

    if (pipe(p_stdin) != 0 || pipe(p_stdout) != 0)
        return -1;

    pid = fork();

    if (pid < 0)
        return pid;
    else if (pid == 0)
    {
        close(p_stdin[WRITE]);
        dup2(p_stdin[READ], READ);
        close(p_stdout[READ]);
        dup2(p_stdout[WRITE], WRITE);

        execl("/bin/sh", "sh", "-c", command, NULL);
	return (pid_t)NULL;
    }

    if (infp == NULL)
        close(p_stdin[WRITE]);
    else
        *infp = p_stdin[WRITE];
	// The way it was p_stdin[read] in this program is still open
    if (outfp == NULL)
        close(p_stdout[READ]);
    else
        *outfp = p_stdout[READ];
	// as well as p_stdout[write], they're closed in the fork, but not in the original program
	//fix is ez:
	close(p_stdin[READ]); // We only write to the forks input anyway
	close(p_stdout[WRITE]); // and we only read from its output
    return pid;
}

