/* -*- mode:c++ -*- ********************************************************
 * file:        BaseMacLayer.h
 *
 * author:      Jerome Rousselot
 *
 * copyright:   (C) 2009 CSEM SA
 *              This program is free software; you can redistribute it
 *              and/or modify it under the terms of the GNU General Public
 *              License as published by the Free Software Foundation; either
 *              version 2 of the License, or (at your option) any later
 *              version.
 *              For further information see file COPYING
 *              in the top level directory
 *
 * Funding: This work was partially financed by the European Commission under the
 * Framework 6 IST Project ”Wirelessly Accessible Sensor Populations"
 * (WASP) under contract IST-034963.
 ***************************************************************************
 * part of:    Modifications to the MF-2 framework by CSEM
 **************************************************************************/

#ifndef BaseMacLayer_H
#define BaseMacLayer_H

#include "BasicMacLayer.h"

class BaseMacLayer : public BasicMacLayer {

public:
	void initialize(int stage);
    int getMACAddress() { return macaddress; }
    int getNETAddress() { return netaddress; }

    static const int NET_BROADCAST, MAC_BROADCAST;

    virtual void handleSelfMsg(cMessage *) {  }

    virtual void handleUpperMsg(cMessage*) { }

    virtual void handleLowerControl(cMessage*) { }

protected:
	int macaddress;
	int netaddress;

private:
};

#endif
