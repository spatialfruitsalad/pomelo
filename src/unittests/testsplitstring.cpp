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
#include "../splitstring.hpp"

void testsplitstring();


int main()
{
    
    //first some tests for the point class
    testsplitstring();

    // all tests complete, done
    std::cout << "\n\nall tests complete!" << std::endl;
    return 0;
}


void testsplitstring()
{
    std::cout << "splitstring\n";
    // empty string
    {
        std::string t = "";
        splitstring ss(t.c_str());
        auto ssv  = ss.split('a');
        assert(ssv.size() == 0);
    }
    // string consists only of delimiters
    {
        std::string t = "----";
        splitstring ss(t.c_str());
        auto ssv  = ss.split('-');
        assert(ssv.size() == 0);
    }
    // 
    {
        std::string t = "- -- -";
        splitstring ss(t.c_str());
        auto ssv  = ss.split('-');
        assert(ssv.size() == 2);
        assert(ssv[0] == " ");
        assert(ssv[1] == " ");
    }
    {
        std::string t = "-a--b-";
        splitstring ss(t.c_str());
        auto ssv  = ss.split('-');
        assert(ssv.size() == 2);
        assert(ssv[0] == "a");
        assert(ssv[1] == "b");
    }
    {
        std::string t = "-i test.dat -o outfolder -mode SPHERE";
        splitstring ss(t.c_str());
        auto ssv  = ss.split(' ');
        assert(ssv.size() == 6);
        assert(ssv[0] == "-i");
        assert(ssv[3] == "outfolder");
    }
}
