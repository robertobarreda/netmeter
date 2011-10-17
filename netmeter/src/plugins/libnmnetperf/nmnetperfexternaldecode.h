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
#ifndef NMNETPERFEXTERNALDECODE_H
#define NMNETPERFEXTERNALDECODE_H

#include <nmparent.h>
#include <nmscp.h>

#include "nmnetperfinputfields.h"
#include "nmnetperfstatusmanager.h"
#include "nmnetperferrormanager.h"
#include "nmnetperfeventdecodefinished.h"

class NMNetperfExternalDecode : public NMParent
{
	public:
		/**
		* \brief Contructor of the class
		*/
		NMNetperfExternalDecode ( NMParent * parent, NMNetperfErrorManager * errorMan, NMNetperfStatusManager * statusMan, NMNetperfEventDecodeFinished * decodeEnd );
		/**
		* \brief Destructor of the class
		*/
		inline ~NMNetperfExternalDecode() {}
		/**
		* \brief Configures the decode
		* \param inputFields test information
		* \param netperfOutputFilePath remote output file where Netperf writes the result
		* \return TRUE if all is correct, or FALSE if not (and NMError is send to the GUI module)
		*/
		bool configureAndRunDecode ( NMNetperfInputFields *, const NMString & );

	private:
		/**
		* \brief Executed when the decode output file has been sent to local (via scp)
		*/
		void decodeFileSent();

		NMNetperfErrorManager * errorManager;
		NMNetperfStatusManager * statusManager;
		NMNetperfEventDecodeFinished * decodeFinished;
		NMScp scp;
};

#endif
