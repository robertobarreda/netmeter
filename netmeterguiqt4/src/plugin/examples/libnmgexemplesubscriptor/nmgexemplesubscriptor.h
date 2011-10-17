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
#ifndef NMGEXEMPLESUBSCRIPTOR_H
#define NMGEXEMPLESUBSCRIPTOR_H

#include "nmgexemplesubscriptorwidget.h"
#include <nmgmodule.h>

/**
* Exemple of a subscriptor module
*
@author Ernest Nieto
*/
class NMGExempleSubscriptor : public NMGModule
{
	public:
		NMGExempleSubscriptor();
		~NMGExempleSubscriptor();
		void dataReceived ( QString s );
		void errorReceived ( QString s );
		void statusReceived ( QString s );
		void commandReceived ( QString moduleName, NMGCommandType command );
		QWidget * getWidget();
		bool setExecutionData ( QString data );

	private:
		NMGExempleSubscriptorWidget * widget;
};


NMMODULE_CLASS ( NMGExempleSubscriptor )
NMMODULE_NAME ( ( char * ) "NMGExempleSubscriptor" )
NMMODULE_CAPABILITIES ( ( char * ) "example" )
NMMODULE_VERSION ( ( char * ) "1.0" )
NMMODULE_CORENAME ( ( char * ) "NMExempleSubscriptor" )
NMMODULE_GUINAME ( ( char * ) "Subscriptor" )
NMMODULE_TOOLTIP ( ( char * ) "Example module: Subscribes another module to its commands" )

#endif
