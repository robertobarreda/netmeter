/***************************************************************************
 *   Copyright (C) 2008 by Roberto Barreda <rbarreda@ac.upc.edu>           *
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
#ifndef NMGSTATISTICREQUEST_H
#define NMGSTATISTICREQUEST_H

#include <nmgmodule.h>
#include "nmgstatisticrequestwidget.h"

class NMGStatisticRequest : public NMGModule
{
	public:
		NMGStatisticRequest();
		~NMGStatisticRequest();
		void dataReceived ( QString s );
		void moduleExecutionResults ( QString moduleName, QString data );

		QWidget * getWidget();

	private:
		NMGStatisticRequestWidget * widget;

};

NMMODULE_CLASS ( NMGStatisticRequest )
NMMODULE_NAME ( ( char * ) "NMGStatisticRequest" )
NMMODULE_CAPABILITIES ( ( char * ) "Example" )
NMMODULE_VERSION ( ( char * ) "1.0" )
NMMODULE_GUINAME ( ( char * ) "StatisticRequest" )
NMMODULE_TOOLTIP ( ( char * ) "Example module: Request the execution of the statistic module to get statistics" )

#endif
