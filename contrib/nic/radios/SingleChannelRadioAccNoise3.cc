/* -*- mode:c++ -*- ********************************************************
 * file:        SingleChannelRadioAccNoise3.cc
 *
 * author:      Jerome Rousselot, Amre El-Hoiydi
 *				Andreas Koepke
 *
 * copyright:   (C) 2007-2009 CSEM SA, Neuchatel, Switzerland.
 *				(C) 2005 Telecommunication Networks Group (TKN) at
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
 * Framework 6 IST Project "Wirelessly Accessible Sensor Populations"
 * (WASP) under contract IST-034963.
 ***************************************************************************
 * part of:     Modifications to the MF-2 framework by CSEM
 ***************************************************************************/

#include "SingleChannelRadioAccNoise3.h"

Define_Module(SingleChannelRadioAccNoise3)
;

void SingleChannelRadioAccNoise3::initialize(int stage) {
	SingleChannelRadio::initialize(stage);
	if (stage == 0) {

		detailedState = RadioAccNoise3State(RadioAccNoise3State::SLEEP);
		aChannel = ActiveChannel(0);
		energyUsed = 0;
		useSimTracer = par("useSimTracer");
		// set bit rate (bps)
		bitrate.setBitrate(par("bitrate").doubleValue());
		// set timing constants (in seconds)
		delay_setup_rx = par("delaySetupRx").doubleValue();
		//EV << "delay_setup_rx duration = " << delay_setup_rx << endl;
		delay_setup_tx = par("delaySetupTx").doubleValue();
		//EV << "delay_setup_Tx duration = " << delay_setup_tx << endl;
		delay_switch_rx_tx = par("delayRxTx").doubleValue();
		//EV << "delay_switch_rx_tx duration = " << delay_switch_rx_tx << endl;
		delay_switch_tx_rx = par("delayTxRx").doubleValue();
		//EV << "delay_switch_tx_rx duration = " << delay_switch_tx_rx << endl;

		// set power consumptions (in Watts) for each radio state
		powerConsumptions[RadioAccNoise3State::SLEEP]
				= par("sleepPower").doubleValue();
		powerConsumptions[RadioAccNoise3State::SETUP_RX]
				= par("setupRxPower").doubleValue();
		powerConsumptions[RadioAccNoise3State::SETUP_TX]
				= par("setupTxPower").doubleValue();
		powerConsumptions[RadioAccNoise3State::RX]
				= par("rxPower").doubleValue();
		powerConsumptions[RadioAccNoise3State::TX]
				= par("txPower").doubleValue();
		powerConsumptions[RadioAccNoise3State::SWITCH_RX_TX]
				= par("rxTxPower").doubleValue();
		powerConsumptions[RadioAccNoise3State::SWITCH_TX_RX]
				= par("txRxPower").doubleValue();

		// initialize the timer
		timer = new cMessage("SwitchTimer");
		nicModuleId = getParentModule()->getId();

		// get channel category, default channel is set by MAC
		aChannelCat = bb->getCategory(&aChannel);

		// get bit rate category, default bit rate is set by MAC
		bitrateCat = bb->getCategory(&bitrate);

		energyCat = bb->getCategory(&energy);

		// define state
		detailedStateCat = bb->getCategory(&detailedState);
		// but don't try to log energy during initialization
		doLogEnergy = false;
		detailedState.setState(RadioAccNoise3State::SLEEP, simTime());
		// configure doLogEnergy correctly
		doLogEnergy = hasPar("logEnergy") ? par("logEnergy").boolValue() : true;

	} else if (stage == 1) {
		// Get MACAddress
		cModule *aModule = getParentModule()->getSubmodule("mac");
		BaseMacLayer *macModule =
				check_and_cast<BaseMacLayer *> (aModule);
		macaddress = macModule->getMACAddress();
		energy.setHost(macaddress);
		// Get a handle to the tracer module
		// const char *tracerModulePath = "sim.simTracer";
		// cModule *modp = simulation.getModuleByPath(tracerModulePath);

		if(useSimTracer) {
			cModule
				*modp =
						this->getParentModule()->getParentModule()->getParentModule()->getSubmodule(
								"simTracer");
			tracer = check_and_cast<SimTracer *> (modp);
		} else {
			tracer = NULL;
		}
		//    EV << "Finished initialization stage 1 of SingleChannelRadioAccNoise3" <<
		//      endl;
	}
}

