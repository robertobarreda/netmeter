/***************************************************************************
 *   Copyright (C) 2005 by Ernest Nieto                                    *
 *   ernest.nieto@gmail.com                                                *
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
#ifndef NMGEXEMPLEREQUEST_H
#define NMGEXEMPLEREQUEST_H

#include <nmgmodule.h>

#include "nmgexemplerequestwidget.h"

/**
@author Ernest Nieto
*/
class NMGExempleRequest : public NMGModule
{
	public:
		NMGExempleRequest();
		~NMGExempleRequest();
		void dataReceived ( QString s );
		void moduleExecutionResults ( QString moduleName, QString data );
		QWidget * getWidget();

	private:
		NMGExempleRequestWidget * widget;

};

NMMODULE_CLASS ( NMGExempleRequest )
NMMODULE_NAME ( ( char * ) "NMGExempleRequest" )
NMMODULE_CAPABILITIES ( ( char * ) "example" )
NMMODULE_VERSION ( ( char * ) "1.0" )
NMMODULE_CORENAME ( ( char * ) "NMxempleRequest" )
NMMODULE_GUINAME ( ( char * ) "Request" )
NMMODULE_TOOLTIP ( ( char * ) "Example module: Request the execution of the adder module to add two numbers" )

#endif
