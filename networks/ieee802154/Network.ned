//***************************************************************************
// * file:        Network.ned
// *
// * author:      Jerome Rousselot, Marc Loebbers
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

import org.mf2.core.channelControl.ChannelControl;
import org.mf2.contrib.utils.SimTracer;
import org.mf2.core.blackboard.Blackboard;

module Sim
{
    parameters:
        // parametes for the framework
        double playgroundSizeX;
        double playgroundSizeY;
        double numHosts;


        @display("bgb=$playgroundSizeX,$playgroundSizeY,white,,;bgp=0,0");
    submodules:
        blackboard: Blackboard {
            parameters:
                @display("p=200,75");

        }    
        channelcontrol: ChannelControl {
            parameters:
                playgroundSizeX = playgroundSizeX;
                playgroundSizeY = playgroundSizeY;
                @display("p=50,25");
        }
        host[numHosts]: Host {
            parameters:
                @display("p=112,92");

        }
        simTracer: SimTracer {
            @display("p=169,25");
        }

    connections allowunconnected:
    // all connections and gates are to be generated dynamically

}




network sim extends Sim
{
    parameters:
        playgroundSizeX @prompt("playgroundSizeX") = default(40);
        playgroundSizeY @prompt("playgroundSizeY") = default(40);
        numHosts @prompt("Number of hosts:") = default(2);

}
