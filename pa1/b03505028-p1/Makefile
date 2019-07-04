CFLAGS = -g -Wall
#CFLAGS = -O2 -Wall
CFLAGS = -O3 -Wall

fm: main.o Net.o 
	@echo ">> Building fm ..."
	@g++ -o $@ main.o Net.o $(CFLAGS)

main.o: main.cpp Net.h
	@echo "> Compiling main.cpp ..."
	@g++ -c main.cpp $(CFLAGS)

Net.o: Net.h Net.cpp myHashSet.h
	@echo "> Compiling Net.cpp ..."
	@g++ -c Net.cpp $(CFLAGS)  

t: testGen.cpp
	@echo "> Compiling testGen.cpp ..."
	@g++ -o testGen testGen.cpp $(CFLAGS)

clean: 
	@echo "> Removing fm ..."
	@echo "> Removing *.o *.O ..."
	@rm -f fm *.o *.O
