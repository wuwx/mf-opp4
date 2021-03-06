//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __MF_OPP4_ENERGYROUTE_H_
#define __MF_OPP4_ENERGYROUTE_H_

#include <omnetpp.h>

#include "WiseRoute.h"

/**
 * TODO - Generated class
 */
class EnergyRoute : public WiseRoute
{
  public:
	virtual void initialize(int);
    virtual void handleMessage(cMessage *msg);
    virtual void handleLowerMsg(cMessage* msg);
    virtual void updateRouteTable(int origin, int lastHop, double rssi, double ber);

};

#endif
