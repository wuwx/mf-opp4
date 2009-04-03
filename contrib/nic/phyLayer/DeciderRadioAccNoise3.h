/* -*- mode:c++ -*- *******************************************************
 * file:        DeciderRadioAccNoise3.h
 *
 * authors:     Jerome Rousselot, Amre El-Hoiydi
 * 			    David Raguin / Marc Loebbers
 *
 * copyright:   (C) 2007-2009 CSEM SA, Neuchatel, Switzerland
 * 			    (C) 2004 Telecommunication Networks Group (TKN) at
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
 * part of:    Modifications to the MF-2 framework by CSEM
 **************************************************************************/


#ifndef  DECIDER_RADIOACCNOISE3_H
#define  DECIDER_RADIOACCNOISE3_H

#include <omnetpp.h>

#include <BasicLayer.h>
#include <AirFrameRadioAccNoise3_m.h>
#include "ConstsAccNoise3.h"
#include "RadioAccNoise3PhyControlInfo.h"
#include <math.h>
/**
 * @brief Decider for the Radio AccNoise3 Model
 *
 *
 *
 * @ingroup decider
 * @author Jerome Rousselot, Amre El-Hoiydi, Marc L�bbers, David Raguin
 */
class DeciderRadioAccNoise3:public BasicLayer
{


public:
    /** @brief Initialization of the module and some variables*/
  virtual void initialize(int);

  virtual void finish();
protected:
    /**
     * @brief Check whether we received the packet correctly.
     * Also appends the PhyControlInfo
     */
   virtual void handleLowerMsg(cMessage * msg);

    /**
     * @brief This function should not be called.
     */
  virtual void handleUpperMsg(cMessage * msg)
  {
    error("DeciderRadioAccNoise3 does not handle messages from upper layers");
  }

  virtual void handleSelfMsg(cMessage * msg)
  {
    error("DeciderRadioAccNoise3 does not handle selfmessages");
  }

    /** @brief Handle control messages from lower layer
     *  Just cut them through
     */
  virtual void handleLowerControl(cMessage * msg)
  {
    sendControlUp(msg);
  };

  cMessage *decapsMsg(AirFrameRadioAccNoise3 * frame);

    /** @brief converts a dB value into a normal fraction*/
  double dB2fraction(double);

    /** @brief computes if packet is ok or has errors*/
  bool packetOk(double, int, double bitrate);

  virtual double getBERFromSNR(double snr);

  /** @brief modulation type */
  const char * modulation;


protected:
    /** @brief should be set in the omnetpp.ini*/
  double bitrate;

	/** @brief Minimum bit error rate. If SNIR is high, computed ber could be
		higher than maximum radio performance. This value is an upper bound to
		the performance. */
  double BER_LOWER_BOUND;

  long nbFramesWithInterference;
  long nbFramesWithoutInterference;

  long nbFramesWithInterferenceDropped;
  long nbFramesWithoutInterferenceDropped;

  bool stats;



#ifdef _WIN32
    /**
     * @brief Implementation of the error function
     *
     * Unfortunately the windows math library does not provide an
     * implementation of the error function, so we use an own
     * implementation (Thanks to Jirka Klaue)
     *
     * @author Jirka Klaue
     */
  double erfc(double);
#endif


};
#endif
