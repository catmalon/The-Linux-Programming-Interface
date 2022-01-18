# The-Linux-Programming-Interface
![636123118388003750](https://user-images.githubusercontent.com/75157669/143892350-8ef37ac7-d6bd-403b-a523-e30de0f6e507.jpg)

## HW1 / Chapter 4: FILE I/O
4-1
* The tee command reads its standard input until end-of-file, writing a copy of the input to standard output and to the file named in its command-line argument. (We show an example of the use of this command when we discuss FIFOs in Section 44.7.)
Implement tee using I/O system calls. By default, tee overwrites any existing file with the given name. Implement the –a command-line option (tee –a file), which causes tee to append text to the end of a file if it already exists. (Refer to Appendix B for a description of the getopt() function, which can be used to parse command-line options.)

4-2
* Write a program like cp that, when used to copy a regular file that contains holes (sequences of null bytes), also creates corresponding holes in the target file.
## HW2 / My Shell
## HW3 / Chapter 8: USERS AND GROUP
12-1
* Write a program that lists the process ID and command name for all processes being run by the user named in the program’s command-line argument. (You may find the userIdFromName() function from Listing 8-1, on page 159, useful.) This can be done by inspecting the Name: and Uid: lines of all of the /proc/PID/status files on the system. Walking through all of the /proc/PID directories on the system requires the use of readdir(3), which is described in Section 18.8. Make sure your program correctly handles the possibility that a /proc/PID directory disappears between the time that the program determines that the directory exists and the time that it tries to open the corresponding /proc/PID/status file.

12-2
* Write a program that draws a tree showing the hierarchical parent-child relationships of all processes on the system, going all the way back to init. For each process, the program should display the process ID and the command being executed. The output of the program should be similar to that produced by pstree(1), although it does need not to be as sophisticated. The parent of each process on the system can be found by inspecting the PPid: line of all of the /proc/PID/status files on the system. Be careful to handle the possibility that a process’s parent (and thus its /proc/PID directory) disappears during the scan of all /proc/PID directories.

12-3
* Write a program that lists all processes that have a particular file pathname open. This can be achieved by inspecting the contents of all of the /proc/PID/fd/* symbolic links. This will require nested loops employing readdir(3) to scan all /proc/PID directories, and then the contents of all /proc/PID/fd entries within each /proc/PID directory. To read the contents of a /proc/PID/fd/n symbolic link requires the use of readlink(), described in Section 18.5.

## HW4 / My Shell
## HW5 / Chapter 44: PIPES AND FIFOS
44-1
* Write a program that uses two pipes to enable bidirectional communication
between a parent and child process. The parent process should loop reading a
block of text from standard input and use one of the pipes to send the text to the
child, which converts it to uppercase and sends it back to the parent via the other
pipe. The parent reads the data coming back from the child and echoes it on
standard output before continuing around the loop once more.

44-2
* Implement popen() and pclose(). Although these functions are simplified by not
requiring the signal handling employed in the implementation of system()
(Section 27.7), you will need to be careful to correctly bind the pipe ends to file
streams in each process, and to ensure that all unused descriptors referring to the
pipe ends are closed. Since children created by multiple calls to popen() may be
running at one time, you will need to maintain a data structure that associates the
file stream pointers allocated by popen() with the corresponding child process IDs.
(If using an array for this purpose, the value returned by the fileno() function, which
obtains the file descriptor corresponding to a file stream, can be used to index the
array.) Obtaining the correct process ID from this structure will allow pclose() to
select the child upon which to wait. This structure will also assist with the SUSv3
requirement that any still-open file streams created by earlier calls to popen() must
be closed in the new child process.
## HW6 / Chapter 19: MONITORING FILE EVENTS
19-1
* Write a program that logs all file creations, deletions, and renames under the
directory named in its command-line argument. The program should monitor
events in all of the subdirectories under the specified directory. To obtain a list of
all of these subdirectories, you will need to make use of nftw() (Section 18.9). When
a new subdirectory is added under the tree or a directory is deleted, the set of
monitored subdirectories should be updated accordingly.
## HW7 / Chapter 22: SIGNALS
22-3
* Section 22.10 stated that accepting signals using sigwaitinfo() is faster than the use
of a signal handler plus sigsuspend(). The program signals/sig_speed_sigsuspend.c,
supplied in the source code distribution for this book, uses sigsuspend() to
alternately send signals back and forward between a parent and a child process.
Time the operation of this program to exchange one million signals between the
two processes. (The number of signals to exchange is provided as a command-line
argument to the program.) Create a modified version of the program that instead
uses sigwaitinfo(), and time that version. What is the speed difference between the
two programs?

22-4
* Implement the System V functions sigset(), sighold(), sigrelse(), sigignore(), and
sigpause() using the POSIX signal API.
## HW8 / Chapter 44: PIPES AND FIFOS / Chapter 46: SYSTEM V MESSAGE QUEUES
44-6
* The server in Listing 44-7 (fifo_seqnum_server.c) assumes that the client process is
well behaved. If a misbehaving client created a client FIFO and sent a request to the
server, but did not open its FIFO, then the server’s attempt to open the client FIFO
would block, and other client’s requests would be indefinitely delayed. (If done
maliciously, this would constitute a denial-of-service attack.) Devise a scheme to deal
with this problem. Extend the server (and possibly the client in Listing 44-8)
accordingly.

44-7
* Write programs to verify the operation of nonblocking opens and nonblocking I/O
on FIFOs (see Section 44.9).

46-2
* Recode the sequence-number client-server application of Section 44.8 to use System V
message queues. Use a single message queue to transmit messages from both client
to server and server to client. Employ the conventions for message types described
in Section 46.8.
## HW9 / Chapter 49: MEMORY MAPPINGS / Chapter 52: POSIX MESSAGE QUEUES / Chapter 54: POSIX SHARED MEMORY
49-1
* Write a program, analogous to cp(1), that uses mmap() and memcpy() calls (instead
of read() or write()) to copy a source file to a destination file. (Use fstat() to obtain
the size of the input file, which can then be used to size the required memory
mappings, and use ftruncate() to set the size of the output file.)

52-3
* Rewrite the file-server application of Section 46.8 to use POSIX message queues
instead of System V message queues.

54-1
* Rewrite the programs in Listing 48-2 (svshm_xfr_writer.c) and Listing 48-3
(svshm_xfr_reader.c) to use POSIX shared memory objects instead of System V
shared memory.
## HW10 / Producer & Consumer
## HW11 / Chapter 56 ~ 61: SOCKETS
## HW12 / DRIVERS
## FINAL / Chapter 4: FILE I/O / Chapter 23: TIMERS AND SLEEPING / Chapter 53: POSIX SEMAPHORES
4-2
* Write a program like cp that, when used to copy a regular file that contains holes
(sequences of null bytes), also creates corresponding holes in the target file.

23-1
* Although alarm() is implemented as a system call within the Linux kernel, this is
redundant. Implement alarm() using setitimer().

53-1
* Rewrite the programs in Listing 48-2 and Listing 48-3 (Section 48.4) as a threaded
application, with the two threads passing data to each other via a global buffer, and
using POSIX semaphores for synchronization.
