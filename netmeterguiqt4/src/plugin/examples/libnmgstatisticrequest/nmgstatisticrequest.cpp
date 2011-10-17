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
#include "nmgstatisticrequest.h"

#include <iostream>
using namespace std;

/**
 * Contructor of the class
 * @return
 */
NMGStatisticRequest::NMGStatisticRequest()
		: NMGModule()
{
	moduleName = "NMGStatistic";
	moduleCapabilities = "Example";
	moduleVersion = "1.0";
	moduleCoreMatching = "NMStatisticRequest";
	moduleGUIName = "Request";
	widget = 0;
}

/**
 * Destructor of the class
 * @return
 */
NMGStatisticRequest::~NMGStatisticRequest()
{}

void NMGStatisticRequest::dataReceived ( QString s )
{
	cout << "NMGStatisticRequest::dataReceived" << endl << qPrintable(s) << endl;
}

void NMGStatisticRequest::moduleExecutionResults ( QString moduleName, QString data )
{
	cout << "**********************************************************************************" << endl;
	cout << "NMGStatisticRequest::moduleExecutionResults: " << qPrintable(moduleName) << endl;
	cout << "Data: " << qPrintable(data) << endl;
	cout << "**********************************************************************************" << endl;

	int iniPos = data.indexOf ( "<data>" ), lastPos = data.indexOf ( "</data>" );
	QString addValue = data.mid ( iniPos + QString ( "<data>" ).length(), lastPos - ( iniPos + QString ( "<data>" ).length() ) );

	widget->txtResult->setPlainText ( addValue );
}

QWidget * NMGStatisticRequest::getWidget()
{
	if ( !widget ) widget = new NMGStatisticRequestWidget ( this, &getPreferencesManager() );
	return widget;
}

