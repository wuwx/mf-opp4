package org.mf2.contrib.netwLayer;
//***************************************************************************
// file:        pairwise.ned
//
// author:      Jerome Rousselot
// 
// copyright:   (C) 2007-2008 CSEM SA, Neuchatel, Switzerland.
//
// description: Implementation of the pairwise routing protocol
//		Ref: "A Robust, Responsive, Distributed Tree-Based Routing
//		Algorithm Guaranteeing N Valid Links per Node in Wireless
//		Ad-Hoc Networks", Tunc Ikikardes, Markus Hofbauer, 
//		August Kaelin, Martin May, Proceedings of IEEE
//		Symposium on Computers and Communications, IEEE, July, 2007.
//*
//*              This program is free software; you can redistribute it
//*              and/or modify it under the terms of the GNU General Public
//*              License as published by the Free Software Foundation; either
//*              version 2 of the License, or (at your option) any later
//*              version.
//*              For further information see file COPYING
//*              in the top level directory
//*
//* Funding: This work was partially financed by the European Commission under the  
//* Framework 6 IST Project "Wirelessly Accessible Sensor Populations"  
//* (WASP) under contract IST-034963.
//***************************************************************************
//* part of:    Modifications to the MF-2 framework by CSEM
//**************************************************************************/

simple pairwise
{
    parameters:
        // debug switch
        bool debug;
        bool trace;
        bool stats;
        bool useSimTracer;
        // number of spanning trees to be built
        // Note that the algorithm will converge only if 
        // n =< min{Node degrees}
        // that is, if only one node has less than n neighbours routing
        // will not work.
        // (currently this feature is not implemented ; the protocol builds
        // only one route  to the sink per node.)
        double n;
        // sink node IP address (integer)
        double sinkAddress;
        // the sink directs the tree building procedure with periodic floods.
        // iterationDelay is the period between two floods.
        double iterationDelay @unit(s);
        // time to wait after receiving an iteration message before taking a
        // decision concerning the favorite parent. This time should be large
        // enough so that all nodes have received the iteration message and
        // rebroadcast it.
        double decisionDelay @unit(s);
        // Time to wait for an acknowledgement after sending a join request
        // to a favorite child.
        double ackDelay @unit(s);
        // Time to wait for a join request from a favorite parent after chosing
        // favorite parent.
        double assocDelay @unit(s);
        // Number of additional tree-building iterations to perform when an 
        // iteration does not lead to a new node joining the tree.
        double MaxNoUpdates;
        // Selects the routing metric to use. Refer to pairwise.h, metrics enum:
        // 1 for BER-based metric
        // 2 for hop count-based metric
        double metric;
        // eventual parameter for the metric
        // for the hop count based metric, it is the maximum acceptable BER
        // for a link to be considered (e.g. 0.01).
        double metricThreshold;
        // lenght of the routing packet header
        double headerLength @unit(byte);

    gates:
        input uppergateIn; // from application layer
        output uppergateOut; // to application layer
        output upperControlOut; // for control messages to upper layer
        input lowerControlIn;   // for control messages from mac layer
        input lowergateIn; // from MAC layer
        output lowergateOut; // to MAC layer
}

