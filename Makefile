CXX = clang++ -Wall -Wextra -O3 -std=c++1y 
CXXVORO = clang++ -std=c++1y -g -O3
CXXTEST = clang++ -std=c++1y -g -O3 -Ilib/googletest-master/googletest/include/
LUAFLAG = -DUSELUA

all:  LINK_luafree LINK unittest applicationtest

GENERIC:  LINK 

obj/voro.o: lib/voro++/src/voro++.*
	$(CXXVORO) -c -o obj/voro.o lib/voro++/src/voro++.cc

obj/fileloader.o: src/fileloader.*
	$(CXX) -c -o obj/fileloader.o src/fileloader.cpp

obj/pointpattern.o: src/pointpattern.*
	$(CXX) -c -o obj/pointpattern.o src/pointpattern.cpp

obj/main_luafree.o: src/main.cpp  src/triangle.hpp src/duplicationremover.hpp src/writerpoly.hpp src/writeroff.hpp src/IWriter.hpp src/postprocessing.hpp src/parsexyz.hpp src/parsepolymer.hpp src/parseellipsoids.hpp src/parsesphcyl.hpp  src/GenericMatrix.h src/parsexyzr.hpp src/parsetetra.hpp src/output.hpp src/colorTable.hpp src/parsetetra_blunt.hpp src/tetrahedra.hpp src/cmdlparser.hpp src/writerfe.hpp
	mkdir -p obj
	mkdir -p bin
	$(CXX) -c -o obj/main_luafree.o src/main.cpp 

obj/main.o: src/main.cpp src/triangle.hpp src/duplicationremover.hpp src/writerpoly.hpp src/writeroff.hpp src/IWriter.hpp src/postprocessing.hpp src/parsexyz.hpp src/parsexyzr.hpp src/parsepolymer.hpp src/parseellipsoids.hpp src/parsesphcyl.hpp src/GenericMatrix.h src/parsetetra.hpp  src/output.hpp src/colorTable.hpp src/parsetetra_blunt.hpp src/tetrahedra.hpp src/cmdlparser.hpp src/writerfe.hpp
	mkdir -p obj
	mkdir -p bin
	$(CXX) -c -o obj/main.o src/main.cpp -I/usr/include/lua5.2 $(LUAFLAG)

LINK_luafree: obj/main_luafree.o obj/voro.o obj/fileloader.o obj/pointpattern.o
	$(CXX) obj/main_luafree.o obj/voro.o obj/fileloader.o obj/pointpattern.o -o bin/pomelo

LINK: obj/main.o obj/voro.o obj/fileloader.o obj/pointpattern.o
	$(CXX) obj/main.o obj/voro.o obj/fileloader.o obj/pointpattern.o -o bin/pomelo_generic -llua5.2 -I/usr/include/lua5.2 $(LUAFLAG) 

unittest: src/cmdlparser.hpp src/duplicationremover.hpp obj/pointpattern.o src/unittests/cmdlparsertest.cpp src/unittests/pointpatterntest.cpp src/unittests/duplicationremovertest.cpp src/unittests/splitstringtest.cpp src/unittests/triangletest.cpp 
	$(CXXTEST) src/unittests/cmdlparsertest.cpp src/unittests/duplicationremovertest.cpp src/unittests/pointpatterntest.cpp src/unittests/splitstringtest.cpp src/unittests/triangletest.cpp obj/pointpattern.o -Llib/googletest-master/build/lib/ -lpthread -lgtest -lgtest_main -o bin/unittest
	./bin/unittest 

applicationtest: src/applicationtests/twospheretest.cpp LINK
	$(CXXTEST) src/applicationtests/twospheretest.cpp -Llib/googletest-master/build/lib/ -lpthread -lgtest -lgtest_main -o bin/applicationtest
	cd bin && ./pomelo_generic -mode GENERIC -i ../test/2015-12-16_twopoints/twopoints.lua -o ../test/2015-12-16_twopoints/applicationtest && cd ..
	./bin/applicationtest && rm -r test/2015-12-16_twopoints/applicationtest

clean:
	rm obj/*
	rm bin/pomelo
