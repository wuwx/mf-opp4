/***************************************************************************
 * file:        pairwise.cc
 *
 * author:      Jerome Rousselot
 *
 * copyright:   (C) 2007-2008 CSEM SA, Neuchatel, Switzerland.
 *
 * description: Implementation of the pairwise routing protocol
 *		Ref: "A Robust, Responsive, Distributed Tree-Based Routing
 *		Algorithm Guaranteeing N Valid Links per Node in Wireless
 *		Ad-Hoc Networks", Tunc Ikikardes, Markus Hofbauer,
 *		August Kaelin, Martin May, Proceedings of IEEE
 *		Symposium on Computers and Communications, IEEE, July, 2007.
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

#include <limits>
#include <algorithm>

#include "pairwise.h"
#include <cassert>

Define_Module(pairwise);

void pairwise::initialize(int stage)
{
  BasicLayer::initialize(stage);

  if(stage == 1) {
    arp = UbiquitousArpAccess().get();

    EV << "Host index=" << findHost()->getIndex() << ", Id="
    << findHost()->getId() << endl;

    cModule * macmodule = findHost()->getSubmodule("nic")->getSubmodule("mac");
    BaseMacLayer * ipif = dynamic_cast<BaseMacLayer *>(macmodule);


    EV << "  host IP address=" << ipif->getNETAddress() << endl;
    EV << "  host macaddress=" << ipif->getMACAddress() << endl;
    netaddress = ipif->getNETAddress();
    macaddress = ipif->getMACAddress();

    decisionDelay = par("decisionDelay"); // 60;
    ackDelay = par("ackDelay"); // 15;
    assocDelay = par("assocDelay"); // 15;
    iterationDelay = par("iterationDelay"); // 120
    sinkAddress = par("sinkAddress"); // 0
    headerLength = par ("headerLength");
    maxNoUpdates = par("MaxNoUpdates");

    PAIRWISE_ACKJOIN_LENGTH = 16;

    stats = par("stats");
    trace = par("trace");
    debug = par("debug");
    useSimTracer = par("useSimTracer");

    selectedMetric = par("metric");
    metricThreshold = par("metricThreshold");

    missingJoinAcks.setName("missingJoinAcks");
    ignoredJoinReqs.setName("ignoredJoinReqs");
    missingJoinReqs.setName("missingJoinReqs");
    iterationConflicts.setName("iterationConflicts");
    neighbourCosts.setName("neighbourCosts");
    costs.setName("costs");

    nodesInTree.setName("nodesInTree");

    usedAlternateFavorite = 0;
    nbDataPacketsForwarded = 0;
    nbDataPacketsReceived = 0;
    nbDataPacketsSent = 0;
    nbMissedJoinAcks = 0;
    nbInappropriateJoinRequests = 0;
    nbInappropriateJoinAcks = 0;
    nbIterationConflicts = 0;
    nbRxJoinAcks = 0;
    nbTxJoinAcks = 0;
    nbTxJoinReqs = 0;
    nbRxJoinReqs = 0;
    nbEarlyJoinRequests = 0;
    nbRxPropagate = 0;
    nbIgnoredJoinReqs = 0;
    nbTxSuccesses = 0;
    nbTxErrors = 0;
    nbJoinTxErrors = 0;
    nbIterationTxErrors = 0;

    nbNoUpdates = 0;
    currNodeState = ISOLATED;
    currFSMState = IDLE;

    iterationStep = 0;
    myParentInTree = 0;

    // The sink initiates the tree building procedure
    if(netaddress == sinkAddress) {
      EV << "transition 1 from fsm state Idle to WaitBeforeDecision." << endl;
      currNodeState = SINK;
      iterationTimer = new cMessage("iterationTimer");
      treeUpdated = true;
      // wait at least 1 second and then start first
      // iteration of the tree building procedure
      scheduleAt(simTime()+1+uniform(0,10), iterationTimer);
    }
    decisionTimer = new cMessage("decisionTimer");
    ackTimer = new cMessage("ackTimer");
    assocTimer = new cMessage("assocTimer");
  }

  if(useSimTracer) {
    // Get a handle to the tracer module
    const char *tracerModulePath = "sim.simTracer";
    cModule *modp = simulation.getModuleByPath(tracerModulePath);
    tracer = check_and_cast<SimTracer *>(modp);
    // log node position
    BasicMobility * mobility;
      mobility = check_and_cast<BasicMobility*>(getParentModule()->getSubmodule("mobility"));
    tracer->logPosition(netaddress, mobility->getX(), mobility->getY());
  }
}

void pairwise::handleSelfMsg(cMessage* msg) {
  if ( msg == iterationTimer ) {

    if(!treeUpdated && nbNoUpdates < maxNoUpdates) {
        nbNoUpdates++;
        treeUpdated = true;
    }

    if ( treeUpdated ) {
      // we are sink node and we should send a start_iteration message
      NSafeLinksPkt * iterMsg = new NSafeLinksPkt("ITERATION_START", ITERATION_START);
      iterationStep++;
      notInTree.clear();
      inTree.clear();
      iterMsg->setIteration(iterationStep);
      iterMsg->setSrcAddr(netaddress);
      iterMsg->setDestAddr(BaseMacLayer::NET_BROADCAST);
      iterMsg->setStatus(SINK);
      iterMsg->setTimeSinceIterationStart(0);
      iterMsg->setControlInfo(new MacControlInfo(BaseMacLayer::MAC_BROADCAST));
      iterMsg->setByteLength(PAIRWISE_ACKJOIN_LENGTH);
      EV << "Sink node starting new iteration " << iterationStep << "." << endl;
      sendDown(iterMsg);
      scheduleAt(simTime()+decisionDelay, decisionTimer);
      currFSMState = WAITDECISION;
      treeUpdated = false;
      // Schedule next iteration in ten minutes
      scheduleAt(simTime() + iterationDelay, iterationTimer);
    } else {
    // We are sink and no node was added to the tree during the
    // last iteration. We should stop.
    EV << "[sink] No new node added during last iteration."
      << "Tree is complete !" << endl;
    }
  } else if ( msg == decisionTimer ) {

    // the time has come to take a decision

    EV << "transition 4 from state WaitDecision to "
            << "pseudostate Connected ?"<< endl;

    if(currFSMState != WAITDECISION)
        EV << "Error ! current state is not waitdecision. This cannot be"
                << " transition 4. FSMState=" << currFSMState << "." << endl;

    if ( currNodeState == ISOLATED ) {
      // choose a parent
      EV << "transition 8 from pseudostate Connected ? to WaitRequest." << endl;
      if (inTree.size() > 0) {
        EV << "I know about " << neighbours.size() << " neighbours." << endl;
        vector<int>::iterator potPar;
        favoriteParent = inTree.front();
        EV << "Initial parent: " << favoriteParent << " with value "
           << neighbours[favoriteParent] << "." << endl;
        for( potPar = inTree.begin(); potPar != inTree.end(); potPar++) {
          EV << "Considering neighbour " << *potPar << " with value "
             << neighbours[*potPar] << endl;
          if ( neighbours[favoriteParent] > neighbours[*potPar] ) {
           favoriteParent = *potPar;
            EV << "I found a better neighbour ! Updating pointer to "
               << *potPar << endl;
          }
        }
        scheduleAt(simTime()+assocDelay, assocTimer);
        currFSMState = WAITREQUEST;
        EV << "state WAITREQUEST, favorite parent in tree is "
        << favoriteParent << endl;
      } else {
        EV << "I don't know any neighbour part of the tree ! I don't have a favorite one."<<endl;
        currFSMState =WAITREQUEST;
        favoriteParent = -1;
      }

    } else if ( notInTree.size() > 0) {
      // choose a child
      EV << "transition 5 from pseudostate Connected ? to WaitReply." << endl;
      currFSMState = WAITREPLY;
      vector<int>::iterator potChild;
      selectedChild = notInTree.front();
      for(potChild = notInTree.begin(); potChild != notInTree.end(); potChild++)
        if ( neighbours[selectedChild] > neighbours[*potChild] )
          selectedChild = *potChild;
      // send it a join request
      NSafeLinksPkt * joinReq = new NSafeLinksPkt("JOIN_REQUEST", JOIN_REQUEST);
      joinReq->setIteration(iterationStep);
      joinReq->setSrcAddr(netaddress);
      joinReq->setDestAddr(selectedChild);
      joinReq->setControlInfo(new MacControlInfo(selectedChild));
      joinReq->setByteLength(PAIRWISE_ACKJOIN_LENGTH);
      EV << "State WAITREPLY, my favorite child is " << selectedChild
      << ", I'll propose him to join me." << endl;
      sendDown(joinReq);
      nbTxJoinReqs++;
      scheduleAt(simTime()+ackDelay, ackTimer);
    }
  } else if ( msg == ackTimer ) {
      nbMissedJoinAcks++;
      if(trace) {
          missingJoinAcks.record(selectedChild);

      }
    // my request was ignored because I was not the favorite parent.
    EV << "transition 6 from FSM state WaitReply to Idle." << endl;
    EV << "My join request was ignored. I was probably not "
      << " the favorite parent." << endl;
    currFSMState = IDLE;
  } else if ( msg == assocTimer ) {
      EV << "transition 10 from FSM State WaitRequest to Idle." << endl;
      EV << "I did not receive a join request from my favorite parent." << endl;
      currFSMState = IDLE;
  }
}

int pairwise::cost(int addr_i, int addr_j) {
  return 1;
}

/**
 * Decapsulates the packet from the received Network packet
 **/
