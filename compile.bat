cd new
flex -onew.yy.c new.lex
bison -d new.y
gcc -o new.exe new.tab.c new.yy.c
cd ..
new\new < %1
copyBuildTree
del tmp
