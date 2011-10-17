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
#ifndef NMGMODULECOMMUNICATIONMANAGER_H
#define NMGMODULECOMMUNICATIONMANAGER_H

#include "nmgmodulesubscriptionlist.h"
#include "nmgmodule.h"

#include <QHash>
#include <QList>
#include <QString>

typedef QHash<QString, QString> NMGModuleCommunicationManagerRequestMap; /* key-> id origen, value->id desti */
typedef NMGModuleCommunicationManagerRequestMap::iterator NMGModuleCommunicationManagerRequestMapIterator;

typedef QHash<QString, NMGModuleSubscriptionList *> NMGModuleCommunicationManagerSubscriptionMap;
typedef NMGModuleCommunicationManagerSubscriptionMap::iterator NMGModuleCommunicationManagerSubscriptionMapIterator;

class NMGModuleCommunicationManager : public QObject
{
		Q_OBJECT
	public:
		NMGModuleCommunicationManager ( QObject *parent = 0, const char *name = 0 );
		~NMGModuleCommunicationManager();

		QString request ( NMGModule * mod, QString moduleName, QString data );
		void endRequest ( QString id );
		bool isRequested ( QString id );
		QString getRequestDestination ( QString id );

		QString subscribe ( NMGModule * mod, QString moduleNameSubscribed, QString data );
		void endSubscribe ( NMGModule * mod, QString id );
		void endPendingSubscriptions ( QString id );
		bool commandExecuted ( NMGModule * mod, NMGCommandType commandType,
		                       const QString & data = "", const QString & receiver = "" );
		QString executeModule ( QString moduleName, QString data );

	private:
		NMGModuleSubscriptionList * getSubscriptionListOrCreate ( QString id );
		NMGModuleSubscriptionList * getSubscriptionList ( QString id );

	private:
		NMGModuleCommunicationManagerRequestMap requestMap;
		NMGModuleCommunicationManagerSubscriptionMap subscriptionMap;

};

#endif
