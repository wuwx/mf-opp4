/* -*- mode:c++ -*- ********************************************************
 * file:        UbiquitousArp.cc
 *
 * author:      Jerome Rousselot
 * 				Daniel Willkomm

 *
 * copyright:   (C) 2007-2009 CSEM SA, Neuchatel, Switzerland.
 * 				(C) 2005 Telecommunication Networks Group (TKN) at
 *              Technische Universitaet Berlin, Germany.
 *
 *
 *              This program is free software; you can redistribute it
 *              and/or modify it under the terms of the GNU General Public
 *              License as published by the Free Software Foundation; either
 *              version 2 of the License, or (at your option) any later
 *              version.
 *              For further information see file COPYING
 *              in the top level directory
 *
 * Funding: This work was partially financed by the European Commission under the
 * Framework 6 IST Project "Wirelessly Accessible Sensor Populations"
 * (WASP) under contract IST-034963.
 ***************************************************************************
 * part of:    Modifications to the MF-2 framework by CSEM
 **************************************************************************/

#include "UbiquitousArp.h"

Define_Module(UbiquitousArp);

void UbiquitousArp::initialize(int stage)
{
	BasicModule::initialize(stage);
}

int UbiquitousArp::getMacAddr(const int netwAddr)
{
    if(debug) {
        Enter_Method("getMacAddr(%d)",netwAddr);
    } else {
        Enter_Method_Silent();
    }

    cModule * host = getParentModule();
    cModule * sim = host->getParentModule();
    int macaddress;
    bool found = false;
    for(int i=0; !found && i < host->size(); i++) {
    	cModule * mac = sim->getSubmodule("host", i)->getSubmodule("nic")->getSubmodule("mac");
    	BaseMacLayer * ipif = check_and_cast<BaseMacLayer *>(mac);
    	if(ipif->getNETAddress() == netwAddr) {
    		macaddress = ipif->getMACAddress();
    		found = true;
    	}
    }
    EV << "looking up IP address " << netwAddr << ", found mac address " << macaddress << endl;
    return macaddress;
}

int UbiquitousArp::getNetwAddr(const int macAddr)
{
    if(debug) {
        Enter_Method("getNetwAddr(%d)",macAddr);
    } else {
        Enter_Method_Silent();
    }

    cModule * host = getParentModule();
    cModule * sim = host->getParentModule();
    int netaddress;
    bool found = false;
    for(int i=0; !found && i < host->size(); i++) {
    	cModule * mac = sim->getSubmodule("host", i)->getSubmodule("nic")->getSubmodule("mac");
    	BaseMacLayer * ipif = check_and_cast<BaseMacLayer *>(mac);
    	if(macAddr==ipif->getMACAddress()) {
    		netaddress = ipif->getNETAddress();
    		found = true;
    	}
    }
    EV << "looking up MAC address " << macAddr << ", found IP address " << netaddress << endl;
    return netaddress;
}
