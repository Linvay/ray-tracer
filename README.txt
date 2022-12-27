Advanced Computer Graphics Final Project

TEAM MEMBER:
    40847038S 林廷威
    40847013S 王瑞渝

MAIN PROGRAM: main.exe
PLATFORM: Windows
Program Execution:
    In terminal, run the command "make" to compile all the necessary files
    Then run the command "./main" to start the program
    You will be prompted with the question "Please enter input file name: "
    Enter the file name with P6 data
    The program will generate the result image named "result.ppm"

TEST DATA:
    Input_Suzanne.txt
    Input_Suzanne_Far.txt
    Input_Bunny.txt
    Input_Bunny_Far.txt
    Use these filenames when the program prompts you to input a filename

ABOUT PROGRAM COMPILATION
    In the terminal, run "make" to compile all the obj files and executables
    If you are using Linux OS, please change the following line:
        del main.exe algebra3.o imageIO.o Object.o BVH.o
    to:
        rm main algebra3.o imageIO.o Object.o BVH.o