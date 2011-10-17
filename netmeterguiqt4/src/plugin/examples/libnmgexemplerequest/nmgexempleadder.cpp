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
#include "nmgexempleadder.h"

#include <QLineEdit>

#include <iostream>
using namespace std;

NMGExempleAdder::NMGExempleAdder()
		: NMGModule()
{
	moduleName = "NMGExempleAdder";
	moduleCapabilities = "example";
	moduleVersion = "1.0";
	moduleCoreMatching = "NMExempleAdder";
	moduleGUIName = "Adder";
	widget = 0;
}

NMGExempleAdder::~NMGExempleAdder()
{}

void NMGExempleAdder::dataReceived ( QString s )
{
	int iniPos = s.indexOf ( "<add>" ), lastPos = s.indexOf ( "</add>" );
	QString addValue = s.mid ( iniPos + QString ( "<add>" ).length(), lastPos - ( iniPos + QString ( "<add>" ).length() ) );
	widget->txtAdd->setText ( addValue );
}

/**
 * Format:
 * <data>
 *  <value1>5</value1>
 *  <value2>6</value2>
 * </data>
 * @param data
 * @return
 */
bool NMGExempleAdder::setExecutionData ( QString data )
{
	//s'hauria de comprovar el format i si no es correcte retornar false

	sendCommand ( Start, data );
	return true;
}

QWidget * NMGExempleAdder::getWidget()
{
	if ( !widget ) widget = new NMGExempleAdderWidget ( this );
	return widget;
}
