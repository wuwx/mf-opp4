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

#include "EnergyPhyLayer.h"
#include "PhyControlInfo.h"


Define_Module(EnergyPhyLayer);

void EnergyPhyLayer::initialize(int stage) {
	SnrEvalRadioAccNoise3::initialize(stage);
}

void EnergyPhyLayer::finish() {
	SnrEvalRadioAccNoise3::finish();
}

AirFrameRadioAccNoise3 *EnergyPhyLayer::encapsMsg(cMessage * msg) {
	EV << "(encapsMsg)" << endl;
	AirFrameRadioAccNoise3 *frame;

	frame = new AirFrameRadioAccNoise3(msg->getName(), AIRFRAME);
	PhyControlInfo *pco =
			static_cast<PhyControlInfo *> (msg->removeControlInfo());
	frame->setBitrate(pco->getBitrate());
	frame->setPSend(transmitterPower);
	frame->setBitLength(headerLength);
	frame->setHostMove(hostMove);
	frame->setChannelId(channel.getActiveChannel());
	assert(static_cast<cPacket*>(msg));
	cPacket* pkt = static_cast<cPacket*> (msg);
	frame->encapsulate(pkt);
	frame->setDuration(frame->getBitLength() / frame->getBitrate());
	frame->setHostMove(hostMove);
	delete pco;

	//  EV << "SnrEvalRadioAccNoise3::encapsMsg duration: " << frame->
	//    getDuration() << "\n";
	return frame;
}



void EnergyPhyLayer::handleMessage(cMessage * msg) {
	//EV << "(handleMessage)" << endl;
	if (msg->getArrivalGateId() == uppergateIn) {
		AirFrameRadioAccNoise3 *frame = encapsMsg(msg);

		handleUpperMsg(frame);
	} else if (msg == txOverTimer) {
		//EV << "transmission over" << endl;
		sendControlUp(new cMessage("TRANSMISSION_OVER",
				NicControlType::TRANSMISSION_OVER));
	} else if (msg->isSelfMessage()) {
		if (msg->getKind() == RECEPTION_COMPLETE) {
			//EV << "frame is completely received now\n";
			// unbuffer the message
			AirFrameRadioAccNoise3 *frame = unbufferMsg(msg);
			handleLowerMsgEnd(frame);
		} else {
			handleSelfMsg(msg);
		}
	} else {
		// msg must come from channel
		AirFrameRadioAccNoise3 *frame =
				static_cast<AirFrameRadioAccNoise3 *> (msg);
		handleLowerMsgStart(frame);
		bufferMsg(frame);
	}
}

void EnergyPhyLayer::sendDown(AirFrameRadioAccNoise3 * msg) {
        //EV << "(sendDown)" << endl;
        if (!msg)
                EV<< "msg == null !!!" << endl;
        else {
                MacPkt *macPkt = static_cast<MacPkt *> (msg->dup()->decapsulate());
                int dest = macPkt->getDestAddr();
                cMessage *msg2 = static_cast < cMessage *>(msg);
                if (dest == BaseMacLayer::MAC_BROADCAST) {
                        sendToChannel(msg2, 0.0);
                } else {
                	EV << dest << "=============================================" << endl;
                    const cGate *outGate = cc->getOutGateTo(getParentModule()->getId(), dest, &hostMove.startPos );
					//EV << outGate->getId() << "=============================================" << endl;
                    sendDelayed(msg2 , 0.0, outGate->getId());
                }
        }
}

void EnergyPhyLayer::handleUpperMsg(AirFrameRadioAccNoise3 * frame) {
	//EV << "(handleUpperMsg)" << endl;
	scheduleAt(simTime() + frame->getDuration(), txOverTimer);
	sendDown(frame);
	if (trace) {
		frameDurations.record(static_cast<double> (frame->getDuration()));
	}
}
