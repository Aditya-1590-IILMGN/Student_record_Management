HOW TO BUILD AND RUN (WINDOWS)

1. Download the SQLite amalgamation zip (sqlite-amalgamation-xxxxxx.zip) from:
   https://sqlite.org/download.html
   Extract sqlite3.h and sqlite3.c into this exact folder (alongside main.cpp).
2. Open Command Prompt (cmd) in this folder and run:

   a. Compile sqlite3.c using the C compiler (gcc)
   gcc -c sqlite3.c -o sqlite3.o

   b. Compile your C++ files and link with sqlite3.o
   g++ -std=c++17 -I. main.cpp Database.cpp InputValidator.cpp StudentManager.cpp sqlite3.o -o StudentManager.exe

   c. Run the executable
   .\StudentManager.exe
Notes:
- Make sure MinGW / g++ is installed and added to your Windows PATH.
- On first run, students.db will be created automatically in this folder.
