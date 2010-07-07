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

package org.mf2.networks.energy;

import org.mf2.contrib.nic.macLayer.BaseMacLayer;

//
// TODO auto-generated module
//
simple EnergyMacLayer extends BaseMacLayer
{
    parameters:
		@class(EnergyMacLayer);
        // debug switch
        bool debug = default(false);
        bool stats = default(true);
        bool trace = default(false);
        // level at which we consider medium busy (in dBm)
        double busyRSSI @unit(dBm) = default(-97 dBm);

        // simple interframe space. Time to wait between receiving a frame and acknowledging it.
        // Should be bigger than the maximum time for switching between Tx and Rx at the receiver.
        double sifs @unit(s) = default(0.000192 s); // 12 symbols
        // length of MAC header
        double headerLength @unit(bit) = default(72 bit);
        // size of the MAC queue (maximum number of packets in Tx buffer)
        double queueLength = default(100);
        // default channel 
        double defaultChannel = default(0);
        // bit rate 
        double bitrate @unit(bps) = default(250000 bps);
        // complete MAC ack message length (in bits) 
        // (! headerLength is not added to this)
        double ackLength @unit(bit) = default(40 bit);
        // minimum backoff exponent
        double macMinBE = default(3);
        // maximum backoff exponent
        double macMaxBE = default(8);
        // maximum backoffs
        double macMaxCSMABackoffs = default(5);
        // maximum number of frame retransmission
        double macMaxFrameRetries = default(3);
        // base unit for all backoff calculations
        double aUnitBackoffPeriod @unit(s) = default(0.00032 s);
        // time to wait for an acknowledgement after transmitting a unicast frame.
        double macAckWaitDuration @unit(s) = default(0.00056 s);  // 1+12+10+12 symbols
        // Clear Channel Assessment detection time
        double ccaDetectionTime @unit(s) = default(0.000128 s); // 8 symbols
        // Time to setup radio to reception state
        double rxSetupTime @unit(s);
        // Time to switch radio from Rx to Tx state
        double aTurnaroundTime @unit(s) = default(0.000192 s);    // 12 symbols

    gates:
        input uppergateIn;
        output uppergateOut;
        input lowergateIn;
        output lowergateOut;
        output upperControlOut; // for control messages to upper layer
        input lowerControlIn;   // for control messages from phy layer
}