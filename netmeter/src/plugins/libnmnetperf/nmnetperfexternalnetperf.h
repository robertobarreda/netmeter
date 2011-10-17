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
#ifndef NMNETPERFEXTERNALNETPERF_H
#define NMNETPERFEXTERNALNETPERF_H

#include "nmnetperfinputfields.h"
#include "nmnetperferrormanager.h"
#include "nmnetperfstatusmanager.h"
#include "nmnetperfeventfinishednetperf.h"

#include <nmstring.h>
#include <nmparent.h>

class NMExternal;

class NMNetperfExternalNetperf : public NMParent
{
	public:
		/**
		* \brief Contructor of the class
		*/
		NMNetperfExternalNetperf ( NMParent *, NMNetperfErrorManager *, NMNetperfStatusManager *, NMNetperfEventFinishedNetperf * );
		/**
		* \brief Destructor of the class
		*/
		inline ~NMNetperfExternalNetperf() {}
		/**
		* \brief Configures the external Netperf
		* \param inputFields test information
		* \param netperfOutputFilePath remote output file where Netperf writes the result
		* \return TRUE if all is correct, or FALSE if not (and NMError is send to the GUI module)
		*/
		bool configureAndRunNetperf ( NMNetperfInputFields *, const NMString & );
		/**
		* Method executed when there is data from the netperf process
		*/
		void processReceived();
		/**
		* Method executed when the execution of netperf ends
		*/
		void processExit();
		/**
		* Kills the external netperf
		*/
		void killNetperf();

	private:
		NMNetperfErrorManager * errorManager;
		NMNetperfStatusManager * statusManager;
		NMExternal * external;
		NMNetperfEventFinishedNetperf * finishedNetperf;
};

#endif
