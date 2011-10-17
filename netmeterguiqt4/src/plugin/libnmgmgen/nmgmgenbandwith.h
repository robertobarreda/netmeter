/***************************************************************************
 *   Copyright (C) 2005 by Ernest Nieto                                    *
 *   ernest.nieto@gmail.com                                                *
 *   Copyright (C) 2007 by Joan Sala                                       *
 *   jsala@ac.upc.edu                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef NMGMGENBANDWITH_H
#define NMGMGENBANDWITH_H


#define ETH_PAYLOAD_LLC  	18
#define ETH_IP4_HDR      	20
#define ETH_IP6_HDR      	40
#define ETH_UDP_HDR      	8


#define ATM_PAYLOAD_ATM  	48
#define ATM_MIDA_ATM     	53
#define ATM_PAYLOAD_AAL5 	8
#define ATM_PAYLOAD_HALF_AAL5 	4
#define ATM_PAYLOAD_LLC  	8
#define ATM_IP4_HDR      	20
#define ATM_IP6_HDR      	40
#define ATM_UDP_HDR      	8


/**
@author Ernest Nieto
*/
class NMGMGenBandwith
{
private:
    NMGMGenBandwith();

    ~NMGMGenBandwith();

public:
    static int getATMBandwith(int rate, int packetSize, bool isIPv6=false, bool isRaw=false,
		bool isBPSatIPlevel=false, bool isLinkLevel=false, bool countHeaderATM=true,
		bool isKbps=true);

    static int getEthernetBandwith(int rate, int packetSize, bool isIPv6=false, bool isRaw=false,
		bool isBPSatIPlevel=false, bool isKbps=true);

};

#endif
