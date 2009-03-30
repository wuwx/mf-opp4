

cplusplus {{
#include <NetwPkt_m.h>
}}



class NetwPkt;



message NSafeLinksPkt extends NetwPkt
{
    // for iteration floods:
    int iteration;
    int status;
    int nbPotentialParents;
    int type;
    double metric;
    double timeSinceIterationStart;
}
