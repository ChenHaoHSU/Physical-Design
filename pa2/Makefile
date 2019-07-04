CXX = g++
EXE = Floorplan.exe

CFLAGS = -g -Wall
CFLAGS = -O3 -Wall

OBJS = main.o parser.o floorplan.o result.o utility.o \
       contourline.o btree.o

HEADERS = floorplanner.h block.h net.h terminal.h btree.h \
          myHashSet.h mydefine.h recovermsg.h contourline.h \
          gnuplot_i.h

$(EXE): $(OBJS) 
	@echo ">> Building $(EXE) ..."
	@$(CXX) -o $@ $(OBJS) $(CFLAGS)

%.o: %.cpp $(HEADERS)
	@echo "> Compiling $< ..."
	@$(CXX) $< -c $(CFLAGS)

clean: 
	@echo "> Removing $(EXE) ..."
	@echo "> Removing *.o ..."
	@rm -f $(EXE) *.o *.O *.gch
