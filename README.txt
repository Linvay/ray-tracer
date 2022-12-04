Advanced Computer Graphics Assignment 4

MAIN PROGRAM: main.exe
Program Execution:
    In terminal, run the command "./main"
    You will be prompted with the question "Please enter input file name: "
    Enter the file name with P6 data
    The program will generate the result image named "result.ppm"

PROGRAM COMPILATION:
If you are using Linux OS distros or Mac OS, 
run the command "make" to compile the program

IMPORTANT NOTE:
The program currently only applies 1 layer of bounding volume, in order to speed up the process the complex model
must not take up too large of a portion of the screen, or else the acceleration would be minimal.
This can be done by moving the "E" attribute away from the scene.

If you are using Windows OS, you'll have to install the MinGW Package Manager
and install the package mingw32-make in order to use run "make" command
You can refer to this link for guidance on installation: https://linuxhint.com/run-makefile-windows/