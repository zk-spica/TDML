cd new
flex -onew.yy.c new.lex
bison -d new.y
gcc -o new.exe new.tab.c new.yy.c
cd ..
cd css
flex -ocss.yy.c css.lex
bison -d css.y
gcc -o css.exe css.tab.c css.yy.c
cd ..
new\new < %1
copyBuildTree
del tmp
css\css < %2
copyBuildCSSList
del tmpCSS
cd script
script ..\%3 ..\tmpScript
cd ..
copyScript
del tmpScript



