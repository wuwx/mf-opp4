package org.mf2.contrib.nic.macLayer;

//***************************************************************************
//* file:        BaseMacLayer.ned
//*
//* author:      Jerome Rousselot
//* 
//* copyright:   (C) 2007-2009 CSEM SA, Neuchatel, Switzerland.
//*
//*              This program is free software; you can redistribute it
//*              and/or modify it under the terms of the GNU General Public
//*              License as published by the Free Software Foundation; either
//*              version 2 of the License, or (at your option) any later
//*              version.
//*              For further information see file COPYING
//*              in the top level directory
//*
//* Funding: This work was partially financed by the European Commission under the  
//* Framework 6 IST Project "Wirelessly Accessible Sensor Populations"  
//* (WASP) under contract IST-034963.
//***************************************************************************
//* part of:    Modifications to the MF-2 framework by CSEM
//**************************************************************************/

simple BaseMacLayer
{
    parameters:
        int netaddress; // Network address
        int macaddress = default(netaddress); // MAC address
}
