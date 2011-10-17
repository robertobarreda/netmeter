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
#ifndef NMGEXEMPLEADDER_H
#define NMGEXEMPLEADDER_H

#include "nmgexempleadderwidget.h"
#include <nmgmodule.h>

/**
@author Ernest Nieto
*/
class NMGExempleAdder : public NMGModule
{
	public:
		NMGExempleAdder();
		~NMGExempleAdder();
		void dataReceived ( QString s );
		bool setExecutionData ( QString data );
		QWidget * getWidget();

	private:
		NMGExempleAdderWidget * widget;
};

NMMODULE_CLASS ( NMGExempleAdder )
NMMODULE_NAME ( ( char * ) "NMGExempleAdder" )
NMMODULE_CAPABILITIES ( ( char * ) "example" )
NMMODULE_VERSION ( ( char * ) "1.0" )
NMMODULE_CORENAME ( ( char * ) "NMGxempleAdder" )
NMMODULE_GUINAME ( ( char * ) "Adder" )
NMMODULE_MODULEICON ( ( char * ) "iconadder.png" )
NMMODULE_TOOLTIP ( ( char * ) "Example module: Adds two numbers" )

#endif