void SingleChannelRadioAccNoise3::execute(commands command) {
	Enter_Method_Silent();
	//    if(debug) {               // direct methods calls between modules require this block.
	//        Enter_Method("execute()");
	//    } else {
	//        Enter_Method_Silent();
	//    }
	//EV << "In executeRadio" << endl;
	switch (command) {
	case ENTER_SLEEP:
		// always allowed
		// transitions: 3, 4, 7, 8, 14, 17, 24
		if (timer->isScheduled()) {
			cancelEvent(timer);
		}
		timer->setKind(RadioAccNoise3State::SLEEP);
		startTimer(0);
		break;
	case ENTER_TX:
		switch (detailedState.getState()) {
		case RadioAccNoise3State::SLEEP: // transition 1
		case RadioAccNoise3State::SETUP_TX: // transition 10
		case RadioAccNoise3State::SETUP_RX: // transition 12
			timer->setKind(RadioAccNoise3State::SETUP_TX);
			startTimer(0);
			break;
		case RadioAccNoise3State::RX: // transition 22
		case RadioAccNoise3State::SWITCH_RX_TX: // transition 23
		case RadioAccNoise3State::SWITCH_TX_RX: // transition 19
			timer->setKind(RadioAccNoise3State::SWITCH_RX_TX);
			startTimer(0);
			break;
		case RadioAccNoise3State::TX: // transition 13
			break;
		default:
			fsmError(command);
		}
		break;
	case ENTER_RX:
		switch (detailedState.getState()) {
		case RadioAccNoise3State::SLEEP: // transition 5
		case RadioAccNoise3State::SETUP_TX: // transition 11
		case RadioAccNoise3State::SETUP_RX: // transition 9
			timer->setKind(RadioAccNoise3State::SETUP_RX);
			startTimer(0);
			break;
		case RadioAccNoise3State::TX: // transition 15
		case RadioAccNoise3State::SWITCH_TX_RX: // transition 16
		case RadioAccNoise3State::SWITCH_RX_TX: // transition 21
			timer->setKind(RadioAccNoise3State::SWITCH_TX_RX);
			startTimer(0);
			break;
		case RadioAccNoise3State::RX: // transition 22
			break;
		default:
			fsmError(command);
		}
		break;
	default:
		fsmError(command);
	}
}

void SingleChannelRadioAccNoise3::fsmError(commands command) {
	stringstream errMsg;

	errMsg << "Error! Radio FSM (current state is " << detailedState.getState();
	errMsg << ") received an unknown command: " << command << ".";
	error(errMsg.str().data());
}

void SingleChannelRadioAccNoise3::enterState(RadioAccNoise3State::States newState) {
	bool foundError = false;
	switch (newState) {
	// Steady states
	case RadioAccNoise3State::SLEEP:
		state.setState(RadioState::SLEEP);
	case RadioAccNoise3State::TX:
		state.setState(RadioState::SEND);
	case RadioAccNoise3State::RX:
		state.setState(RadioState::RECV);
		break;
		// Transient states
	case RadioAccNoise3State::SETUP_RX:
		timer->setKind(RadioAccNoise3State::RX);
		state.setState(RadioState::SWITCH_TO_RECV);
		startTimer(delay_setup_rx);
		break;
	case RadioAccNoise3State::SETUP_TX:
		timer->setKind(RadioAccNoise3State::TX);
		state.setState(RadioState::SWITCH_TO_SEND);
		startTimer(delay_setup_tx);
		break;
	case RadioAccNoise3State::SWITCH_RX_TX:
		timer->setKind(RadioAccNoise3State::TX);
		state.setState(RadioState::SWITCH_TO_SEND);
		startTimer(delay_switch_rx_tx);
		break;
	case RadioAccNoise3State::SWITCH_TX_RX:
		timer->setKind(RadioAccNoise3State::RX);
		state.setState(RadioState::SWITCH_TO_RECV);
		startTimer(delay_switch_tx_rx);
		break;
	default:
		foundError = true;
		fsmError(newState);
	}
	if (doLogEnergy) {
		logEnergy(newState);
	}
	detailedState.setState(newState, simTime());
	bb->publishBBItem(detailedStateCat, &detailedState, nicModuleId);
	bb->publishBBItem(stateCat, &state, nicModuleId);
}