cMessage* pairwise::decapsMsg(NetwPkt *msg)
{
  cMessage *m = msg->decapsulate();
  m->setControlInfo(new NetwControlInfo(msg->getSrcAddr()));
  // delete the netw packet
  delete msg;
  return m;
}


/**
 * Encapsulates the received ApplPkt into a NetwPkt and set all needed
 * header fields.
 **/
NSafeLinksPkt* pairwise::encapsMsg(cMessage *msg) {
  unsigned int finalDestNetwAddr;
  unsigned int nextHopNetwAddr;
  unsigned long nextHopMacAddr;
  EV <<"in encaps...\n";

  NSafeLinksPkt *pkt = new NSafeLinksPkt(msg->getName(), DATA);
  pkt->setByteLength(headerLength);
  NetwControlInfo* cInfo = dynamic_cast<NetwControlInfo*>(msg->removeControlInfo());

  if ( cInfo == 0 ) {
    EV << "warning: Application layer did not specifiy a destination L3 address\n"
       << "\tusing broadcast address instead\n";
    //finalDestNetwAddr = L3BROADCAST;
    finalDestNetwAddr = BaseMacLayer::NET_BROADCAST;
  } else {
	  EV <<"CInfo removed, netw addr="<< cInfo->getNetwAddr() <<endl;
	  finalDestNetwAddr = cInfo->getNetwAddr();
	  delete cInfo;
  }

  pkt->setSrcAddr(netaddress);
  pkt->setInitialSrcAddr(netaddress);
  pkt->setDestAddr(finalDestNetwAddr);

  EV << " netw "<< netaddress << " sending packet to network host "
  << finalDestNetwAddr << "." <<endl;
  if(finalDestNetwAddr == BaseMacLayer::NET_BROADCAST) {
	  // broadcast
    EV << "sendDown: nHop=L3BROADCAST -> message has to be broadcasted"
       << " -> set destMac=L2BROADCAST\n";
    nextHopMacAddr = BaseMacLayer::MAC_BROADCAST;
    nextHopNetwAddr = finalDestNetwAddr;
  } else {
      //Unicast: find next hop
      nextHopNetwAddr = getRoute(finalDestNetwAddr);
  }

  nextHopMacAddr = arp->getMacAddr(nextHopNetwAddr);
  pkt->setDestAddr(nextHopNetwAddr);

    EV << " routing packet at src host index=" << findHost()->getIndex()
       << ", netaddress=" << netaddress << ", nextHopNetwAddr="
       << nextHopNetwAddr << ", nextHopMacAddr=" << nextHopMacAddr
       << ", finalDestNetwAddr=" << finalDestNetwAddr << "\n";


  pkt->setControlInfo(new MacControlInfo(nextHopMacAddr));
  //encapsulate the application packet
  assert(static_cast<cPacket*>(msg));
  pkt->encapsulate(static_cast<cPacket*>(msg));
  return pkt;
}

