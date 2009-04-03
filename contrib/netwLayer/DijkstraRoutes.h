/***************************************************************************
 * file:        DijkstraRoutes.h
 *
 * author:      Jerome Rousselot
 *
 * copyright:   (C) 2007 CSEM
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
 * description: Implementation of a precomputed routing layer
 **************************************************************************/

#ifndef DIJKSTRAROUTES_H_
#define DIJKSTRAROUTES_H_


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

/**
 * @brief This class maintains a routing table of shortest paths
 * to a node.
 * Current implementation assumes that destination address is always 0.
 * It reads route information from a text file containing on each line
 * three network addresses (as decimal integers) separated by a space.
 * First number is current node, next number is final destination
 * (always zero) and third number is next hop information.
 *
 * TODO:
 *  - Call Dijkstra algorithm (from Boost library) at initialization time
 *    and use positions information instead of relying on an external file.
 *
 *
 * @ingroup netwLayer
 * @author Jerome Rousselot
 **/

class DijkstraRoutes
{
public:
	/*
	 * Default constructor.
	 */
	DijkstraRoutes();

  ~DijkstraRoutes();

  /*
   * @brief Load routes information from a text file.
   */
  void loadRoutes(const char * routesFile);
  /*
   * @brief Vector of routes. Each route is a vector of integers (network addresses).
   * Each route contains two addresses. The first one is the current hop, the second
   * one is the next hop to reach address 0.
   * TODO: optimize data structure and also allows addressing more than one node.
   */
  vector <vector<int> > routesVector;

public:
	/*
	 * @brief Returns the next hop network address to reach node finalDest from node
	 * src. Currently always assumes that finalDest = 0.
	 */
  int getRoute (int src, int finalDest);

  //static DijkstraRoutes& getInstance();

protected:
	bool ready;
};


#endif /*DIJKSTRAROUTES_H_*/
