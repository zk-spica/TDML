flex -o%1.yy.c %1.lex
bison -d %1.y
gcc -o %1.exe %1.tab.c %1.yy.c
