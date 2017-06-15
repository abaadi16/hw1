# cs335 lab1
# to compile your project, type make and press enter

all: hw1

hw1: hw1.cpp mParticle.cpp
	g++ hw1.cpp mParticle.cpp libggfonts.a str.h fonts.h -Wall -ohw1 -lX11 -lGL -lGLU -lm

clean:
	rm -f hw1
	rm -f *.o

