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

#include "EnergyRoute.h"

Define_Module(EnergyRoute);

void EnergyRoute::initialize(int stage)
{
    WiseRoute::initialize(stage);
}

void EnergyRoute::handleMessage(cMessage *msg)
{
	WiseRoute::handleMessage(msg);
}

void EnergyRoute::handleLowerMsg(cMessage* msg)
{
	int macBcastAddr = BaseMacLayer::MAC_BROADCAST;
	int bcastIpAddr = BaseMacLayer::NET_BROADCAST;
	WiseRoutePkt* netwMsg = check_and_cast<WiseRoutePkt*>(msg);
	int finalDestAddr = netwMsg->getFinalDestAddr();
	int initialSrcAddr = netwMsg->getInitialSrcAddr();
	int srcAddr = netwMsg->getSrcAddr();
	double rssi = static_cast<MacControlInfo*>(netwMsg->getControlInfo())->getRSSI();
	double ber = static_cast<MacControlInfo*>(netwMsg->getControlInfo())->getBER();
	// Check whether the message is a flood and if it has to be forwarded.
	floodTypes floodType = updateFloodTable(netwMsg->getIsFlood(), initialSrcAddr, finalDestAddr,
										    netwMsg->getSeqNum());
	allReceivedRSSI.record(rssi);
	allReceivedBER.record(ber);
	if (floodType == DUPLICATE) {
		nbDuplicatedFloodsReceived++;
		delete netwMsg;
	}
	else {
		// If the message is a route flood, update the routing table.
		if (netwMsg->getKind() == ROUTE_FLOOD)
			updateRouteTable(initialSrcAddr, srcAddr, rssi, ber);

		if (finalDestAddr == myNetwAddr || finalDestAddr == bcastIpAddr) {
			WiseRoutePkt* msgCopy;
			if (floodType == FORWARD) {
				// it's a flood. copy for delivery, forward original.
				// if we are here (see updateFloodTable()), finalDestAddr == IP Broadcast. Hence finalDestAddr,
				// initialSrcAddr, and destAddr have already been correctly set
				// at origin, as well as the MAC control info. Hence only update
				// local hop source address.
				msgCopy = check_and_cast<WiseRoutePkt*>(netwMsg->dup());
				netwMsg->setSrcAddr(myNetwAddr);
				((MacControlInfo*) netwMsg->getControlInfo())->setNextHopMac(macBcastAddr);
				sendDown(netwMsg);
				nbDataPacketsForwarded++;
			}
			else
				msgCopy = netwMsg;
			if (msgCopy->getKind() == DATA) {
				sendUp(decapsMsg(msgCopy));
				nbDataPacketsReceived++;
			}
			else {
				nbRouteFloodsReceived++;
				delete msgCopy;
			}
		}
		else {
			// not for me. if flood, forward as flood. else select a route
			if (floodType == FORWARD) {
				netwMsg->setSrcAddr(myNetwAddr);
				((MacControlInfo*) netwMsg->getControlInfo())->setNextHopMac(macBcastAddr);
				sendDown(netwMsg);
				nbDataPacketsForwarded++;
				nbUnicastFloodForwarded++;
			}
			else {
				int nextHop = getRoute(finalDestAddr);
				if (nextHop == bcastIpAddr) {
					// no route exist to destination, attempt to send to final destination
					nextHop = finalDestAddr;
					nbGetRouteFailures++;
				}
				netwMsg->setSrcAddr(myNetwAddr);
				netwMsg->setDestAddr(nextHop);
				((MacControlInfo*) netwMsg->getControlInfo())->setNextHopMac(arp->getMacAddr(nextHop));
				sendDown(netwMsg);
				nbDataPacketsForwarded++;
				nbPureUnicastForwarded++;
			}
		}
	}
}

void EnergyRoute::updateRouteTable(int origin, int lastHop, double rssi, double ber)
{
	tRouteTable::iterator pos;

	pos = routeTable.find(origin);
	receivedRSSI.record(rssi);
	receivedBER.record(ber);
	if (pos == routeTable.end()) {
		// A route towards origin does not exist yet. Insert the currently discovered one
		// only if the received RSSI is above the threshold.
		if (rssi > rssiThreshold) {
			tRouteTableEntry newEntry;

			// last hop from origin means next hop towards origin.
			newEntry.nextHop = lastHop;
			newEntry.rssi = rssi;
			routeRSSI.record(rssi);
			routeBER.record(ber);
			routeTable.insert(make_pair(origin, newEntry));
			if(useSimTracer) {
			  tracer->logLink(myNetwAddr, lastHop);
			}
			nbRoutesRecorded++;
			if (origin == 0)
				nextHopSelectionForSink.record(lastHop);
		}
	}
	else {
		// A route towards the node which originated the received packet already exists.
		// Replace its entry only if the route proposal that we just received has a stronger
		// RSSI.
		tRouteTableEntry entry = pos->second;
		if (entry.rssi > rssiThreshold) {
			entry.nextHop = lastHop;
			entry.rssi = rssi;
			if (origin == 0)
				nextHopSelectionForSink.record(lastHop);
		}
	}
}
