#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../src/lox_assert.h"

int main() {
	int outin[2];
	LOX_ASSERT(pipe(outin) == 0);
	pid_t pid = fork();
	bool is_child = pid == 0;
	if (is_child) {
		close(outin[1]);

		LOX_ASSERT(dup2(outin[0], 0) >= 0);
		close(outin[0]);

		char prog[] = "./test/lox";
		char* args[] = {prog, NULL};
		int ret = execvp(prog, args);

		if (ret == -1) {
			dprintf(1, "execvp failed\n");
			return ret;
		}
	} else {
		close(outin[0]);

		char input[] = "1+1\n1+1\n";
		write(outin[1], input, strlen(input));
		close(outin[1]);

		// wait for child process to exit
		int status = 0;
		LOX_ASSERT(waitpid(pid, &status, 0) > 0);
		if (!WIFEXITED(status)) {
			LOX_ASSERT(WIFSIGNALED(status))
			printf("lox didn't terminate normally because it was killed by "
				   "signal: %d\n",
				   WTERMSIG(status));
			return WTERMSIG(status);
		}
		printf("WEXITSTATUS(status) == %d\n", WEXITSTATUS(status));
		LOX_ASSERT(WEXITSTATUS(status) == 0);
	}
	return 0;
}
