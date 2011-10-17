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
#ifndef NMGEXEMPLESUBSCRIPTED_H
#define NMGEXEMPLESUBSCRIPTED_H

#include "nmgexemplesubscriptedwidget.h"
#include <nmgmodule.h>

/**
@author Ernest Nieto
*/
class NMGExempleSubscripted : public NMGModule
{
	public:
		NMGExempleSubscripted();
		~NMGExempleSubscripted();
		void dataReceived ( QString s );
		void errorReceived ( QString s );
		void statusReceived ( QString s );
		QWidget * getWidget();
		bool setExecutionData ( QString data );

	private:
		NMGExempleSubscriptedWidget * widget;

};


NMMODULE_CLASS ( NMGExempleSubscripted )
NMMODULE_NAME ( ( char * ) "NMGExempleSubscripted" )
NMMODULE_CAPABILITIES ( ( char * ) "example" )
NMMODULE_VERSION ( ( char * ) "1.0" )
NMMODULE_CORENAME ( ( char * ) "NMExempleSubscripted" )
NMMODULE_GUINAME ( ( char * ) "Subscripted" )
NMMODULE_TOOLTIP ( ( char * ) "Example module: Module subscribed to another module which shows the commands sent by the other" )

#endif
