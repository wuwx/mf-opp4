package org.mf2.contrib.nic.phyLayer;
//***************************************************************************
// * file:        Decider802154.ned
// *
// * author:      Jerome Rousselot, Amre El-Hoiydi, Marc Loebbers 
// *
// * copyright:   (C) 2004 Telecommunication Networks Group (TKN) at
// *              Technische Universitaet Berlin, Germany.
// *              (C) 2007-2009 CSEM, Neuchatel, Switzerland.
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


//
// Decider module of the 802.15.4 modules
//
simple DeciderRadioAccNoise3
{
    parameters:
        bool debug = default(false); // debug switch
        bool stats = default(true); // record some scalar statistics
        double berLowerBound = default(1E-8); // minimum possible bit error rate (BER floor).
	string modulation = default("msk"); // modulation type
    gates:
        output uppergateOut;
        output upperControlOut;
        input lowergateIn;
        input lowerControlIn;
}
