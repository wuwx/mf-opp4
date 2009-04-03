package org.mf2.contrib.applLayer;
//****************************************************************************
// * file:        SensorApplLayer.ned
// *
// * author:      Amre El-Hoiydi, Jerome Rousselot
// *
// * copyright:   (C) 2007-2009 CSEM SA, Neuchatel, Switzerland.
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

// Application layer to test lower layer implementations. Generate periodic traffic addressed to a sink.
//
// @sa SensorApplLayer
// @author Amre El-Hoiydi, Jerome Rousselot
simple SensorApplLayer
{
    parameters:
        bool debug = default(false); // debug switch
        bool doStats = default(true);   // stats generation switch
        double nbPackets = default(10);
        string trafficType = default("exponential");
        double trafficParam = default(60);
        double initializationTime @unit(s) = default(600 s); // minimum time before generation of the first packet
        double headerLength @unit(byte) = default(10 byte); // length of the application payload

    gates:
        input lowergateIn; // from network layer
        input lowerControlIn;
        output lowergateOut; // to network layer
}