void pairwise::handleLowerMsg(cMessage* msg) {
    int nextHopNetwAddr;
    unsigned long nextHopMacAddr;
    int finalDestNetwAddr;
    int initSrcAddr;
    NSafeLinksPkt * tellParent;
    double ber = 0.9;
    double costEstimation=0;

    NSafeLinksPkt *m = check_and_cast<NSafeLinksPkt *>(msg);
    NetwPkt * netwMsg = check_and_cast<NetwPkt *>(msg);

    EV << "Received a radio message of kind " << m->getKind()
    << "[fsm="<< currFSMState << ", node="
    << currNodeState << "]" << endl;

    if(m->getKind() != DATA) {
        if(m->getIteration() < iterationStep) {
            nbIterationConflicts++;
            if(trace) {
                iterationConflicts.record(m->getSrcAddr());
            }
        }
    }

    switch ( m->getKind() ) {

        case ITERATION_START:

            if ( iterationStep <  m->getIteration() ) {
                // we must repeat this packet
                EV << "transition 2 from fsm state Idle to state WaitDecision."
                        << endl;
                EV << "updating iteration step from " << iterationStep
                << " to " << m->getIteration() <<  " and repeating the message."
                << endl;

                // reset iteration variables
                inTree.clear();
                notInTree.clear();

                // prepare and send ITERATION_START packet
                iterationStep = m->getIteration();
                simtime_t offset = m->getArrivalTime() - m->getCreationTime() + m->getTimeSinceIterationStart() - 0.001;
                NSafeLinksPkt *mdup = (NSafeLinksPkt *) m->dup();
                mdup->setSrcAddr(netaddress);
                mdup->setControlInfo(new MacControlInfo(BaseMacLayer::MAC_BROADCAST));
                mdup->setStatus(currNodeState);
                mdup->setTimeSinceIterationStart(offset.dbl());
                if(currNodeState == ISOLATED) {
                  mdup->setMetric(std::numeric_limits<double>::max());
                  if(iterationStep > 1) {
                      // my favorite parent didn't send me a join request
                      if(trace) {
                        missingJoinReqs.record(favoriteParent);
                      }
                  }
                } else if (currNodeState == SINK) {
                  mdup->setMetric(0);
                } else {
                  mdup->setMetric(neighbours[myParentInTree]);
                }
                //mdup->setNbPotentialParents(XX);
                sendDown(mdup);
                // clear iteration variables
                notInTree.clear();
                inTree.clear();

                scheduleAt(simTime() + decisionDelay - offset, decisionTimer);
                currFSMState = WAITDECISION;
            } else {
                EV << "transition 3 from fsm state WaitDecision to WaitDecision."
                        << endl;
            }

            // compute link cost and store it

            ber = static_cast<MacControlInfo*>(m->getControlInfo())
            			->getBER();

            // RSSI-based metric:
            //double rssi = static_cast<MacControlInfo*>(m->getControlInfo())->getRSSI();
            //costEstimation= m->getMetric() + rssi;

            switch(selectedMetric) {
                case BER:
                    // BER-based metric:
                    costEstimation = m->getMetric() +
                            1/pow(ber, (double) m->getBitLength());
                    break;
                case HOPCOUNT:
                default:
                    // hop-based metric:
                    if(ber < metricThreshold) { // threshold to avoid bad links
                        costEstimation = m->getMetric() + 1;
                        neighbours[m->getSrcAddr()] = costEstimation;
                    }
                    break;
            }


            if(trace) {
                neighbourCosts.record(m->getSrcAddr());
                costs.record(costEstimation);
            }

            EV << "Updated neighbour " << m->getSrcAddr()
               << " with weight " << neighbours[m->getSrcAddr()] << "." << endl;

            if ( m->getStatus() == IN_TREE || m->getStatus() == SINK) {
                // add source to potential parents if new
                vector<int>::iterator pos = find(inTree.begin(), inTree.end(), m->getSrcAddr());
                if(pos == inTree.end()) {
                	inTree.push_back(m->getSrcAddr());
                	EV << "Added IN_TREE node " << m->getSrcAddr() << " to my list"
                	<< " of potential parents." << endl;
                	EV << "My IN_TREE list has now " << inTree.size()
                   << " items." << endl;
		}

            } else if ( m->getStatus() == ISOLATED ) {
                // add source to potential children if new
                vector<int>::iterator pos = find(notInTree.begin(), notInTree.end(), m->getSrcAddr());
                if(pos == notInTree.end()) {
                	notInTree.push_back(m->getSrcAddr());
                	EV << "Added ISOLATED node " << m->getSrcAddr() << " to my list"
                	<< " of potential children." << endl;
                	EV << "My ISOLATED list has now " << notInTree.size()
                	<< " items." << endl;
				}
            }
            delete m;
            break;

        case JOIN_REQUEST:

            nbRxJoinReqs++;
            if (currFSMState == WAITDECISION) {
                nbEarlyJoinRequests++;
            } else if (currFSMState == WAITREQUEST) {
                // check if the request comes from my favorite parent
                // or from an alternative favorite parent (with same cost)
                // if so, send an ACK_JOIN

                if (netwMsg->getSrcAddr() != favoriteParent &&
                        neighbours[netwMsg->getSrcAddr()] > neighbours[favoriteParent]) {
                    EV << "transition 11 from fsm state WaitRequest "
                            << "to state WaitRequest (ignoring JOIN_REQUEST "
                            << " from node " << netwMsg->getSrcAddr() << " since my"
                            << " favorite parent is node " << favoriteParent
                            << "." << endl;
                    nbIgnoredJoinReqs++;
                    if(trace) {
                        ignoredJoinReqs.record(netwMsg->getSrcAddr());
                    }
                } else {
                    EV << "transition 9 from fsm state WaitRequest to state Idle."
                            << endl;
                    if (favoriteParent != netwMsg->getSrcAddr()) {
                        favoriteParent = netwMsg->getSrcAddr();
                        usedAlternateFavorite = 1;
                    }
                    myParentInTree = favoriteParent;
                    currNodeState = IN_TREE;
                    currFSMState = IDLE;
                    cancelEvent(assocTimer);
                    NSafeLinksPkt * ackJoin = new NSafeLinksPkt("ACK_JOIN", ACK_JOIN);
                    ackJoin->setIteration(iterationStep);
                    ackJoin->setSrcAddr(netaddress);
                    ackJoin->setDestAddr(favoriteParent);
                    ackJoin->setControlInfo(new MacControlInfo(favoriteParent));
                    ackJoin->setByteLength(PAIRWISE_ACKJOIN_LENGTH);
                    EV << "State WAITREQUEST, my favorite parent " << favoriteParent
                            << " invited me to join him. Sending an ack." << endl;
                    sendDown(ackJoin);
                    nbTxJoinAcks++;
                }
            } else {
                nbInappropriateJoinRequests++;
            }

            delete m;
            break;

        case ACK_JOIN:
            if (currFSMState == WAITREPLY) {
                EV << "transition 7 from fsm state WaitReply "
                        << "to state Idle by receiving an ack_join from node "
                        << netwMsg->getSrcAddr() << "." << endl;
                children.push_back(netwMsg->getSrcAddr());
                subTree[netwMsg->getSrcAddr()] = netwMsg->getSrcAddr();
                treeUpdated = true;
                cancelEvent(ackTimer);
                nbRxJoinAcks++;
                if(useSimTracer) {
                  tracer->logLink(netaddress, netwMsg->getSrcAddr());
                }

                if (currNodeState != SINK) {
                    tellParent = new NSafeLinksPkt("PROPAGATE", PROPAGATE);
                    tellParent->setIteration(iterationStep);
                    tellParent->setInitialSrcAddr(netwMsg->getSrcAddr());
                    tellParent->setFinalDestAddr(sinkAddress);
                    tellParent->setSrcAddr(netaddress);
                    tellParent->setDestAddr(myParentInTree);
                    tellParent->setControlInfo(new MacControlInfo(myParentInTree));
                    tellParent->setByteLength(PAIRWISE_ACKJOIN_LENGTH);
                    sendDown(tellParent);
                } else if(stats) {
                	nodesInTree.record(netwMsg->getSrcAddr());
                }
                currFSMState = IDLE;
            } else {
                nbInappropriateJoinAcks++;
            }
            delete m;
            break;
        case PROPAGATE:
            // message must reach sink to tell it that a new node joined tree.
            treeUpdated = true;
            nbRxPropagate++;
            EV << "There is a new node in my subtree with address "
               << m->getInitialSrcAddr() << ". It can be reached through "
               << m->getSrcAddr() << "." << endl;
            subTree[netwMsg->getInitialSrcAddr(), netwMsg->getSrcAddr()];
            if(currNodeState != SINK) {
              m->setDestAddr(myParentInTree);
              m->removeControlInfo();
              m->setControlInfo(new MacControlInfo(myParentInTree));
              sendDown(m);
            } else {
              delete m;
            }
            break;
        case DATA:
        default:
            routeDataPacket(m);
            break;
        /*
        default:
            EV << "Unknown message type received from lower layer !" << endl;
            delete m;
            break;
            * */
    }

}

