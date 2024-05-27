cmake -S . -B build/
cmake --build build/
start build/Debug/Compiler.exe %1

timeout /t 2  /nobreak  >nul

nasm -fwin64 .\out.asm
ld .\out.o -l kernel32 -o out.exe
.\out.exe
echo %ERRORLEVEL%