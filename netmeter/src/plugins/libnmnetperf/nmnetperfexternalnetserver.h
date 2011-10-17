/***************************************************************************
 *   Copyright (C) 2008 by Roberto Barreda                                 *
 *   rbarreda@ac.upc.edu                                                   *
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
#ifndef NMNETPERFEXTERNALNETSERVER_H
#define NMNETPERFEXTERNALNETSERVER_H

#include "nmnetperfinputfields.h"
#include "nmnetperferrormanager.h"
#include "nmnetperfstatusmanager.h"
#include "nmnetperfeventnetserverrunning.h"
#include "nmnetperfeventfinishednetserver.h"

#include <nmparent.h>

class NMExternal;

class NMNetperfExternalNetserver : public NMParent
{
	public:
		/**
		* \brief Contructor of the class
		*/
		NMNetperfExternalNetserver ( NMParent * parent, NMNetperfErrorManager * errorMan, NMNetperfStatusManager * statusMan, NMNetperfEventNetserverRunning * netserverRunning, NMNetperfEventFinishedNetserver * finishedNetserver );
		/**
		* \brief Destructor of the class
		*/
		inline ~NMNetperfExternalNetserver() {}
		/**
		* \brief Configures the external Netserver
		* \param inputFields test information
		* \return TRUE if all is correct, or FALSE if not (and NMError is send to the GUI module)
		*/
		bool configureAndRunNetserver ( NMNetperfInputFields * );
		/**
		* \brief Executed when the process has received data
		*/
		void netserverProcessReceived();
		/**
		* \brief Executed when the process exits
		*/
		void netserverProcessExit();
		/**
		* \brief Kills the netserver process
		*/
		bool killNetserver();

	private:
		bool isRunning;
		NMNetperfErrorManager * errorManager;
		NMNetperfStatusManager * statusManager;
		NMExternal * external;
		NMNetperfEventNetserverRunning * netserverRunning;
		NMNetperfEventFinishedNetserver * finishedNetserver;
};

#endif