void pairwise::handleLowerControl(cMessage *msg) {
    /*
	switch(msg->getKind()) {
        case wisemac::TXOVER:
            nbTxSuccesses++;
            break;
        case wisemac::TXFAIL:
        case wisemac::TXDROP:
            nbTxErrors++;
            if(currFSMState == WAITDECISION)
                nbIterationTxErrors++;
            else if(currFSMState == WAITREPLY)
                nbJoinTxErrors++;
            break;
    }
    */
    delete msg;
}

int pairwise::getRoute(int finalDestNetwAddr) {
  if ( subTree.find(finalDestNetwAddr) == subTree.end() ) {
    EV << "No route to " << finalDestNetwAddr << " from "
       << netaddress << ", forwarding to my parent in tree." << endl;
    return myParentInTree; // route to sink
  } else {
    EV << "I know a route to " << finalDestNetwAddr << " from "
       << netaddress << ", sending packet to "
       << subTree[finalDestNetwAddr] << "." << endl;
    return subTree[finalDestNetwAddr];
  }
}

void pairwise::routeDataPacket(NSafeLinksPkt * m) {

  unsigned int nextHopNetwAddr;
  unsigned long nextHopMacAddr;
  unsigned int finalDestNetwAddr;
  unsigned int initSrcAddr;

  finalDestNetwAddr=m->getFinalDestAddr();
  initSrcAddr=m->getSrcAddr();
  if (finalDestNetwAddr == netaddress ||
       finalDestNetwAddr == BaseMacLayer::NET_BROADCAST) {
    // packet has arrived
    EV << " routing packet at dest,  from " << initSrcAddr << " to "
    << finalDestNetwAddr << "sending up" << endl;
    sendUp(decapsMsg(m));
    nbDataPacketsReceived++;
    return;
  } else {
    // routing
    nextHopNetwAddr = getRoute(finalDestNetwAddr);
  }

  nextHopMacAddr = arp->getMacAddr(nextHopNetwAddr);

  EV << " routing packet fwd " << initSrcAddr << " to "
     << finalDestNetwAddr << " next hop netw " << nextHopNetwAddr
     << " next hop mac" << nextHopMacAddr << endl;

  // rewrite hop information
  m->setSrcAddr(netaddress);
  m->setDestAddr(nextHopNetwAddr);
  // ask MAC layer to transmit it
  m->removeControlInfo();
  m->setControlInfo(new MacControlInfo(nextHopMacAddr));
  sendDown(m);
  nbDataPacketsForwarded++;
}

