/***************************************************************************
 * file:        SensorApplLayer.h
 *
 * author:      Jerome Rousselot, Amre El-Hoiydi
 *
 * copyright:   (C) 2007-2009 CSEM SA, Neuchatel, Switzerland.
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

#ifndef SENSOR_APPL_LAYER_H
#define SENSOR_APPL_LAYER_H

#include <vector>
#include "BasicApplLayer.h"
#include "NetwControlInfo.h"
#include "Packet.h"
//#include "cstat.h"
#include "BaseMacLayer.h"

using namespace std;

/**
 * @brief Test class for the application layer
 *
 * All nodes periodically generate a packet addressed to a sink.
 * This class takes three arguments:
 * - packets: the number of packets to be sent by this application.
 * - trafficType: affects how the time interval between two packets
 * 				is generated. Possible values are "periodic", "uniform",
 * 				and "exponential".
 * - trafficParam: parameter for traffic type. For "periodic" traffic
 * 					this value is the constant time interval, for
 * 					"uniform" traffic this is the maximum time interval
 * 					between two packets and for "exponential" it is
 * 					the mean value. These values are expressed in seconds.
 *
 * @ingroup applLayer
 * @author Amre El-Hoiydi, Jerome Rousselot
 **/
class SensorApplLayer:public BasicApplLayer
{
public:


  /** @brief Initialization of the module and some variables*/
  virtual void initialize(int);
  virtual void finish();

  ~SensorApplLayer();

  enum APPL_MSG_TYPES
  {
    SEND_DATA_TIMER,
    STOP_SIM_TIMER,
    DATA_MESSAGE
  };

  enum TRAFFIC_TYPES
  {
    UNKNOWN=0,
    PERIODIC,
    UNIFORM,
    EXPONENTIAL
  };

protected:
  cMessage * delayTimer;
  int petitTest;
  int myAppAddr;
  int sentPackets;
  simtime_t initializationTime;
  simtime_t firstPacketGeneration;
  simtime_t lastPacketReception;
  // parameters:
  TRAFFIC_TYPES trafficType;
  double trafficParam;
  int nbPackets;
  long nbPacketsSent;
  long nbPacketsReceived;
  bool stats;
  bool debug;
  vector < cStdDev > latencies;
  cStdDev latency;
  cOutVector latenciesRaw;
  Packet packet; // informs the simulation of the number of packets sent and received by this node.
  int catPacket;
  int hostID;

protected:
  /** @brief Handle self messages such as timer... */
   virtual void handleSelfMsg(cMessage *);

  /** @brief Handle messages from lower layer */
  virtual void handleLowerMsg(cMessage *);

  /** @brief send a data packet to the next hop */
  virtual void sendData();

  /** @brief calculate time to wait before sending next packet, if required. */
  void scheduleNextPacket();
};

#endif
