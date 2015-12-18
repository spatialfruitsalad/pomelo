#ifndef FILELOADER_GUARD
#define FILELOADER_GUARD

#include <vector>
#include <string>
#include "particleparameterset.hpp"
class fileloader
{
    public:
       void read(std::string filename, std::vector<particleparameterset>& set);

};


#endif
