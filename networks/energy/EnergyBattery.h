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

#ifndef __MF_OPP4_ENERGYBATTERY_H_
#define __MF_OPP4_ENERGYBATTERY_H_

#include <omnetpp.h>
#include <BasicModule.h>
#include <RadioAccNoise3State.h>
#include "BatteryState.h"

/**
 * TODO - Generated class
 */
class EnergyBattery : public BasicModule
{
	public:
		BatteryState batteryState;

	protected:
		// parameters
		double batteryRECV;
		double batteryTX;
		double batterySLEEP;
		double maxBattery;

		// statistics
		cOutVector batteryStateVector;

		// state
		cMessage* updateStateMsg;
		int updateInterval;

		int batteryStateCategory;
		int radioStateCategory;  // category number given by bb for RadioState
		RadioAccNoise3State::States radioState;  //  Tracking the RadioState

		simtime_t lastTimestamp;

		simtime_t timeInSleep;
		simtime_t timeInRecv;
		simtime_t timeInTx;

	protected:
		/** @brief Some initialization stuff */
		virtual void initialize(int);

		/** @brief some finishing stuff - close the stats, etc. */
		virtual void finish();

		/** @brief handle all messages for this module - self message only, for updating the batteryState. */
		virtual void handleMessage(cMessage *msg);

		/** @brief Called by the Blackboard whenever a change occurs we're interested in */
		virtual void receiveBBItem(int category, const BBItem *details, int scopeModuleId);

		// helper function
		void doUpdateBatteryState(simtime_t& timeInState, double currentDrawn);

		/** @brief updates the timeInSleep/timeInRecv/timeInTx variable and the battery state since the last update */
		void updateBatteryState(RadioAccNoise3State::States radioState);
};

#endif