void SingleChannelRadioAccNoise3::logEnergy(RadioAccNoise3State newState) {
	//  EV << "Logging energy: macaddress=" << macaddress << ", state=" << state.
	//    getState()
	//    << ", entranceTime=" << state.
	//    getEntranceTime() << ", simTime=" << simTime()
	//    << ", powerconsumption=" << powerConsumptions[state.
	//						  getState()] << "." << endl;
	if (useSimTracer)
		tracer->radioEnergyLog(macaddress, detailedState.getState(), simTime()
				- detailedState.getEntranceTime(), powerConsumptions[detailedState.getState()],
				powerConsumptions[newState.getState()]);
	energyUsed = energyUsed + (simTime() - detailedState.getEntranceTime()).dbl()
			* powerConsumptions[detailedState.getState()];
	//energy.setEnergyUsed(energyUsed);
	//energy.setCurrentP(powerConsumptions[newState.getState()]);
	// bb->publishBBItem(energyCat, &energy, macaddress);
}

void SingleChannelRadioAccNoise3::startTimer(simtime_t delay) {
	//EV << "In startTimer, new delay is " << delay << "." << endl;
	if (timer->isScheduled())
		cancelEvent(timer);
	scheduleAt(simTime() + delay, timer);
}

void SingleChannelRadioAccNoise3::fsmError(RadioAccNoise3State::States newState) {
	stringstream errMsg;

	errMsg << "Error! Radio FSM (current state is" << detailedState.getState();
	errMsg << ") was asked to switch to an unknown state:" << newState << ".";
	error(errMsg.str().data());
}

void SingleChannelRadioAccNoise3::handleMessage(cMessage * m) {
	if (m == timer) {
		enterState((RadioAccNoise3State::States) m->getKind() );
	} else {
		error("Radio received an unknown message ! It was not the radio timer.");
	}
}

bool SingleChannelRadioAccNoise3::setActiveChannel(int c) {
	if (debug) {
		Enter_Method
		("setActiveChannel %i", c);
	} else {
		Enter_Method_Silent();
	}
	aChannel.setActiveChannel(c);
	bb->publishBBItem(aChannelCat, &aChannel, nicModuleId);
	return true;
}

bool SingleChannelRadioAccNoise3::setBitrate(double b) {
	if (debug) {
		Enter_Method
		("setBitRate %g", b);
	} else {
		Enter_Method_Silent();
	}
	bitrate.setBitrate(b);
	bb->publishBBItem(bitrateCat, &bitrate, nicModuleId);
	return true;
}

void SingleChannelRadioAccNoise3::finish() {
	SingleChannelRadio::finish();
	if (doLogEnergy)
		logEnergy(RadioAccNoise3State::SLEEP);
	recordScalar("energyUsed", energyUsed);
	recordScalar("avgPowerConsumption", 1000 * energyUsed / simTime().dbl());
}

SingleChannelRadioAccNoise3::~SingleChannelRadioAccNoise3() {
	cancelAndDelete(timer);
}

bool SingleChannelRadioAccNoise3::switchToSleep() {
	execute(ENTER_SLEEP);
	return true;
}

bool SingleChannelRadioAccNoise3::switchToSend() {
	execute(ENTER_TX);
	return true;
}

bool SingleChannelRadioAccNoise3::switchToRecv() {
	execute(ENTER_RX);
	return true;
}
