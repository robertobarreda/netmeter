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
#ifndef NMGWIDGETFROMXMLACTION_H
#define NMGWIDGETFROMXMLACTION_H

#include "nmgwidgetfromxmlinputdata.h"
#include "nmgwidgetfromxmlinterface.h"

#include <QDomNode>
#include <QList>
#include <QMessageBox>

typedef enum{Send, Hide, Assign, Validation} Action;

struct ValidationItem
{
	QString id;
	QString type;
	QString message;
};

struct ActionInfo
{
	Action action;
	QString type;
	QString data;
	QString module;
	QList <ValidationItem *> list;
};

class NMGWidgetFromXMLAction
{
	public:
		NMGWidgetFromXMLAction( NMGWidgetFromXMLInterface  * wi, NMGWidgetFromXMLInputData * id, QDomNode &parent );
		~NMGWidgetFromXMLAction();
		void setInputData( NMGWidgetFromXMLInputData * id );
		void execute();

	private:
		void addActionSend( QDomNode & parent );
		void addActionHide( QDomNode &parent );
		void addActionAssign( QDomNode &parent );
		void addActionValidation( QDomNode &parent );
		QString getTree( QDomNode & parent );
		QString putValues( QString request );

	private:
		QList<ActionInfo *> actionList;

		NMGWidgetFromXMLInterface  * widgetInterface;
		NMGWidgetFromXMLInputData * inputData;
};

#endif
