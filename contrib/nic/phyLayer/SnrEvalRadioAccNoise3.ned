package org.mf2.contrib.nic.phyLayer;
//***************************************************************************
// * file:        SnrEvalRadioAccNoise3.ned
// *
// * author:      Jerome Rousselot, Amre El-Hoiydi, Marc Loebbers
// *
// * copyright:   (C) 2007-2009 CSEM SA, Neuchatel, Switzerland.
// *		 	  (C) 2004 Telecommunication Networks Group (TKN) at
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
// * Framework 6 IST Project "Wirelessly Accessible Sensor Populations"  
// * (WASP) under contract IST-034963.
// ***************************************************************************
// * part of:    Modifications to the MF-2 framework by CSEM
// **************************************************************************/


simple SnrEvalRadioAccNoise3
{
    parameters:
        bool debug = default(false); // debug switch
        bool trace = default(false); // vector log switch
        bool stats = default(true); // scalar statistics switch
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
