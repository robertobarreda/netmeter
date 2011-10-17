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
#include "nmgexemplerequest.h"

#include <QLineEdit>

#include <iostream>
using namespace std;

/**
 * Contructor of the class
 * @return
 */
NMGExempleRequest::NMGExempleRequest()
		: NMGModule()
{
	moduleName = "NMGExempleRequest";
	moduleCapabilities = "example";
	moduleVersion = "1.0";
	moduleCoreMatching = "NMExempleRequest";
	moduleGUIName = "Request";
	widget = 0;
}


/**
 * Destructor of the class
 * @return
 */
NMGExempleRequest::~NMGExempleRequest()
{
}

void NMGExempleRequest::dataReceived ( QString s )
{
	cout << "NMGExempleRequest::dataReceived" << endl << qPrintable(s) << endl;
}

void NMGExempleRequest::moduleExecutionResults ( QString moduleName, QString data )
{
	cout << "**********************************************************" << endl;
	cout << "NMGExempleRequest::moduleExecutionResults: " << qPrintable( moduleName ) << ", data: " << qPrintable( data ) << endl;
	cout << "**********************************************************" << endl;

	int iniPos = data.indexOf ( "<add>" ), lastPos = data.indexOf ( "</add>" );
	QString addValue = data.mid ( iniPos + QString ( "<add>" ).length(), lastPos - ( iniPos + QString ( "<add>" ).length() ) );

	widget->txtAdd->setText ( addValue );
}

QWidget * NMGExempleRequest::getWidget()
{
	if ( !widget ) widget = new NMGExempleRequestWidget ( this );
	return widget;
}

