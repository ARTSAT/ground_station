# Macros
PRG	= despatch_tracker
OBJ	= main.o stochastic.o planet.o spacecraft.o gstation.o despatch.o tracker.o

INCLUDE	= -I eigen-eigen-6b38706d90a9

CFLAGS	= -g -Wall
LFLAGS	= -lm

CC	= g++ $(CFLAGS)

# Primary target
$(PRG) : $(OBJ)
	$(CC) -o $@ $^ $(INCLUDE) $(LFLAGS)

# Suffix rule
.cpp.o :
	$(CC) -c $< $(INCLUDE)

# Target for cleaning files
clean :
	rm -f $(PRG) $(OBJ)

# Dependencies
main.o: main.cpp tracker.h
stochastic.o: stochastic.cpp stochastic.h
planet.o: planet.cpp planet.h
spacecraft.o: spacecraft.cpp spacecraft.h planet.h stochastic.h
gstation.o: gstation.cpp gstation.h planet.h spacecraft.h stochastic.h
despatch.o: despatch.cpp despatch.h spacecraft.h
tracker.o: tracker.cpp tracker.h gstation.h planet.h spacecraft.h despatch.h stochastic.h