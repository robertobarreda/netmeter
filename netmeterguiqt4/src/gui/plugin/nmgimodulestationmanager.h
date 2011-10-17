/***************************************************************************
 *   Copyright (C) 2007 by Roberto Barreda <rbarreda@ac.upc.edu>           *
 *   Copyright (C) 2005 by Ernest Nieto <ernest.nieto@gmail.com>           *
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
#ifndef NMGIMODULESTATIONMANAGER_H
#define NMGIMODULESTATIONMANAGER_H

#include <nmghostinformationmanager.h>
#include <nmghostinformationcombowidget.h>

/**
* Interface which allows to handle stations
*/
class NMGIModuleStationManager
{
	public:
		NMGIModuleStationManager() {}
		virtual ~NMGIModuleStationManager() {}
		virtual void addFieldToFieldsManager ( QString fieldId, QString fieldToShow, QString defaultValue = "" ) = 0;
		virtual bool existsStation ( QString hostAddressOrAlias ) = 0;
		virtual NMGStation * getStation ( QString hostAddressOrAlias ) = 0;
		virtual void addStation ( NMGStation * ) = 0;
		virtual void removeStation ( QString hostAddressOrAlias ) = 0;
		virtual NMGHostInformationComboWidget * createHostComboWidget ( QWidget *parent, const char *name ) = 0;
		virtual QString getModuleStationKey() = 0;

};

#endif
