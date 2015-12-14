#ifndef PARTICLEPARAMETER_GUARD
#define PARTICLEPARAMETER_GUARD

#include <vector>

class particleparameterset
{
    public:
        std::vector<double> parameter;
        void push_back(double x){parameter.push_back(x);};
};


#endif
