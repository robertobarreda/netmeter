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
#ifndef NMNETPERF_H
#define NMNETPERF_H

#include <nmmoduleinterface.h>

#include "nmnetperferrormanager.h"
#include "nmnetperfutils.h"
#include "nmnetperfinterpret.h"
#include "nmnetperfexternalnetperf.h"
#include "nmnetperfexternalnetserver.h"
#include "nmnetperfexternaldecode.h"
#include "nmnetperfstatusmanager.h"
#include "nmnetperfeventnetserverrunning.h"
#include "nmnetperfeventfinishednetperf.h"
#include "nmnetperfeventfinishednetserver.h"
#include "nmnetperfeventdecodefinished.h"

class NMNetperf : public NMModuleInterface, public NMNetperfErrorManager, public NMNetperfStatusManager, public NMNetperfEventNetserverRunning,
			public NMNetperfEventFinishedNetperf, public NMNetperfEventFinishedNetserver, public NMNetperfEventDecodeFinished
{
	public:
		/**
		* \brief Constructor of the class
		*/
		NMNetperf();
		/**
		 * \brief Destructor of the class
		 */
		inline ~NMNetperf() {};
		/**
		 * \brief Initializes the module
		 */
		inline void initModule() {}
		/**
		 * \brief Executed when the module is removed
		 */
		inline void removeModule() {}

		/**
		* \brief Start command
		* \param data
		*/
		void start ( void * );
		/**
		 * \brief Stop command
		 */
		void stop ( void * );
		/**
		 * \brief Status command
		 */
		void status ( void * );
		/**
		* \brief Configure command
		*/
		void configure ( void * );
		/**
		* \brief Notifies the error
		* \param err Error type identifier
		*/
		void notifyError ( NMError err );

	private:
		/**
		 * \brief Executed when netserver is running, so time to run netperf
		 */
		void netserverRunning();
		/**
		 * \brief Executed when the netperf finishes the execution: stops netserver
		 */
		void finishedNetperf();
		/**
		 * \brief Executed when the netserver finishes the execution: starts decode process
		 */
		void finishedNetserver();
		/**
		 * \brief Execute when the decode process has finished
		 */
		void decodeFinished();
		/**
		 * Returns the suffixs of the files
		 * @return NMString with the suffix for the file name
		 */
		inline NMString getFilesSuffix()
		{
			return "_" + getCurrentTimeSuffix() + "_" + NMString::random().subString ( 2, 4 );
		}
		/**
		 * Returns the current time suffix with the format yyyymmdd_hhmmss
		 * \return NMString with the time expression suffix for the file name
		 */
		NMString getCurrentTimeSuffix();
		/**
		 * \brief Sets the status of the test
		 * \param status
		 */
		void setStatus ( NetperfStatus );
		/**
		 * \brief Sets the status of the test
		 * \return int with the status of the test
		 */
		inline int getStatus () { return testStatus; }

		NetperfStatus testStatus;
		NMString testFileSuffix, netserverOutputFile;

		bool netperfStarted, netserverStarted;
		NMNetperfInterpret interpret;
		NMNetperfExternalNetperf extNetperf;
		NMNetperfExternalNetserver extNetserver;
		NMNetperfExternalDecode extDecode;
};

#endif
