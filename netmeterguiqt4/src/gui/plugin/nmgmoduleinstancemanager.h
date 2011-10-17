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
#ifndef NMGMODULEINSTANCEMANAGER_H
#define NMGMODULEINSTANCEMANAGER_H

#include "nmgmodule.h"
#include <nmgrandomgenerator.h>

#include <QString>
#include <QHash>

typedef QHash<QString, NMGModule *> QMapModuleInstanceManager; /* key-> testId, value-> Module associated*/
typedef QMapModuleInstanceManager::iterator QMapModuleInstanceManagerIterator;

typedef QList<NMGModule *> QModuleList;
typedef QModuleList::iterator QModuleListIterator;

class NMGModuleInstanceManager : public QObject
{
		Q_OBJECT
	public:
		NMGModuleInstanceManager ( QObject *parent = 0, const char *name = 0 );
		~NMGModuleInstanceManager();
		bool existsTest ( QString id );
		NMGModule * getModule ( QString id );
		void addModule ( QString id, NMGModule * m );
		void removeModule ( QString id );
		QString newTestId();
		NMGModule * getModuleByWidget ( QWidget * w );
		QString getTestIdByModule ( NMGModule * m );
		void removeTestIdByModule ( NMGModule * m );
		void removeAll();

	private:
		NMGRandomGenerator random;
		QMapModuleInstanceManager instanceMap;

};

#endif
