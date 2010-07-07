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

package org.mf2.networks.energy;

//
// TODO auto-generated module
//
simple EnergyRoute
{
    parameters:
        // debug switch
        bool debug = default(false);
        bool trace = default(false);
        bool stats = default(true);
     	bool useSimTracer = default(false);
     	
        // sink node IP address (integer)
        int sinkAddress = default(0);
        // the sink directs the tree building procedure with periodic floods.
        // iterationDelay is the period between two floods.
        
        // lenght of the routing packet header
        double headerLength @unit(byte) = default(10 byte);
        
        // RSSI threshold for route selection
        double rssiThreshold @unit(dBm) = default(-50 dBm);
        
        // If set to zero, this node does not initiates route tree building.
        // If set to a value larger than zero, this nodes periodically initiates route tree building.
        double routeFloodsInterval @unit(s) = default(0 s);

    gates:
        input uppergateIn; // from application layer
        output uppergateOut; // to application layer
        output upperControlOut; // for control messages to upper layer
        input lowerControlIn;   // for control messages from mac layer
        input lowergateIn; // from MAC layer
        output lowergateOut; // to MAC layer
}
