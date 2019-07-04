NAME=b03505028-p2
TNAME=$NAME.tar

rm -f $TNAME
rm -r -f $NAME

mkdir $NAME

make clean
make

cp *.cpp $NAME
cp *.h $NAME
cp Makefile $NAME
cp Floorplan.exe $NAME
cp readme.txt $NAME
cp report.doc $NAME
cp report.pdf $NAME

tar zcvf $TNAME $NAME

make clean

#rm -r -f $NAME
