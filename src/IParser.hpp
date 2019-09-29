/* 
Copyright 2019 Simon Weis and Philipp Schoenhoefer

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

#ifndef IPARSER_H_GUARD_123456
#define IPARSER_H_GUARD_123456

class IParser
{
public:

    virtual ~IParser () = default;
    double xmin{0};
    double ymin{0};
    double zmin{0};
    double xmax{0};
    double ymax{0};
    double zmax{0};
    bool xpbc{false};
    bool ypbc{false};
    bool zpbc{false};
    bool percstruct{false};
    unsigned int cellmin{0};
    unsigned int cellmax{0};
};

#endif

