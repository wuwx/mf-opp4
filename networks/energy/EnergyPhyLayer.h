//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __MF_OPP4_ENERGYPHYLAYER_H_
#define __MF_OPP4_ENERGYPHYLAYER_H_

#include <omnetpp.h>
#include "SnrEvalRadioAccNoise3.h"
#include "MacPkt_m.h"
#include "BaseMacLayer.h"

/**
 * TODO - Generated class
 */
class EnergyPhyLayer : public SnrEvalRadioAccNoise3
{
public:

	/** @brief Some extra parameters have to be read in */
	virtual void initialize(int);

	virtual void finish();

protected:

	AirFrameRadioAccNoise3 *encapsMsg(cMessage * msg);

	void handleMessage(cMessage * msg);

	void sendDown(AirFrameRadioAccNoise3 * msg);

	void handleUpperMsg(AirFrameRadioAccNoise3 * frame);

};

#endif
