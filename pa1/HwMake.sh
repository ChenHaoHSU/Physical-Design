NAME=b03505028-p1
TNAME=$NAME.tar
FILES="main.cpp Net.h Net.cpp myHashSet.h readme.txt report.doc report.pdf fm Makefile"

rm -f $TNAME
rm -r -f $NAME

mkdir $NAME

make clean
make

for f in $FILES
do 
   cp $f $NAME
done

make clean 

tar zcvf $TNAME $NAME

#rm -r -f $NAME
