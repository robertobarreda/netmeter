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
#ifndef NMGHOSTINFORMATIONMANAGER_H
#define NMGHOSTINFORMATIONMANAGER_H

#include "nmgmodulefieldsmanager.h"
#include "nmgconfigurationfilemanager.h"
#include <nmghostinformationwidget.h>
#include <nmghostinformationcombowidget.h>

#include <QObject>
#include <QMap>
#include <QList>

#define HOST_INFORMATION_FILE	"netmeter.stations.xml"

#define TAG_STATIONS_ROOT	"netmeterstations"
#define TAG_STATIONS		"stations"


typedef QMap<QString, NMGStation *> NMGHostManagerMap;
typedef NMGHostManagerMap::iterator NMGHostManagerMapIterator;

typedef QList<NMGHostInformationComboWidget *> NMGHostComboList;
typedef NMGHostComboList::iterator NMGHostComboListIterator;

class NMGHostInformationManager : public QObject
{
		Q_OBJECT
	private:
		NMGHostInformationManager ( QObject *parent = 0, const char *name = 0 );

	public:
		~NMGHostInformationManager();
		static NMGHostInformationManager * self();
		NMGModuleFieldsManager & getFieldsManager();
		bool existsStation ( QString hostAddressOrAlias );
		NMGStation * getStation ( QString hostAddressOrAlias );
		void addStation ( NMGStation * );
		void removeStation ( QString hostAddressOrAlias );
		NMGHostInformationComboWidget * createHostComboWidget ( QWidget *parent, const char *name = 0 );
		void showHostWidget ( QString host = 0, NMGHostInformationComboWidget * notifyTo = 0, const QString & fieldToFocus = 0 );
		void comboToBeDeleted ( NMGHostInformationComboWidget * combo );
		void notifyAddedField ( QString module, QString fieldName, QString fieldNameToShow, QString defaultValue );
		void addAddressToCombo ( NMGHostInformationComboWidget * w );
		void setBaseWidgetFocus ( NMGHostInfoBaseWidgetFocus baseWidget );

		void loadInformationWidgetForManagement(); /* Shows global station manager from context menú */

	signals:
		void addedField ( QString module, QString fieldName, QString fieldNameToShow, QString defaultValue );

	public slots:
		void hostWidgetHidden();
		void setCurrentWidgetModule ( QString module );

	private:
		void createHostWidget();
		void updateWidgetWithNewStation ( NMGStation * station );
		void notifyAddressToComboList ( QString address );

	private:
		static NMGHostInformationManager * instance;
		NMGHostManagerMap hostsMapByAddress; // Map for address
		NMGHostManagerMap hostsMapByAlias;   // Map for alias
		NMGModuleFieldsManager fieldsManager;
		NMGHostInformationWidget * hostWidget;
		NMGXMLConfigurationFile * stationsFile;
		NMGHostInformationComboWidget * comboToNotify;
		NMGHostComboList comboList;
};

#endif
