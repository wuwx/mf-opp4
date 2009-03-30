/***************************************************************************
 * file:        WiseRoute.h
 *
 * author:      Damien Piguet, Jérôme Rousselot
 *
 * copyright:   (C) 2007-2009 CSEM SA, Neuchâtel, Switzerland.
 *
 * description: Implementation of the routing protocol of WiseStack.
 *
 **************************************************************************
 * part of:     Modifications to the MF Framework by CSEM
 **************************************************************************/

#ifndef wiseroute_h
#define wiseroute_h

#include <omnetpp.h>

#include <BasicLayer.h>
#include <BasicMobility.h>
#include <fstream>
#include "UbiquitousArp.h"
#include "WiseRoutePkt_m.h"
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
class WiseRoute : public BasicLayer
{
public:
    /** @brief Initialization of the module and some variables*/
    virtual void initialize(int);
    virtual void finish();

    ~WiseRoute();

protected:
	enum messagesTypes {
	    UNKNOWN=0,
	    DATA,
	    ROUTE_FLOOD,
	    SEND_ROUTE_FLOOD_TIMER
	};

	typedef enum floodTypes {
		NOTAFLOOD,
		FORWARD,
		FORME,
		DUPLICATE
	} floodTypes;


	typedef struct tRouteTableEntry {
		int nextHop;
		double rssi;
	} tRouteTableEntry;

	typedef map<int, tRouteTableEntry> tRouteTable;
	typedef multimap<int, unsigned long> tFloodTable;

	tRouteTable routeTable;
	tFloodTable floodTable;

    /**
     * @brief Length of the NetwPkt header
     * Read from omnetpp.ini
     **/
    int headerLength;

    /** @brief Pointer to the arp module*/
    UbiquitousArp* arp;

    /** @brief cached variable of my network address */
    int myNetwAddr;
    int macaddress;

    int sinkAddress;

	bool useSimTracer;

    /** @brief Minimal received RSSI necessary for adding source to routing table. */
    double rssiThreshold;

    /** @brief Interval [seconds] between two route floods. A route flood is a simple flood from
     *         which other nodes can extract routing (next hop) information.
     */
    double routeFloodsInterval;

    /** @brief Flood sequence number */
    unsigned long floodSeqNumber;

    SimTracer *tracer;
    cMessage* routeFloodTimer;

    long nbDataPacketsForwarded;
    long nbDataPacketsReceived;
    long nbDataPacketsSent;
    long nbDuplicatedFloodsReceived;
    long nbFloodsSent;
    long nbPureUnicastSent;
    long nbRouteFloodsSent;
    long nbRouteFloodsReceived;
    long nbUnicastFloodForwarded;
    long nbPureUnicastForwarded;
    long nbGetRouteFailures;
    long nbRoutesRecorded;

    cOutVector receivedRSSI;
    cOutVector routeRSSI;
    cOutVector allReceivedRSSI;
    cOutVector allReceivedBER;
    cOutVector routeBER;
    cOutVector receivedBER;
    cOutVector nextHopSelectionForSink;

    bool trace, stats, debug;

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

    /** @brief Update routing table.
     *
     * The tuple provided in argument gives the next hop address to the origin.
     * The table is updated only if the RSSI value is above the threshold.
     */
    virtual void updateRouteTable(int origin, int lastHop, double rssi, double ber);

    /** @brief Decapsulate a message */
    cMessage* decapsMsg(WiseRoutePkt *msg);

    /** @brief update flood table. returns detected flood type (general or unicast flood to forward,
     *         duplicate flood to delete, unicast flood to me
     */
    floodTypes updateFloodTable(bool isFlood, int srcAddr, int destAddr, unsigned long seqNum);

    /** @brief find a route to destination address. */
    int getRoute(int destAddr, bool iAmOrigin = false);
};

#endif
