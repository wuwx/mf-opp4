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

import org.mf2.contrib.nic.radios.SingleChannelRadioAccNoise3;
import org.mf2.contrib.nic.phyLayer.DeciderRadioAccNoise3;
import org.mf2.contrib.nic.phyLayer.SnrEvalRadioAccNoise3;
import org.mf2.contrib.nic.macLayer.csma;

//
// TODO auto-generated type
//
module EnergyNic
{
	//	parameters:
	
	
    gates:
        input uppergateIn; // from upper layers
        input radioIn; // to receive AirFrames
        output uppergateOut; // to upper layers
        output upperControlOut; // control connection

    submodules:
        decider: DeciderRadioAccNoise3 {
            parameters:
                @display("p=106,157;i=block/process_s");

        }
        snrEval: EnergyPhyLayer {
            parameters:
                publishRSSIAlways = false;
                headerLength = 48 bit; // ieee 802.15.4
                carrierFrequency = 868E+6 Hz;
                thermalNoise = -110 dBm;
                sfdLength = 32 bit; // four bytes (sync with wisemac.cc RADIO_OVERHEAD)
                @display("p=96,236;i=block/wrxtx");

        }
        radio: SingleChannelRadioAccNoise3 {
            parameters:
                bitrate = 250000 bps;
                // From TI CC1100 datasheet rev. C
                delaySetupRx = 0.00108 s;
                delaySetupTx = 0.00108 s;
                delayRxTx = 0.000010 s;
                delayTxRx = 0.000022 s;
                // power consumption from boards
                sleepPower = 0.000060 W; // 20 µA * 3 V
                rxPower = 0.0492 W;   // 16.4 mA * 3 V
                txPower = 0.051 W;   // 17 mA * 3 V
                rxTxPower = 0.051 W; 
                txRxPower = 0.051 W; 
                setupTxPower = 0.0246 W; 
                setupRxPower = 0.0246 W; 
                @display("p=200,30;b=30,25");

        }
        mac: EnergyMacLayer {
            parameters:
                rxSetupTime = 0.00108 s;
				
                @display("p=96,87;i=block/layer");

        }
        
    connections:

        radioIn --> snrEval.radioIn;

        snrEval.uppergateIn <-- { @display("m=m,25,0,25,0"); } <-- mac.lowergateOut;
        snrEval.uppergateOut --> { @display("m=m,73,0,50,50"); } --> decider.lowergateIn;
        snrEval.upperControlOut --> decider.lowerControlIn;

        decider.uppergateOut --> { @display("m=m,50,50,75,0"); } --> mac.lowergateIn;
        decider.upperControlOut --> mac.lowerControlIn;

        mac.uppergateOut --> uppergateOut;
        mac.upperControlOut --> upperControlOut;
        mac.uppergateIn <-- uppergateIn;
    
}
