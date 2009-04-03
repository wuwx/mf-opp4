/* -*- mode:c++ -*- ********************************************************
 * file:        Energy.h
 *
 * author:      Jerome Rousselot
 *
 * copyright:   (C) 2009 CSEM SA
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

#ifndef ENERGY_H
#define ENERGY_H

#include <omnetpp.h>
#include <Blackboard.h>
#include <sstream>

/**
 * @brief Class that keeps track of the energy used.
 *
 * @ingroup blackboard
 * @author Andreas K�pke
 * @sa Blackboard
 */

class  Energy : public BBItem
{
    BBITEM_METAINFO(BBItem);

protected:
    /** @brief energy used until now. */
	double energyUsed;
	/**@brief power consumption of current mode. */
	double currentP;
	/**@brief host address */
    long host;

public:

    /** @brief Constructor*/
    Energy() : BBItem(), energyUsed(0), currentP(0), host(0) {
    };


    double getEnergyUsed() const  {
        return energyUsed;
    }

    double getcurrentP() const  {
            return currentP;
	}

    void setEnergyUsed(double e)  {
        energyUsed = e;
    }

    void setCurrentP(double p)  {
            currentP = p;
	}

    void setHost(long addr) {
    	host = addr;
    }

    /** @brief Enables inspection */
    std::string info() const {
        std::ostringstream ost;
        ost << " Energy used is "<< energyUsed;
        return ost.str();
    }
};


#endif
