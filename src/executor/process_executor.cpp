#include "process_executor.h"
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <array>
#include <cstring>
#include <cerrno>

ProcessResult ProcessExecutor::execute(const std::vector<std::string>& command) {
    ProcessResult result;
    result.success = false;
    result.exit_code = -1;

    if (command.empty()) {
        result.output = "Error: Empty command";
        return result;
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        result.output = "Error: Failed to create pipe: " + std::string(strerror(errno));
        return result;
    }

    pid_t pid = fork();

    if (pid == -1) {
        // Fork failed
        result.output = "Error: Failed to fork process: " + std::string(strerror(errno));
        close(pipefd[0]);
        close(pipefd[1]);
        return result;
    } else if (pid == 0) {
        // Child process
        close(pipefd[0]); // Close unused read end

        // Redirect stdout and stderr to the pipe
        dup2(pipefd[1], STDOUT_FILENO);
        dup2(pipefd[1], STDERR_FILENO);
        close(pipefd[1]);

        // Prepare arguments for execvp
        std::vector<char*> args;
        for (const auto& arg : command) {
            args.push_back(const_cast<char*>(arg.c_str()));
        }
        args.push_back(nullptr);

        execvp(args[0], args.data());

        // If execvp returns, it must have failed
        std::cerr << "Error: execvp failed for " << args[0] << ": " << strerror(errno) << "\n";
        _exit(127);
    } else {
        // Parent process
        close(pipefd[1]); // Close unused write end

        // Read output from the child
        std::array<char, 4096> buffer;
        ssize_t count;
        while ((count = read(pipefd[0], buffer.data(), buffer.size())) > 0) {
            result.output.append(buffer.data(), count);
        }
        close(pipefd[0]);

        // Wait for child to finish
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            result.output += "\nError: waitpid failed: " + std::string(strerror(errno));
            return result;
        }

        if (WIFEXITED(status)) {
            result.exit_code = WEXITSTATUS(status);
            result.success = (result.exit_code == 0);
        } else if (WIFSIGNALED(status)) {
            result.exit_code = 128 + WTERMSIG(status);
            result.output += "\nProcess terminated by signal " + std::to_string(WTERMSIG(status));
        }
    }

    return result;
}
