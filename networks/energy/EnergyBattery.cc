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

#include "EnergyBattery.h"

#define myId (getParentModule()->getId()-4)

Define_Module(EnergyBattery);

void EnergyBattery::initialize(int stage)
{
	BasicModule::initialize(stage);
	EV << "Stage is " << stage << endl;
	if (stage == 0)
	{
		batteryStateVector.setName("batteryState");

		maxBattery = par("maxBatteryCapacity");
        batteryTX = par("batteryTX");
        batteryRECV = par("batteryRECV");
		batterySLEEP = par("batterySLEEP");

		batteryState.setBatteryState(double(par("batteryCapacity")) / maxBattery);

		timeInSleep = 0;
		timeInTx = 0;
		timeInRecv = 0;

		updateInterval = par("updateInterval");
		if (updateInterval < 1)
			updateInterval = 1;

        radioState = RadioAccNoise3State::RX;
        RadioAccNoise3State cs;
        radioStateCategory = -1;
        radioStateCategory = bb->subscribe(this, &cs, getParentModule()->getSubmodule("nic")->getId());
		EV << "Category is " << radioStateCategory << endl;
		lastTimestamp = 0;

		batteryStateCategory = bb->getCategory(&batteryState);

		updateStateMsg = NULL;
	}
	else if (stage == 1)
	{
		// publish the battery state
		bb->publishBBItem(batteryStateCategory, &batteryState, getParentModule()->getId());

		// schedule the update timer
		updateStateMsg = new cMessage("BatteryUpdateTimer");
		scheduleAt(simTime() + updateInterval, updateStateMsg);
		EV << "Scheduling the timer.\n";
	}
}

/** @brief some finishing stuff - close the stats, etc. */
void EnergyBattery::finish()
{
	cancelAndDelete(updateStateMsg);
	//delete updateState;

	recordScalar("timeInSleep", timeInSleep);
	recordScalar("timeInTx", timeInTx);
	recordScalar("timeInRecv", timeInRecv);
}

/**
 * Update the internal copies of interesting BB variables
 *
 */
void EnergyBattery::receiveBBItem(int category, const BBItem *details, int scopeModuleId)
{

	Enter_Method_Silent();
	BasicModule::receiveBBItem(category, details, scopeModuleId);

	if (batteryState.getBatteryState() <= 0)
		return;

    if (category == radioStateCategory)
    {
    	RadioAccNoise3State::States newRadioState = static_cast<const RadioAccNoise3State *>(details)->getState();

		EV << "===New radio state. Current radio state: " << radioState << ", new state:" << newRadioState << endl;
		// if the last radioState was RECV and the new is TX, substract time*batteryRECV
		if (newRadioState == radioState)
		{
			EV << "No change in radio state. \n";
			return;
		}
		if ((newRadioState != RadioAccNoise3State::RX) && (newRadioState != RadioAccNoise3State::TX) && (newRadioState != RadioAccNoise3State::SLEEP))
		{
			EV << "New radio state different from RECV, SEND or SLEEP. Ignore and stay in the same state.\n";
			return;
		}

		EV << "NIC ID: " << getParentModule()->getSubmodule("nic")->getId() << " BB ID: " << scopeModuleId << endl;
		updateBatteryState(newRadioState);

		radioState = newRadioState;

		bb->publishBBItem(batteryStateCategory, &batteryState, getParentModule()->getId());

		batteryStateVector.record(batteryState.getBatteryState());

		EV << "New battery status: " << batteryState.getBatteryState() << endl;
		EV << "New radio state: " << radioState << endl;
    }
}

void EnergyBattery::handleMessage(cMessage *msg)
{
	// the module can receive only self messages
	if (msg != updateStateMsg)
		error("invalid message received.");

	// update the state
	//updateBatteryState(radioState);

	// publish the new value.
	bb->publishBBItem(batteryStateCategory, &batteryState, getParentModule()->getId());

	batteryStateVector.record(batteryState.getBatteryState());

	EV << "Current battery status: " << batteryState.getBatteryState() << endl;

	// reschedule or die.
	if (batteryState.getBatteryState() > 0)
	{
		EV << "Rescheduling the update timer.\n";
		scheduleAt(simTime() + updateInterval, updateStateMsg);
	}
	else
	{
		// TODO disable radio or take appropriate action
		EV << "Battery is empty. END. \n";
	}
}

void EnergyBattery::doUpdateBatteryState(simtime_t& timeInState, double currentDrawn)
{
	simtime_t deltaT = simTime() - lastTimestamp;
	timeInState += deltaT;
	deltaT = 1;
	batteryState.setBatteryState(batteryState.getBatteryState() - currentDrawn * SIMTIME_DBL(deltaT) / 3600 / maxBattery);
	lastTimestamp = simTime();
}

void EnergyBattery::updateBatteryState(RadioAccNoise3State::States radioState)
{
	if (radioState == RadioAccNoise3State::RX)
		doUpdateBatteryState(timeInRecv, batteryRECV);
	else if (radioState == RadioAccNoise3State::TX)
		doUpdateBatteryState(timeInTx, batteryTX);
	else if (radioState == RadioAccNoise3State::SLEEP)
		doUpdateBatteryState(timeInSleep, batterySLEEP);
}
