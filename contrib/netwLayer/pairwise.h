/***************************************************************************
 * file:        pairwise.h
 *
 * author:      Jérôme Rousselot
 *
 * copyright:   (C) 2007-2008 CSEM SA, Neuchâtel, Switzerland.
 *
 * description: Implementation of the pairwise routing protocol
 *		Ref: "A Robust, Responsive, Distributed Tree-Based Routing
 *		Algorithm Guaranteeing N Valid Links per Node in Wireless
 *		Ad-Hoc Networks", Tunc Ikikardes, Markus Hofbauer,
 *		August Kaelin, Martin May, Proceedings of IEEE
 *		Symposium on Computers and Communications, IEEE, July, 2007.
 **************************************************************************
 * part of:     Modifications to the MF Framework by CSEM
 **************************************************************************/

#ifndef pairwise_H
#define pairwise_H

#include <omnetpp.h>

#include <BasicLayer.h>
#include <BasicMobility.h>
#include <fstream>
#include "DijkstraRoutes.h"
#include "UbiquitousArp.h"
#include "NSafeLinksPkt_m.h"
#include "MacPkt_m.h"
#include "BaseMacLayer.h"
#include "SimTracer.h"
#include "NetwControlInfo.h"
#include "MacControlInfo.h"

#include <map>
#include <list>
#include <math.h>

using namespace std;

/**
 * @brief This class offers routing services using pairwise
 * algorithm to build N redundant spanning trees with the sink node
 * as root node.
 *
 * @ingroup netwLayer
 * @author Jérôme Rousselot
 **/
class pairwise : public BasicLayer
{

  protected:
    /**
     * @brief Length of the NetwPkt header
     * Read from omnetpp.ini
     **/
    int headerLength;

    /** @brief Pointer to the arp module*/
    UbiquitousArp* arp;

    /** @brief cached variable of my network address */
    int netaddress;
    int macaddress;
    map<int,double> neighbours;
    vector<int> inTree, notInTree, children;
    map<int, int> subTree;
    int myParentInTree, favoriteParent, sinkAddress;

    bool useSimTracer;

    SimTracer *tracer;

    long nbDataPacketsForwarded;
    long nbDataPacketsReceived;
    long nbDataPacketsSent;
    long nbIgnoredJoinReqs;
    long nbInappropriateJoinRequests;
    long nbInappropriateJoinAcks;
    long nbIterationConflicts;
    long nbRxPropagate;
    long nbTxErrors;
    long nbTxSuccesses;
    long nbIterationTxErrors;
    long nbJoinTxErrors;
    bool trace, stats, debug;
    int usedAlternateFavorite;

    cOutVector missingJoinAcks;
    cOutVector ignoredJoinReqs;
    cOutVector missingJoinReqs;
    cOutVector iterationConflicts;
    cOutVector neighbourCosts;
    cOutVector costs;
    cOutVector nodesInTree;

    int nbNoUpdates;
    int maxNoUpdates;
    int selectedMetric;
    double metricThreshold;

    int PAIRWISE_ACKJOIN_LENGTH;

public:


    /** @brief Initialization of the module and some variables*/
    virtual void initialize(int);

    virtual void finish();


  protected:
    /**
     * @name Handle Messages
     * @brief Functions to redefine by the programmer
     *
     * These are the functions provided to add own functionality to your
     * modules. These functions are called whenever a self message or a
     * data message from the upper or lower layer arrives respectively.
     *
     **/
    /*@{*/

    /** @brief Handle messages from upper layer */
    virtual void handleUpperMsg(cMessage* msg);

    /** @brief Handle messages from lower layer */
    virtual void handleLowerMsg(cMessage* msg);

    /** @brief Handle self messages */
    virtual void handleSelfMsg(cMessage* msg);

    /** @brief Handle control messages from lower layer */
    virtual void handleLowerControl(cMessage* msg);


    /*@}*/


    enum messagesTypes {
      UNKNOWN=0,
      ITERATION_START,
      JOIN_REQUEST,
      ACK_JOIN,
      PROPAGATE,
      DEBUG,
      DATA
    };

    enum metrics {
        BER=1,
        HOPCOUNT
    };

    enum nodeStates {
      SINK=0,
      IN_TREE,
      ISOLATED
    };

    enum fsmStates {
      NONE = 0,
      IDLE,
      WAITDECISION,
      WAITREPLY,
      WAITREQUEST
    };

    int currFSMState;
    bool treeUpdated;
    int  currNodeState;

    int selectedChild;

    // stats counters
    int nbEarlyJoinRequests;
    int nbMissedJoinAcks;
    int nbRxJoinAcks;
    int nbTxJoinAcks;
    int nbTxJoinReqs;
    int nbRxJoinReqs;
    // sink timer to synchronize iterations
    cMessage * iterationTimer;
    // timer to trigger decision on best (parent/child)
    cMessage * decisionTimer;
    // timer to wait for join ack from child after sending a join request
    cMessage * ackTimer;
    // timer to wait for join request from favorite parent before going
    // back to idle state.
    cMessage * assocTimer;

    simtime_t decisionDelay;
    simtime_t ackDelay;
    simtime_t assocDelay;
    simtime_t iterationDelay;

    // current iteration step during initialization
    int iterationStep;

    /** @brief Compute weight of edge between nodes i and j */
    virtual int cost(int addr_i, int addr_j);

    /** @brief decapsulate higher layer message from NetwPkt */
    virtual cMessage* decapsMsg(NetwPkt*);

    /** @brief Encapsulate higher layer packet into an NetwPkt*/
    virtual NSafeLinksPkt* encapsMsg(cMessage*);

    void routeDataPacket(NSafeLinksPkt * m);

    int getRoute(int finalDestNetwAddr);


};

#endif
