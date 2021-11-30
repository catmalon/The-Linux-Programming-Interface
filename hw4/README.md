# Part 1:
#### 1. Write, compile, and run a program named hostinfo that prints out system information in the following format.
* [Usage] ./hostinfo

![image](https://user-images.githubusercontent.com/75157669/143988541-109f29bf-0580-4ae1-b1aa-64c5eda95f12.png)

#### 2. Write, compile, and run a program named mydate that prints out the day and time in the following format.
* [Usage] ./mydate

![image](https://user-images.githubusercontent.com/75157669/143988607-0a1b533f-523d-47bc-8eb7-61ebe5317eb8.png)

#### 3. Write a program called printdir that prints the current directory. Determine what size buffer to pass to getcwd() for dynamic allocation.(Do not use pwd().)
* [Usage] ./printdir

![image](https://user-images.githubusercontent.com/75157669/143988685-686531fb-0817-4bb8-9974-45126abadaff.png)

#### 4. Write a program called mycat that is a simple version of the program cat. The program takes exactly one file name as argument; you should open it for reading and display its contents to the screen. Check that there is exactly one argument (argc == 2) and if not, display the usage message ”Usage: mycat filename”.
* [Usage] ./mycat FILENAME

![image](https://user-images.githubusercontent.com/75157669/143988863-5deef8f7-69f3-4a69-a94c-5e53b40c9323.png)

#### 5. Write pipe_ls to practice using pipe() and dup(). Have your process start ls (using fork() and exec()) but read the output from ls over a pipe. The ls program writes output on descriptor 1, so some work has to be done to get the pipe connected. Write what you read on the pipe to stdout.
* [Usage] ./pipe_ls

![image](https://user-images.githubusercontent.com/75157669/143988993-edf01859-c5f8-4f8a-8e43-467861fe6c15.png)

# Part 2:
#### 1. Edit the builtin.c file to recognize cd, pwd, id, hostname and builtin. Write functions implementing these commands, and compile then into your shell. The builtin command lists the functions built into your shell.

![image](https://user-images.githubusercontent.com/75157669/143989233-a32a3e2d-9d0b-4138-b3d4-0c85acfd3cf6.png)

#### 2. Modify the redirect_in.c file to recognize standard input and redirect_out.c file to recognize standard output. Add code to the pipe_present.c file to check for the pipe symbol. Add code to the pipe_command.c file to create a process to execute each of the pipe. Modify is_background.c to check the “&” symbol. Alter the run_command.c file to call these functions.

![image](https://user-images.githubusercontent.com/75157669/143990163-eb608b61-dcd3-424e-95cd-65cef306067d.png)

# Part 3:
#### 1. The first look up project lab familiarizes you with the format of the dictionary by completing the convert.c program that creates the dictionary of fixed-length records (fixrec) from a file of variable-length entries (dict). Add code to convert.c to change an editable file into a fixed-length record format.
![convert](https://user-images.githubusercontent.com/75157669/143990221-394a032c-8689-4564-bc78-8391b8e474f8.png)

#### 2. Add code to the lookup1.c file to do a simple linear search through a file of fixed length records. Link with main.c, the user interface module.
![file_lookup](https://user-images.githubusercontent.com/75157669/143990234-b62ba4a9-1e6e-4ab7-98a1-e8690c813c39.png)

