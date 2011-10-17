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
#ifndef NMGMODULEREGISTER_H
#define NMGMODULEREGISTER_H

#include <nmmodulemanager.h>
#include "nmgmoduleinstancemanager.h"

#include <QMap>
#include <QObject>

typedef QMap<QString,NMModule *> QMapModuleManager;
typedef QMapModuleManager::iterator QMapModuleManagerIterator;

class NMGModuleRegister : public QObject
{
		Q_OBJECT
	private:
		NMGModuleRegister ( QObject *parent = 0, const char *name = 0 );

	public:
		~NMGModuleRegister();
		bool existsModule ( QString name );
		NMModule * getModule ( QString name );
		static NMGModuleRegister * self();
		QString createModuleInterface ( QString name );
		QString duplicateModuleInterface ( QString id );
		NMGModule * getModuleInterface ( QString id );
		NMGModuleInstanceManager &  getInstanceManager();

	private:
		static NMGModuleRegister * instance;
		QMapModuleManager moduleMapByName;
		QMapModuleManager moduleMapByCoreName;
		QMapModuleManager moduleMapByGUIName;

		NMGModuleInstanceManager instanceManager;

		int nextId;
};

#endif
