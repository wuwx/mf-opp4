package org.mf2.contrib.nic.radios;
//***************************************************************************
// * file:        SingleChannelRadioAccNoise3.ned
// *
// * author:      Jerome Rousselot, Amre El-Hoiydi, Andreas Koepke
// *
// * copyright:   (C) 2007-2009 CSEM, Neuchatel, Switzerland.
// *			  (C) 2004 Telecommunication Networks Group (TKN) at
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

simple SingleChannelRadioAccNoise3
{
    parameters:
        bool debug = default(false); // debug switch
		bool useSimTracer = default(false);
        double bitrate @unit(bps); // radio bit rate (in bits per second)

        // (1) time parameters (in seconds)
        // (how long does it take to go from one state to another).
        double delaySetupRx @unit(s); // time to switch from sleep mode to Rx mode.
        double delaySetupTx @unit(s); // time to switch from sleep mode to Tx mode.
        double delayRxTx @unit(s);	  // time to switch from Rx mode to Tx mode.
        double delayTxRx @unit(s);    // time to switch from Tx mode to Rx mode.

        // (2) energy parameters (in Watt)
        // power consumption in each radio state.
        double sleepPower @unit(W);   // power consumption in sleep state.
        double setupRxPower @unit(W); // power consumption in SetupRx state.
        double setupTxPower @unit(W); // power consumption in SetupTx state.
        double rxPower @unit(W);      // power consumption in Rx state.
        double txPower @unit(W);	  // power consumption in Tx state.
        double rxTxPower @unit(W);	  // power consumption when switching between Rx and Tx states.
        double txRxPower @unit(W);    // power consumption when switching between Tx and Rx states.
}
