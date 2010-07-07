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
simple EnergyBattery
{
    parameters:
        double maxBatteryCapacity @unit(mAh) = default(3000mAh); // the max battery capacity of this type of nodes in mAh
        double batteryCapacity @unit(mAh) = default(this.maxBatteryCapacity);	// the initial capacity of the battery in mAh
        double batteryTX @unit(mA) = default(350mA);    // current drawn during SEND with full power (MICA 2 default value: RADIO + CPU)
        double batteryRECV @unit(mA) = default(180mA);  // current drawn during RECV (MICA 2 default value: RADIO + CPU)
        double batterySLEEP @unit(mA) = default(15uA); // current drawn during SLEEP (MICA 2 default value: both RADIO and CPU sleep)
        int updateInterval @unit(s) = default(60s); // how often to update the battery state
}
