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
#ifndef NMGWIDGETFROMXMLBUTTONGROUP_H
#define NMGWIDGETFROMXMLBUTTONGROUP_H

#include "nmgwidgetfromxmlinputwidget.h"

#include <QGroupBox>
#include <QLayout>
#include <QRadioButton>

#define CHILDS_SUFFIX	"_rb"

/**
Frame with permits insert radioButtons inside
*/
class NMGWidgetFromXMLButtonGroup : public QGroupBox, public NMGWidgetFromXMLInputWidget
{
		Q_OBJECT
	public:
		NMGWidgetFromXMLButtonGroup( QWidget *parent = 0, const char *name = 0 );
		~NMGWidgetFromXMLButtonGroup();
		QString getId();
		QString getInput();
		void setInput( QString value );
		void setLayout( QLayout * l );
		void newOption( QString value, bool checked = false );

	private:
		QLayout * layout;
};

#endif
