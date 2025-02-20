# Minishell

## Table of Contents

1. [Project Overview](#1-project-overview)
2. [Global Code Flow](#2-global-code-flow)
3. [Code Implementation and Logic](#3-code-implementation-and-logic)
   - [Managing Child Processes](#managing-child-processes)
   - [Inter-Process Communication with Pipes](#inter-process-communication-with-pipes)
   - [Handling Signals and Keyboard Shortcuts](#handling-signals-and-keyboard-shortcuts)
   - [Working with File Descriptors](#working-with-file-descriptors)
   - [Error Handling and Errno](#error-handling-and-errno)
4. [Common Pitfalls & Debugging Techniques](#4-common-pitfalls--debugging-techniques)
5. [Compilation & Execution](#5-compilation--execution)
6. [References](#6-references)

---

## 1. Project Overview

Minishell is a Unix shell implementation that mimics Bash behavior. It includes command execution, redirections, pipelines, and signal handling. The goal is to gain a deeper understanding of system programming in C, managing processes, memory, and file descriptors.

---

## 2. Global Code Flow

#### **1. Startup Phase**
   - **Initialize environment variables** by copying them from the system.
   - **Set up signal handlers** to properly manage `Ctrl+C`, `Ctrl+D`, and `Ctrl+\`.
   - **Display the prompt** using `readline()` and wait for user input.

#### **2. Main Execution Loop**
The core logic of the shell is handled within an infinite loop:
   - **Read user input** using `readline()`.
   - **Add command to history** with `add_history()`.
   - **Parse and tokenize** the input into structured commands.
   - **Expand environment variables** (e.g., `$HOME` → `/home/user`).
   - **Identify and handle built-in commands** (e.g., `cd`, `echo`, `exit`).
   - **Prepare execution**:
     - If the command involves **pipes (`|`)**, handle it right to left.
     - If there are **redirections (`>`, `<`, `>>`, `<<`)**, adjust file descriptors.
     - If it’s an **external command**, fork and execute it using `execve()`.

#### **3. Handling Piped and Redirected Commands**
   - **Pipes (`|`)** are processed from **right to left**, ensuring proper input/output flow.
   - **Redirections (`>`, `<`, `>>`, `<<`)** are processed **before execution**, modifying file descriptors with `dup2()`.
   - **Child processes handle execution** while the parent waits and manages input/output.

#### **4. Process Execution and Management**
   - **Forking**: Create a child process to execute external commands.
   - **Executing Commands**: The child process replaces its image with the command using:
     ```c
     execve(const char *pathname, char *const argv[], char *const envp[]);
     ```
   - **Waiting for Child Process**: The parent process waits for the child to finish using:
     ```c
     pid_t waitpid(pid_t pid, int *wstatus, int options);
     ```

#### **5. Signal Handling**
   - **`Ctrl+C` (`SIGINT`)**: Stops the current foreground process but does not exit the shell.
   - **`Ctrl+D` (`EOF`)**: Exits the shell if entered on an empty line.
   - **`Ctrl+\` (`SIGQUIT`)**: Can be ignored or handled for custom behavior.

#### **6. Memory Management**
   - Free allocated memory after every command to prevent leaks.
   - Close unused file descriptors to avoid resource exhaustion.
   - Properly handle errors to ensure stability.

---

## 3. Code Implementation and Logic

### **Managing Child Processes**

- The shell must **create child processes** to execute external commands using `fork()`:
  ```c
  pid_t fork(void);
  ```
- The parent process should **wait for the child to complete** using `waitpid()`:
  ```c
  pid_t waitpid(pid_t pid, int *wstatus, int options);
  ```
- If `execve()` fails, the child process should terminate gracefully to avoid resource leaks.
- To prevent zombie processes, ensure the parent handles terminated child processes correctly.

### **Inter-Process Communication with Pipes**

- Pipes allow **one process to send output to another process as input**.
- Execution order in pipelines is **right to left**:
  - Example: `cmd1 | cmd2 | cmd3`
    - `cmd3` executes first.
    - `cmd2` takes input from `cmd3` and passes output to `cmd1`.
- A pipe is created using:
  ```c
  int pipe(int pipefd[2]);
  ```
- The standard output of the first process is redirected to the pipe:
  ```c
  dup2(pipefd[1], STDOUT_FILENO);
  ```
- The second process reads from the pipe via `dup2(pipefd[0], STDIN_FILENO);`.
- Always **close unused pipe ends** to avoid resource leaks.

### **Handling Signals and Keyboard Shortcuts**

- **Exit status management (`Ctrl+C`, `Ctrl+D`)**:
- The shell must properly handle:
  - **`SIGINT` (`Ctrl+C`)**: Interrupts the current process but should not terminate the shell.
  - **`SIGQUIT` (`Ctrl+\`)**: Can be ignored or handled differently based on the shell’s design.
  - **`EOF` (`Ctrl+D`)**: Should signal an exit if entered on an empty line.
- Signal handlers are set up using:
  ```c
  struct sigaction sa;
  sa.sa_handler = my_handler;
  sigaction(SIGINT, &sa, NULL);
  ```

### **Working with File Descriptors**

- A file descriptor is an integer representing an open file or process I/O stream.
- The shell must correctly handle:
  - Opening files with `open()`:
    ```c
    int open(const char *pathname, int flags, mode_t mode);
    ```
  - Redirecting standard input/output with `dup2()`:
    ```c
    int dup2(int oldfd, int newfd);
    ```
  - Closing unnecessary file descriptors with `close(fd);`.

### **Error Handling and Errno**

- `errno` is a global variable set by system calls when an error occurs.
- The shell provides **clear error messages** using:
  ```c
  void perror(const char *s);
  ```
  which prints the error message corresponding to `errno`.
- Alternatively, `strerror()` converts `errno` into a readable string:
  ```c
  char *strerror(int errnum);
  ```
  allowing custom error reporting.

---

## 4. Common Pitfalls & Debugging Techniques

- **Zombie processes**:
  - A child process becomes a **zombie** if the parent does not `waitpid()`.
  - Use `ps aux | grep Z` to check for zombies.

- **Pipe mismanagement**:
  - Ensure `dup2()` is called **before** closing pipe file descriptors.
  - Always `close(pipefd[0])` in the writing process and `close(pipefd[1])` in the reading process.

- **Incorrect signal handling**:
  - Reset signal handlers after execution to avoid interference with future commands.

- **Error handling issues**:
  - Always check return values of system calls (`open()`, `dup2()`, `execve()`).
  - Use `strerror(errno)` for **custom error messages** when logging errors.

---

## 5. Compilation & Execution

```sh
make && ./minishell
```

To clean up compiled files:
```sh
make clean
```

To remove all binaries and object files:
```sh
make fclean
```

To recompile from scratch:
```sh
make re
```

---

## 6. References

- [GNU Bash Manual](https://www.gnu.org/software/bash/manual/)
- [Readline Library](https://tiswww.case.edu/php/chet/readline/rltop.html)

