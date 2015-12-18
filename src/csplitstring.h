// Copyright 2015 Simon Weis

// This file is part of RAPS.
//
// RAPS is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//
// RAPS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with RAPS.  If not, see <http://www.gnu.org/licenses/>.
#ifndef CSPLITSTRING_H_INCLUDED
#define CSPLITSTRING_H_INCLUDED

#include <string>
#include <vector>
#include <iostream>

class cSplitString : public std::string
{
    std::vector<std::string> flds;
public:
    cSplitString(char* s) : std::string(s) { };
    std::vector<std::string>& split(char delim, int rep=0)
    {
        if (!flds.empty()) flds.clear();  // empty vector if necessary
        std::string work = data();
        std::string buf = "";
        unsigned int i = 0;
        while (i < work.length())
        {
            if (work[i] != delim)
                buf += work[i];
            else if (rep == 1)
            {
                flds.push_back(buf);
                buf = "";
            }
            else if (buf.length() > 0)
            {
                flds.push_back(buf);
                buf = "";
            }
            i++;
        }
        if (!buf.empty())
            flds.push_back(buf);
        return flds;
    }
};


#endif // CSPLITSTRING_H_INCLUDED
