CXX = g++ -Wall -Wextra -O3 -std=c++11

MAIN = src/main

all: obj/main.o LINK 

#obj/lodepng.o: src/lodepng.* 
#	$(CXX) -c -o obj/lodepng.o src/lodepng.cpp

#obj/post.o: src/postprocessing.*
#	$(CXX) -c -o obj/post.o src/postprocessing.cpp

#obj/ray.o: src/ray.*
#	$(CXX) -c -o obj/ray.o src/ray.cpp

#obj/raytracer.o: obj/ray.o src/raytracer.*
#	$(CXX) -c -o obj/raytracer.o src/raytracer.cpp

obj/main.o: src/main.cpp 
	$(CXX) -c -o obj/main.o src/main.cpp

LINK: obj/main.o 
	$(CXX) obj/main.o -o bin/setvoronoi



%o: %.cpp 
	$(CXX) -o $*.o -c $<

clean:
	rm obj/*
	rm bin/*

