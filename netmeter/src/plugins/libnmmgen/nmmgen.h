/*
 *   $Id: nmmgen.h,v 1.17 2007-05-27 11:19:34 rserral Exp $
 *
 *   Copyright (c) 2006 Ernest Nieto
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifndef NMMGEN_H
#define NMMGEN_H

#include <nmmoduleinterface.h>

#include "nmmgenerrormanager.h"
#include "nmmmgenutils.h"
#include "nmmgeninterpret.h"
#include "nmmgenexternalmgen.h"
#include "nmmgenexternaldrec.h"
#include "nmmgenexternaldecode.h"
#include "nmmgenstatusmanager.h"
#include "nmmgeneventdrecrunning.h"
#include "nmmgeneventreadytorundrec.h"
#include "nmmgeneventfinishedmgen.h"
#include "nmmgeneventfinisehddrec.h"
#include "nmmgeneventdecodefinished.h"
#include "nmmgenreadytorunmgen.h"

/**
@author Ernest Nieto
*/
class NMMGen : public NMModuleInterface, public NMMgenErrorManager, public NMMgenStatusManager, public NMMgenEventDrecRunning,
			public NMMgenEventReadyToRunDrec, public NMMgenEventFinishedMgen, public NMMgenEventFinishedDrec, public NMMgenEventDecodeFinished,
			public NMMgenEventReadyToRunMgen
{
	public:
		NMMGen();
		~NMMGen();
		void initModule();
		void removeModule();
		void start ( void *data );
		void stop ( void *data );
		void status ( void *data );
		void configure ( void *data );
		void notifyError ( NMError err );
		int getMgenVersion();

	private:
		void readyToRunDrec();
		void drecRunning();
		void readyToRunMgen();
		void finishedMgen();
		void finishedDrec();
		void decodeFinished();
		NMString getFilesSuffix();
		NMString getCurrentTimeSuffix();
		void setStatus ( MgenStatus status );

	private:
		MgenStatus testStatus;
		NMString testFileSuffix;
		NMString decodeOutputFile;
		NMString drecOutputFile;
		NMString testFile;
		NMString fileGraphOWD;
		NMString fileGraphOWDD;
		NMString fileGraphIPDV;
		NMString fileGraphIPDVD;

		bool mgenStarted;
		bool sync;
		NMMGenInterpret interpret;
		NMMGenExternalMgen extMgen;
		NMMGenExternalDrec extDrec;
		NMMGenExternalDecode extDecode;
};

#endif
