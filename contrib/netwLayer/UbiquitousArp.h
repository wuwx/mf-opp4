/* -*- mode:c++ -*- ********************************************************
 * file:        UbiquitousArp.h
 *
 * author:  	Jerome Rousselot
 * 			    Daniel Willkomm
 *
 * copyright:   (C) 2007-2009 CSEM
 * 				(C) 2005 Telecommunication Networks Group (TKN) at
 *              Technische Universitaet Berlin, Germany.
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
 * Framework 6 IST Project ”Wirelessly Accessible Sensor Populations"
 * (WASP) under contract IST-034963.
 ***************************************************************************
 * part of:    Modifications to the MF-2 framework by CSEM
 **************************************************************************/

#ifndef UBIQUITOUSARP_H
#define UBIQUITOUSARP_H


#include <omnetpp.h>
#include <BasicModule.h>
#include <ModuleAccess.h>
#include "BaseMacLayer.h"

/**
 * @brief
 *
 * This ARP class allows magical address resolution:
 * it queries the simulator to establish the mappings between
 * mac and IP addresses, instead of sending network query packets.
 *
 * @ingroup netwLayer
 * @author Jerome Rousselot
 **/

class UbiquitousArp : public BasicModule
{


public:

	  /** @brief Initialization of the module and some variables*/
	  virtual void initialize(int);

    /** @brief should not be called,
     *  instead direct calls to the radio methods should be used.
     */
    virtual void handleMessage( cMessage* ){
        error("ARP module cannot receive messages!");
    };

    /** @brief returns a MACAddress associated to a given IPAddress */
    int getMacAddr(const int netwAddr);

    /** @brief returns an IPAddress associated to a given MACAddress */
    int getNetwAddr(const int macAddr);

};

class  UbiquitousArpAccess : public ModuleAccess<UbiquitousArp>
{
  public:
    UbiquitousArpAccess() : ModuleAccess<UbiquitousArp>("ubiquitousarp") {}
};


#endif
