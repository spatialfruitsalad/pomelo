#include<fstream>
#include<iostream>

#include<sstream>
#include<string>
#include "fileloader.hpp"

#include "csplitstring.h"

void fileloader::read(std::string filename, std::vector<particleparameterset>& set)
{
    std::ifstream infile;
    infile.open(filename, std::ifstream::in);
    if (infile.fail())
    {
        std::cout << "Cannot load file " << filename << std::endl;
        return;
    }
#pragma GCC diagnostic ignored "-Wwrite-strings"
    cSplitString line("");
    unsigned int linesloaded = 0;
    std::getline(infile, line);
    while (std::getline(infile, line))
    {
        if(line.find("#")!=std::string::npos) continue; // ignore comment lines

        std::vector<std::string> thisparams = line.split(' ');

        particleparameterset p;
        for (auto it = thisparams.begin(); it != thisparams.end(); ++it)
        {
            double d = std::stof((*it));
            p.push_back(d);
        }
        linesloaded++;
        set.push_back(p);

    }
    std::cout << "Lines loaded: " << linesloaded << std::endl << std::endl;

    infile.close();
}
