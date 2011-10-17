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
#ifndef NMGMODULEMANAGER_H
#define NMGMODULEMANAGER_H

#include <nmmodulemanager.h>
#include "nmgmoduleinstancemanager.h"
#include "nmgmodulecommunicationmanager.h"
#include "nmgmodulefactory.h"
#include "nmgmoduleparent.h"

#include <QMap>
#include <QObject>
#include <QList>

typedef QMap<QString,NMModule *> QMapModuleManager;
typedef QMapModuleManager::iterator QMapModuleManagerIterator;

class NMGModuleManager : public QObject, public NMModuleManager
{
		Q_OBJECT
	private:
		NMGModuleManager ( QObject *parent = 0, const char *name = 0 );

	public:
		~NMGModuleManager();
		bool existsModule ( QString name );
		NMModule * getModule ( QString name );
		static NMGModuleManager * self();
		QString createModuleInterface ( QString name );
		QString duplicateModuleInterface ( QString id );
		NMGModule * getModuleInterface ( QString id );
		QList<NMModule *> getModuleList();
		NMGModuleInstanceManager &  getInstanceManager();
		NMGModuleCommunicationManager & getCommunicationManager();
		NMModule * makeModule ( const NMString& path );
		void deletePluginInterface ( NMGModule * );
		NMGModuleParent * getModuleParent ( NMGModule * );
		void setLoadingModulesStartup ( bool startup );
		bool getLoadingModulesStartup();
		inline void emitFinishedGUIRequest(QString id, QString data) { emit finishedGUIRequest( id, QString(), data ); };

	signals:
		void newModuleAdded ( QString moduleName, QString capabilities );
		void finishedGUIRequest( QString id, QString, QString data);

	public slots:
		void addModule ( NMModule * );
		void loadNewModule ( QString path );
		void unloadExistingModule ( QString moduleName );
		void errorReceived ( QString id, int errorType, QString errorMessage );
		void statusReceived ( QString id, QString data );

	private:
		static NMGModuleManager * instance;
		bool loadingModulesStartup;
		QMapModuleManager moduleMapByName;
		QMapModuleManager moduleMapByCoreName;
		QMapModuleManager moduleMapByGUIName;

		NMGModuleFactory guiModuleFactory;

		NMGModuleCommunicationManager communicationManager;
		NMGModuleInstanceManager instanceManager;

		int nextId;
};

#endif
