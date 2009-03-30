package org.mf2.contrib.nic;
//***************************************************************************
// * file:        Nic802154_TI_CC2420.ned
// *
// * author:      Jerome Rousselot, Marc Loebbers
// *
// * copyright:   (C) 2008-2009 CSEM SA, Neuchatel, Switzerland.
// *		  	  (C) 2004 Telecommunication Networks Group (TKN) at
// *              Technische Universitaet Berlin, Germany.
// *
// *              This program is free software; you can redistribute it
// *              and/or modify it under the terms of the GNU General Public
// *              License as published by the Free Software Foundation; either
// *              version 2 of the License, or (at your option) any later
// *              version.
// *              For further information see file COPYING
// *              in the top level directory
// *
// * Funding: This work was partially financed by the European Commission under the  
// * Framework 6 IST Project ”Wirelessly Accessible Sensor Populations"  
// * (WASP) under contract IST-034963.
// ***************************************************************************
// * part of:    Modifications to the MF-2 framework by CSEM
// **************************************************************************/

import org.mf2.contrib.nic.radios.SingleChannelRadioAccNoise3;
import org.mf2.contrib.nic.phyLayer.DeciderRadioAccNoise3;
import org.mf2.contrib.nic.phyLayer.SnrEvalRadioAccNoise3;
import org.mf2.contrib.nic.macLayer.csma;


//
// This NIC implements a Texas Instruments CC 2420 802.15.4 network interface card
// using the CSMA protocol as specified in IEEE 802.15.4-2006.
// @author Jerome Rousselot
//

module Nic802154_TI_CC2420

{
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
        snrEval: SnrEvalRadioAccNoise3 {
            parameters:
                publishRSSIAlways = false;
                headerLength = default (48 bit); // ieee 802.15.4
                transmitterPower = default(1 mW);
                carrierFrequency = default(2.4E+9 Hz);
                thermalNoise = default(-110 dBm);
                sfdLength = default(8 bit);
                @display("p=96,236;i=block/wrxtx");

        }
        radio: SingleChannelRadioAccNoise3 {
            parameters:
                debug = false;
                bitrate = 250000 bps;
                delaySetupRx = 0.001792 s;
                delaySetupTx = 0.001792 s;
                delayRxTx = 0.000192 s;
                delayTxRx = 0.000192 s;
                sleepPower = 0.0000000693 W; // 0.021 microA * 3.3 V
                rxPower = 0.06204 W;   // 18.8 mA * 3.3 V
                txPower = 0.05742 W;   // 17.4 mA * 3.3 V
                rxTxPower = 0.06204 W; // Upper bound : 18.8 mA * 3.3 V
                txRxPower = 0.06204 W; // idem
                setupTxPower = 0.002109 W; // (ESetupPower+ESetupXtal+ETxCalibrate)/TSetupTx
                setupRxPower = 0.002259 W; // (ESetupPower+ESetupXtal+ERxCalibrate)/TSetupRx
                @display("p=200,30;b=30,25");

        }
        mac: csma {
            parameters:
                rxSetupTime = 0.001792 s;
				
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

