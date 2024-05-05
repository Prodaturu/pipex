<h1 align="center">
	📖 Pipex - 42
</h1>

<p align="center">
	<b><i>Reproduction of shell pipes</i></b><br>
</p>

<p align="center">
	<img alt="GitHub code size in bytes" src="https://img.shields.io/github/languages/code-size/Prodaturu/pipex?color=lightblue" />
	<img alt="Number of lines of code" src="https://img.shields.io/tokei/lines/github/Prodaturu/pipex?color=critical" />
	<img alt="GitHub top language" src="https://img.shields.io/github/languages/top/Prodaturu/pipex?color=blue" />
	<img alt="GitHub last commit" src="https://img.shields.io/github/last-commit/Prodaturu/pipex?color=green" />
</p>


Pipex is a project in C that re-creates the way two commands are piped together via `|` in the shell

````
# ./pipex infile cmd1 cmd2 outfile
pipe()
 |
 |-- fork()
      |
      |-- child // cmd1
      :     |--dup2()
      :     |--close end[0]
      :     |--execve(cmd1)
      :
      |-- parent // cmd2
            |--dup2()
            |--close end[1]
            |--execve(cmd2)
 
# pipe() sends the output of the first execve() as input to the second execve()
# fork() runs two processes (i.e. two commands) in one single program
# dup2() swaps our files with stdin and stdout
 ````
## Setting the pipe 📏

````
void    pipex(int f1, int f2)
{
    int end[2];    pipe(end);
}

# pipe() takes an array of two int, and links them together
# what is done in end[0] is visible to end[1], and vice versa
# pipe() assigns an fd to each end
# Fds are file descriptors
# since files can be read and written to, by getting an fd each, the two ends can communicate
# end[1] will write to the its own fd, and end[0] will read end[1]’s fd and write to its own

````
## Forking the processes ⑃

````
void    pipex(int f1, int f2)
{
    int   end[2];
    pid_t parent;    pipe(end);
    parent = fork();
    if (parent < 0)
         return (perror("Fork: "));
    if (!parent) // if fork() returns 0, we are in the child process
        child_process(f1, cmd1);
    else
        parent_process(f2, cmd2);
}

# fork() splits the process in two sub-processes -> parallel, simultaneous, happen at the same time
# it returns 0 for the child process, a non-zero for the parent process, -1 in case of error
````
end[1] is the child process, end[0] the parent process; the child writes, the parent reads  
Since for something to be read, it must be written first, so cmd1 will be executed by the child, and cmd2 by the parent.  

## FDs
Pipex is run like this ./pipex infile cmd1 cmd2 outfile  
FDs 0, 1, and 2 are by default assigned to stdin, stdout, and stderr  
`infile`, `outfile`, the pipe, the `stdin` and `stdout` are all FDs  
On Linux, you can check your fds currently open with the command ls -la /proc/$$/fd  

Our fd table right now looks like this:
````
                           -----------------    
                 0         |     stdin     |  
                           -----------------    
                 1         |     stdout    |    
                           -----------------    
                 2         |     stderr    |  
                           -----------------
                 3         |     infile    |  // open()
                           -----------------
                 4         |     outfile   |  // open()
                           -----------------
                 5         |     end[0]    | 
                           -----------------
                 6         |     end[1]    |  
                           -----------------
````
## Swapping fds with dup2() ⩕

For the child process, we want infile to be our stdin (as input), and end[1] to be our stdout (we write to end[1] the output of cmd1)  
In the parent process, we want end[0] to be our stdin (end[0] reads from end[1] the output of cmd1), and outfile to be our stdout (we write to it the output of cmd2)  
Visually,
````
// Each cmd needs a stdin (input) and returns an output (to stdout)
   
    infile                                             outfile
as stdin for cmd1                                 as stdout for cmd2            
       |                        PIPE                        ↑
       |           |---------------------------|            |
       ↓             |                       |              |
      cmd1   -->    end[1]       ↔       end[0]   -->     cmd2           
                     |                       |
            cmd1   |---------------------------|  end[0]
           output                             reads end[1]
         is written                          and sends cmd1
          to end[1]                          output to cmd2
       (end[1] becomes                      (end[0] becomes 
        cmd1 stdout)                           cmd2 stdin)

````
We swap fds to stdin/stdout with dup2()  
From the MAN, 
````
int dup2(int fd1, int fd2) : it will close fd2 and duplicate the value of fd2 to fd1
else said, it will redirect fd1 to fd2
````
In pseudo-code:
````
# child_process(f1, cmd1); // add protection if dup2() < 0
// dup2 close stdin, f1 becomes the new stdin
dup2(f1, STDIN_FILENO); // we want f1 to be execve() input
dup2(end[1], STDOUT_FILENO); // we want end[1] to be execve() stdout
close(end[0]) # --> Always close the end of the pipe that's unused,
                    as long as the pipe is open, the other end will 
                    be waiting for some kind of input and will not
                    be able to finish its process
close(f1)
// execve function for each possible path (see below)
exit(EXIT_FAILURE);
````
Parent process in pseudo code will be similar  
It needs a `waitpid()` at the very beginning to wait for the child to finish her process  
````
# parent_process(f2, cmd2);
int status;
waitpid(-1, &status, 0);
dup2(f2, ...); // f2 is the stdout
dup2(end[0], ...); // end[0] is the stdin
close(end[1])
close(f2);
// execve function for each
