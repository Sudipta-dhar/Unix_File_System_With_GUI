# Unix_File_System_With_GUI

This project is a simulation of a Unix-style file system implemented in C++. It allows users to interact with a virtual file system, performing various operations such as creating, deleting, and listing files and directories, changing directories, copying files, displaying storage usage, and displaying file contents.

## Features
-**It Has A loding System**. which make the program more realistically.
- **Create and Delete Files**: Create new files with specified sizes and delete existing files.
- **Create and Delete Directories**: Create new directories and delete existing directories.
- **Change Directory**: Navigate through the file system by changing the current working directory.
- **List Directory Contents**: List all files and directories within the current directory.
- **Copy Files**: Copy files from one location to another within the file system.
- **Display Storage Usage**: Show the total, used, and free blocks in the file system.
- **Display File Contents**: Display the contents of a specified file.
- **Persistent Storage**: Save the state of the file system to disk and load it on startup.

# Work on Linux and Windows Both platforms. But If you want to run it in Windows yo need json file . The Json file given below...
## Getting Started

### Prerequisites

To compile and run this program, you need:

- A C++ compiler (e.g., GCC, MinGW, Microsoft C++ Build Tools)
- Visual Studio Code (VS Code) with the C++ extension

### Installing

1. **Install a C++ Compiler:**

   - **MinGW (for Windows):**
     - Download MinGW from [MinGW website](http://www.mingw.org/).
     - Install it and add `C:\MinGW\bin` to your system's PATH environment variable.
   - **Microsoft C++ Build Tools:**
     - Install the [Microsoft C++ Build Tools](https://visualstudio.microsoft.com/visual-cpp-build-tools/).

2. **Install Visual Studio Code:**
   - Download and install VS Code from [VS Code website](https://code.visualstudio.com/).

3. **Install C++ Extensions in VS Code:**
   - Open VS Code.
   - Go to the Extensions view by clicking on the Extensions icon in the Activity Bar or pressing `Ctrl+Shift+X`.
   - Search for and install the `C/C++` extension by Microsoft.

### Setting Up the Project

1. **Clone or Download the Project:**
   - Clone the repository or download the project files to your local machine.

2. **Configure VS Code for C++ Development:**
   - Create a `tasks.json` file in the `.vscode` directory of your project:
     ```json
     {
         "version": "2.0.0",
         "tasks": [
             {
                 "label": "build",
                 "type": "shell",
                 "command": "g++",
                 "args": [
                     "-g",
                     "${file}",
                     "-o",
                     "${fileDirname}/${fileBasenameNoExtension}.exe"
                 ],
                 "group": {
                     "kind": "build",
                     "isDefault": true
                 },
                 "problemMatcher": ["$gcc"],
                 "detail": "Generated task by Debugger."
             }
         ]
     }
     ```

   - Create a `launch.json` file in the `.vscode` directory of your project:
     ```json
     {
         "version": "0.2.0",
         "configurations": [
             {
                 "name": "g++ - Build and debug active file",
                 "type": "cppdbg",
                 "request": "launch",
                 "program": "${fileDirname}/${fileBasenameNoExtension}.exe",
                 "args": [],
                 "stopAtEntry": false,
                 "cwd": "${fileDirname}",
                 "environment": [],
                 "externalConsole": false,
                 "MIMode": "gdb",
                 "setupCommands": [
                     {
                         "description": "Enable pretty-printing for gdb",
                         "text": "-enable-pretty-printing",
                         "ignoreFailures": true
                     }
                 ],
                 "preLaunchTask": "build",
                 "miDebuggerPath": "gdb",
                 "setupCommands": [
                     {
                         "description": "Enable pretty-printing for gdb",
                         "text": "-enable-pretty-printing",
                         "ignoreFailures": true
                     }
                 ],
                 "internalConsoleOptions": "openOnSessionStart"
             }
         ]
     }
     ```

### Running the Program

1. **Build the Project:**
   - Open the project folder in VS Code.
   - Open the main C++ file.
   - Press `Ctrl+Shift+B` to build the project. If there are no errors, it will create an executable file.

2. **Run the Program:**
   - Press `F5` to start debugging and run the program.

### Usage

The program provides a command-line interface for interacting with the file system. Here are the available commands:

- `createFile <path> <size>`: Create a new file at the specified path with the given size.
- `deleteFile <path>`: Delete the file at the specified path.
- `createDir <path>`: Create a new directory at the specified path.
- `deleteDir <path>`: Delete the directory at the specified path.
- `changeDir <path>`: Change the current directory to the specified path.
- `listDir`: List the contents of the current directory.
- `cp <source> <destination>`: Copy a file from the source path to the destination path.
- `sum`: Display storage usage information.
- `cat <path>`: Display the contents of the file at the specified path.
- `help`: Display a list of available commands.
- `exit`: Exit the program.

### Example

Here is an example interaction with the file system:
FileSystem> createDir /mydir
Directory created: /mydir

FileSystem> changeDir /mydir
Current directory changed to: /mydir

FileSystem> createFile myfile 2048
File created: /mydir/myfile

FileSystem> listDir
Contents of directory: /mydir
[F] myfile - Size: 2048 bytes - Created: [creation time]

FileSystem> cat myfile
Contents of /mydir/myfile:
[content of the file, which may be random if generated by createFile]

FileSystem> cp myfile myfile_copy
File copied from /mydir/myfile to /mydir/myfile_copy

FileSystem> listDir
Contents of directory: /mydir
[F] myfile - Size: 2048 bytes - Created: [creation time]
[F] myfile_copy - Size: 2048 bytes - Created: [creation time]

FileSystem> sum
Total Blocks: 16384
Used Blocks: [number of used blocks]
Free Blocks: [number of free blocks]

FileSystem> deleteFile myfile
File deleted: /mydir/myfile

FileSystem> listDir
Contents of directory: /mydir
[F] myfile_copy - Size: 2048 bytes - Created: [creation time]

FileSystem> exit
Shutting down the file system...
File system saved.

### Authors

- Sudia Sotra Dhar -
- Collaborator's Walid, Ibrahim, Subodh.

### License

This project is licensed under the MIT License.
