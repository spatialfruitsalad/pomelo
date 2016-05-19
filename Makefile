CXX = clang++ -Wall -Wextra -O3 -std=c++11 
CXXVORO = clang++ -std=c++11 -g -O3
LUAFLAG = -DUSELUA

all:  LINK_luafree

GENERIC:  LINK 

obj/voro.o: lib/voro++/src/voro++.*
	$(CXXVORO) -c -o obj/voro.o lib/voro++/src/voro++.cc

obj/fileloader.o: src/fileloader.*
	$(CXX) -c -o obj/fileloader.o src/fileloader.cpp

obj/pointpattern.o: src/pointpattern.*
	$(CXX) -c -o obj/pointpattern.o src/pointpattern.cpp

obj/main_luafree.o: src/main.cpp  src/duplicationremover.hpp src/writerpoly.hpp src/IWriter.hpp src/postprocessing.hpp src/parsexyz.hpp src/output.hpp
	mkdir -p obj
	mkdir -p bin
	$(CXX) -c -o obj/main_luafree.o src/main.cpp 

obj/main.o: src/main.cpp  src/duplicationremover.hpp src/writerpoly.hpp src/IWriter.hpp src/postprocessing.hpp src/parsexyz.hpp src/output.hpp
	mkdir -p obj
	mkdir -p bin
	$(CXX) -c -o obj/main.o src/main.cpp -I/usr/include/lua5.2 $(LUAFLAG)

LINK_luafree: obj/main_luafree.o obj/voro.o obj/fileloader.o obj/pointpattern.o
	$(CXX) obj/main_luafree.o obj/voro.o obj/fileloader.o obj/pointpattern.o -o bin/pomelo


LINK: obj/main.o obj/voro.o obj/fileloader.o obj/pointpattern.o
	$(CXX) obj/main.o obj/voro.o obj/fileloader.o obj/pointpattern.o -o bin/pomelo -llua5.2 -I/usr/include/lua5.2 $(LUAFLAG) 



clean:
	rm obj/*
	rm bin/pomelo
