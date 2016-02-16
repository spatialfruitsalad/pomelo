CXX = g++ -Wall -Wextra -O3 -std=c++11 -I/usr/include/lua5.2 -ggdb

CXXVORO = g++ -std=c++11 -g -O3

MAIN = src/main

all: obj/main.o LINK 

obj/voro.o: lib/voro++/src/voro++.*
	$(CXXVORO) -c -o obj/voro.o lib/voro++/src/voro++.cc

obj/fileloader.o: src/fileloader.*
	$(CXX) -c -o obj/fileloader.o src/fileloader.cpp

obj/pointpattern.o: src/pointpattern.*
	$(CXX) -c -o obj/pointpattern.o src/pointpattern.cpp
#obj/ray.o: src/ray.*
#	$(CXX) -c -o obj/ray.o src/ray.cpp

#obj/raytracer.o: obj/ray.o src/raytracer.*
#	$(CXX) -c -o obj/raytracer.o src/raytracer.cpp

obj/main.o: src/main.cpp  src/duplicationremover.hpp src/polywriter.hpp
	$(CXX) -c -o obj/main.o src/main.cpp -llua5.2

LINK: obj/main.o obj/voro.o obj/fileloader.o obj/pointpattern.o
	$(CXX) obj/main.o obj/voro.o obj/fileloader.o obj/pointpattern.o -o bin/setvoronoi -llua5.2



%o: %.cpp 
	$(CXX) -o $*.o -c $<

clean:
	rm obj/*
	rm bin/setvoronoi
