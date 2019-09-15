/* 
Copyright 2018 Simon Weis and Philipp Schoenhoefer

This file is part of Pomelo.

Pomelo is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Pomelo is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Pomelo.  If not, see <http://www.gnu.org/licenses/>.

The development of Pomelo took place at the Friedrich-Alexander University of Erlangen and was funded by the German Research Foundation (DFG) Forschergruppe FOR1548 "Geometry and Physics of Spatial Random Systems" (GPSRS). 
*/
#include <iostream>
#include <vector>
#include <assert.h>
#include <sstream>
#include "../cmdlparser.hpp"

void testcmdlparser();


int main()
{
    
    //first some tests for the point class
    testcmdlparser();

    // all tests complete, done
    std::cout << "\n\nall tests complete!" << std::endl;
    return 0;
}

void convertStringToArgs(std::string s, std::vector<char*>& args)
{
    args.clear();
    // for some reason this conversion ignores the first argument. Not going to dig into right now, so the following line is just a quickfix
    s = "a " + s;
    std::istringstream iss(s);

    std::string token;
    while(iss >> token) {
          char* arg = new char[token.size() + 1];
            copy(token.begin(), token.end(), arg);
              arg[token.size()] = '\0';
                args.push_back(arg);
    }
}

void cleanArgs(std::vector<char*>& args)
{
    for(size_t i = 0; i < args.size(); i++)
       delete[] args[i];
    args.clear();
}

void testcmdlparser()
{
    std::cout << "\n\ntest cmdlparser\n";


    std::cout << "cmdlParser: filename\n";
    {
        std::string t = "-i test.dat -o outfolder -mode sphcyl";
        std::vector<char*> args;
        convertStringToArgs(t, args);
        cmdlParser cp;
        cp.parseArguments(args.size(), &args[0]);
        assert (cp.filename == "test.dat");
        cleanArgs(args);
    }
    {
        std::string t = "-i blahurazzzzpumpelpuuu -o outfolder -mode sphcyl";
        std::vector<char*> args;
        convertStringToArgs(t, args);
        cmdlParser cp;
        cp.parseArguments(args.size(), &args[0]);
        assert (cp.filename == "blahurazzzzpumpelpuuu");
        cleanArgs(args);
    }

    std::cout << "cmdlParser: outfolder\n";
    {
        std::string t = "-i test.dat -o outfolder -mode sphcyl";
        std::vector<char*> args;
        convertStringToArgs(t, args);
        cmdlParser cp;
        cp.parseArguments(args.size(), &args[0]);
        assert (cp.outfolder == "outfolder");
        cleanArgs(args);
    }
    {
        std::string t = "-o blahurazzzzpumpelpuuu -i outfolder -mode sphcyl";
        std::vector<char*> args;
        convertStringToArgs(t, args);
        cmdlParser cp;
        cp.parseArguments(args.size(), &args[0]);
        assert (cp.outfolder == "blahurazzzzpumpelpuuu");
        cleanArgs(args);
    }
    std::cout << "cmdlparser: argument order\n";
    {
        std::string t = "-i in -o out -mode sphcyl";
        std::vector<char*> args;
        convertStringToArgs(t, args);
        cmdlParser cp;
        cp.parseArguments(args.size(), &args[0]);
        assert (cp.outfolder == "out");
        assert (cp.filename == "in");
        assert (cp.thisMode == SPHCYL);
        cleanArgs(args);
    }
    {
        std::string t = "-o out -i in -mode sphcyl";
        std::vector<char*> args;
        convertStringToArgs(t, args);
        cmdlParser cp;
        cp.parseArguments(args.size(), &args[0]);
        assert (cp.outfolder == "out");
        assert (cp.filename == "in");
        assert (cp.thisMode == SPHCYL);
        cleanArgs(args);
    }
    {
        std::string t = "-o out -mode sphcyl -i in";
        std::vector<char*> args;
        convertStringToArgs(t, args);
        cmdlParser cp;
        cp.parseArguments(args.size(), &args[0]);
        assert (cp.outfolder == "out");
        assert (cp.filename == "in");
        assert (cp.thisMode == SPHCYL);
        cleanArgs(args);
    }
    {
        std::string t = "-i in -mode sphcyl -o out";
        std::vector<char*> args;
        convertStringToArgs(t, args);
        cmdlParser cp;
        cp.parseArguments(args.size(), &args[0]);
        assert (cp.outfolder == "out");
        assert (cp.filename == "in");
        assert (cp.thisMode == SPHCYL);
        cleanArgs(args);
    }
    {
        std::string t = "-mode sphcyl -i in -o out";
        std::vector<char*> args;
        convertStringToArgs(t, args);
        cmdlParser cp;
        cp.parseArguments(args.size(), &args[0]);
        assert (cp.outfolder == "out");
        assert (cp.filename == "in");
        assert (cp.thisMode == SPHCYL);
        cleanArgs(args);
    }
    {
        std::string t = "-mode sphcyl -o out -i in";
        std::vector<char*> args;
        convertStringToArgs(t, args);
        cmdlParser cp;
        cp.parseArguments(args.size(), &args[0]);
        assert (cp.outfolder == "out");
        assert (cp.filename == "in");
        assert (cp.thisMode == SPHCYL);
        cleanArgs(args);
    }
}
