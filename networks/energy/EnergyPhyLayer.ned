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
simple EnergyPhyLayer
{
    parameters:
        bool debug = default(false); // debug switch
        bool trace = default(false); // vector log switch
        bool stats = default(true); // scalar statistics switch
        bool compatibilityMode = default(false); // if active, the radio will be in reception at initialization time. This allows to run MAC protocols depending on this behavior inherited from the original radio model.
        double alpha = default(2.5);
        double headerLength @unit(bit); // Total length of PHY header (usually sync+sfd) 
        double transmitterPower @unit(mW) = default(1 mW);  // Radiated power at antenna.
        double carrierFrequency @unit(Hz);  // Carrier frequency of the signal (Used for pathloss evaluation).
        double thermalNoise @unit(dBm);      // Thermal noise in receiver.
        bool publishRSSIAlways;				// Always publish RSSI changes to the blackboard independently of  the radio state.
        double sfdLength @unit(bit); // Length of Start Frame Delimiter (used to compute probability of successful synchronization)
		string channelModel = default("freespace");
		//xml channelParameters;
    gates:
        input uppergateIn;
        input radioIn;
        output uppergateOut;
        output upperControlOut;
}
