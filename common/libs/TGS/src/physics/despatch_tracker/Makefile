# Macros
PRG	= sctracker
OBJ	= main.o tf.o planet.o spacecraft.o geocoord.o tracker.o despatch.o

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
main.o: main.cpp despatch.h
tf.o: tf.cpp tf.h
planet.o: planet.cpp planet.h tf.h
spacecraft.o: spacecraft.cpp spacecraft.h tf.h planet.h
geocoord.o: geocoord.cpp spacecraft.h tf.h planet.h
tracker.o: tracker.cpp tracker.h tf.h planet.h spacecraft.h
despatch.o: despatch.cpp despatch.h tf.h tracker.h