/**
 * Redefine this function if you want to process messages from upper
 * layers before they are send to lower layers.
 *
 * For the Dijkstra we just use the destAddr of the network
 * message as a nextHop
 *
 * To forward the message to lower layers after processing it please
 * use @ref sendDown. It will take care of anything needed
 **/
void pairwise::handleUpperMsg(cMessage* msg)
{
  sendDown(encapsMsg(msg));
  nbDataPacketsSent++;
}

void pairwise::finish()
{
  cancelAndDelete(ackTimer);
  cancelAndDelete(decisionTimer);
  cancelAndDelete(assocTimer);
  if(currNodeState == SINK)
    cancelAndDelete(iterationTimer);
  if(stats) {
    recordScalar("nbDataPacketsForwarded", nbDataPacketsForwarded);
    recordScalar("nbDataPacketsReceived", nbDataPacketsReceived);
    recordScalar("nbDataPacketsSent", nbDataPacketsSent);
    recordScalar("parentInTree", myParentInTree);
    recordScalar("usedAlternateFavorite", usedAlternateFavorite);

    recordScalar("nbNeighbours", neighbours.size());
    recordScalar("nbChildren", children.size());
    recordScalar("nbEarlyJoinRequests", nbEarlyJoinRequests);
    recordScalar("nbIterations", iterationStep);
    recordScalar("nbMissedJoinAcks", nbMissedJoinAcks);
    recordScalar("nbRxJoinAcks", nbRxJoinAcks);
    recordScalar("nbTxJoinAcks", nbTxJoinAcks);
    recordScalar("nbRxJoinReqs", nbRxJoinReqs);
    recordScalar("nbTxJoinReqs", nbTxJoinReqs);
    recordScalar("nbRxPropagate", nbRxPropagate);
    recordScalar("nbIgnoredJoinReqs", nbIgnoredJoinReqs);
    recordScalar("nbInappropriateJoinRequests", nbInappropriateJoinRequests);
    recordScalar("nbInappropriateJoinAcks", nbInappropriateJoinAcks);
    recordScalar("nbIterationConflicts", nbIterationConflicts);
    recordScalar("nbTxSuccesses", nbTxSuccesses);
    recordScalar("nbTxErrors", nbTxErrors);
    recordScalar("nbJoinTxErrors", nbJoinTxErrors);
    recordScalar("nbIterationTxErrors", nbIterationTxErrors);
  }
  inTree.clear();
  notInTree.clear();
  neighbours.clear();
}


