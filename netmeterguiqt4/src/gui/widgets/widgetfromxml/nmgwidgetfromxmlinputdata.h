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
#ifndef NMGWIDGETFROMXMLINPUTDATA_H
#define NMGWIDGETFROMXMLINPUTDATA_H

#include <QString>

/**
* Interface to implement for the class which has all the input data of the widget or can get the values
*/
class NMGWidgetFromXMLInputData
{
	public:
		NMGWidgetFromXMLInputData() {}
		virtual ~NMGWidgetFromXMLInputData() {}
		virtual QString getInput( QString id ) = 0;
		virtual void setInput( QString id, QString value ) = 0;
		virtual void cleanAllInput() = 0;
};

#endif

