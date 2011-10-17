/***************************************************************************
 *   Copyright (C) 2007 by Roberto Barreda <rbarreda@ac.upc.edu>           *
 *   Copyright (C) 2005 by Ernest Nieto <ernest.nieto@gmail.com>           *
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
#ifndef NMGSTATION_H
#define NMGSTATION_H

#include "nmghost.h"
#include "nmgstationfieldsmanager.h"

#define TAG_STATION		"station"
#define TAG_STATION_TEST	"test"
#define TAG_STATION_MANAGEMENT	"management"
#define TAG_STATION_ADDRESS	"address"
#define TAG_STATION_ALIAS	"alias"
#define TAG_STATION_PORT	"port"
#define TAG_STATION_PROTOCOL	"protocol"
#define TAG_STATION_USERNAME	"username"
#define TAG_STATION_PASSWORD	"password"

#define STATION_PROTOCOL_IPV4	"IPv4"
#define STATION_PROTOCOL_IPV6	"IPv6"

class NMGStation
{
	public:
		NMGStation();
		~NMGStation();
		QString getManagementPassword();
		void setManagementPassword ( QString pass );
		QString getManagementUsername();
		void setManagementUsername ( QString username );
		int getManagementAddressPort();
		void setManagementAddressPort ( int port );
		NMGStationFieldsManager & getStationFieldsManager();
		NMGHost & getManagementHost();
		NMGHost & getTestHost();
		void addXMLStationNode ( QDomDocument& doc, QDomElement& stationsElement );
		static NMGStation * makeStationFromXMLElement ( QDomElement& element );

	private:
		NMGHost managementHost;
		NMGHost testHost;
		int managementAddressPort;
		QString managementPassword;
		QString managementUsername;
		NMGStationFieldsManager stationFieldsManager;
};

#endif
