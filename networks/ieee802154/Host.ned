//***************************************************************************
// * file:        Network.ned
// *
// * author:      Jerome Rousselot
// *		  Marc Loebbers, der Schoenste!
// *
// * copyright:   (C) 2009 CSEM SA, Neuchatel, Switzerland.
// *		  (C) 2004 Telecommunication Networks Group (TKN) at
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


package org.mf2.networks.ieee802154;

import org.mf2.contrib.applLayer.SensorApplLayer;
import org.mf2.contrib.mobility.ConstSpeedMobility;
import org.mf2.contrib.netwLayer.UbiquitousArp;
import org.mf2.contrib.netwLayer.WiseRoute;
import org.mf2.contrib.nic.Nic802154_TI_CC1100;


module Host

{
    parameters:
        @display("bgb=250,250,white;bgp=10,10;i=device/wifilaptop");
    gates:
        input radioIn; // gate for sendDirect

    submodules:
        mobility: ConstSpeedMobility {
            parameters:
                @display("p=200,150");

        }
        appl: SensorApplLayer {
            parameters:
                @display("p=90,50");

        }
        ubiquitousarp: UbiquitousArp {
            parameters:
                @display("p=200,225");

        }
        net: WiseRoute {
            parameters:
                @display("p=90,120");

        }
        nic: Nic802154_TI_CC1100 {
            parameters:
                @display("p=90,189");

        }
    connections:
        nic.uppergateOut --> net.lowergateIn;
        nic.uppergateIn <-- net.lowergateOut;
        nic.upperControlOut --> net.lowerControlIn;

        net.uppergateOut --> appl.lowergateIn;
        net.uppergateIn <-- appl.lowergateOut;
        net.upperControlOut --> appl.lowerControlIn;

        radioIn --> nic.radioIn;
}
