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
#include "nmghostinformationmanager.h"

#include "nmgapplicationinformation.h"
#include <nmgmainwindow.h>
#include <nmgmodule.h>

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QWidget>

#include <iostream>
using namespace std;

NMGHostInformationManager * NMGHostInformationManager::instance = 0;

/**
 * Contructor of the class
 * @param parent
 * @param name
 * @return
 */
NMGHostInformationManager::NMGHostInformationManager ( QObject *parent, const char *name )
		: QObject ( parent ), fieldsManager()
{
	setObjectName ( name );
	hostWidget = 0;
	comboToNotify = 0;

	stationsFile = NMGConfigurationFileManager::self()->getXMLConfigurationFile ( HOST_INFORMATION_FILE );
	if ( stationsFile )
	{
		QDomDocument doc = stationsFile->getDomDocument();
		QDomElement root = doc.documentElement();
		if ( root.tagName() == TAG_STATIONS_ROOT )
		{
			QDomNode node = root.firstChild();
			if ( !node.isNull() and node.isElement() and node.nodeName() == TAG_STATIONS )
			{
				node = node.toElement().firstChild();
				while ( !node.isNull() )
				{
					if ( node.isElement() )
					{
						QDomElement elem = node.toElement();
						NMGStation * station = NMGStation::makeStationFromXMLElement ( elem );
						if ( station ) addStation ( station );
					}
					node = node.nextSibling();
				}
			}
			else cerr << "XML Host information incorrect (" << qPrintable ( node.nodeName() ) << ")." << endl;
		}
	}

	/** Creem algunes de prova si no estan aquestes */
	if ( not existsStation ( "127.0.0.1" ) )
	{
		NMGStation * s = new NMGStation();
		s->getTestHost().setAddress ( "127.0.0.1" );
		s->getTestHost().setAlias ( "localhost" );
		s->getTestHost().setHostIPV4 ( true );
		s->getManagementHost().setAddress ( "127.0.0.1" );
		s->setManagementAddressPort ( 22 );
		s->getManagementHost().setAlias ( "localhost" );
		s->getManagementHost().setHostIPV4 ( true );
		addStation ( s );
	}
}

/**
 * Destructor of the class
 * @return
 */
NMGHostInformationManager::~NMGHostInformationManager()
{

	QDomDocument doc ( QString ( TAG_STATIONS_ROOT ) + "_header" );
	QDomElement root = doc.createElement ( TAG_STATIONS_ROOT );
	doc.appendChild ( root );

	QDomElement stationsElement = doc.createElement ( TAG_STATIONS );
	root.appendChild ( stationsElement );

	//we have to delete all the allocated Fields and save to disk the stations
	for ( NMGHostManagerMapIterator it = hostsMapByAddress.begin(); it != hostsMapByAddress.end(); ++it )
	{
		QString key = it.key();
		NMGStation * station = it.value();
		station->addXMLStationNode ( doc, stationsElement );
		hostsMapByAddress.remove ( key );
		hostsMapByAlias.remove ( station->getTestHost().getAlias() );
		delete station;
	}

	// write xml on memory to disc
	if ( stationsFile )
	{
		stationsFile->getDomDocument().setContent ( doc.toString() );
		stationsFile->writeToDisk();
		delete stationsFile;
	}
}

/**
 * Returns the unique instance of the class (singleton class)
 * @return
 */
NMGHostInformationManager * NMGHostInformationManager::self()
{
	if ( !instance ) instance = new NMGHostInformationManager();
	return instance;
}


/**
 * Returns the filed manager
 * @return
 */
NMGModuleFieldsManager & NMGHostInformationManager::getFieldsManager()
{
	return fieldsManager;
}

/**
 * Returns true if the station identified by the adress exists
 * @param hostAddressOrAlias Test Host address or alias
 * @return
 */
bool NMGHostInformationManager::existsStation ( QString hostAddressOrAlias )
{
	if ( !getStation ( hostAddressOrAlias ) ) return false;
	return true;
}

/**
 * Returns the station associated to the indicated address
 * @param hostAddressOrAlias Test Host address or alias
 * @return The associated station or 0 if it doesn't exist
 */
NMGStation * NMGHostInformationManager::getStation ( QString hostAddressOrAlias )
{
	NMGHostManagerMapIterator it = hostsMapByAddress.find ( hostAddressOrAlias );
	if ( it == hostsMapByAddress.end() )
	{
		it = hostsMapByAlias.find ( hostAddressOrAlias );
		if ( it == hostsMapByAlias.end() ) return 0;
		else return *it;
	}
	return *it;
}

/**
 * Adds a station to the manager (its address can't be empty [""], otherwise it isn't added)
 * @param  station NMGStation to add
 */
void NMGHostInformationManager::addStation ( NMGStation * station )
{
	if ( !station->getTestHost().getAddress().isEmpty() )
	{
		hostsMapByAddress.insert ( station->getTestHost().getAddress(), station );
		notifyAddressToComboList ( station->getTestHost().getAddress() );
		if ( !station->getTestHost().getAlias().isEmpty() )
			hostsMapByAlias.insert ( station->getTestHost().getAlias(), station );
	}
}

/**
 * Removes a station indentified by its address
 * @param hostAddressOrAlias Test Host address or alias
 */
void NMGHostInformationManager::removeStation ( QString hostAddressOrAlias )
{
	NMGStation * station = getStation ( hostAddressOrAlias );
	if ( !station ) return;
	if ( !station->getTestHost().getAddress().isEmpty() ) hostsMapByAddress.remove ( station->getTestHost().getAddress() );
	if ( !station->getTestHost().getAlias().isEmpty() ) hostsMapByAlias.remove ( station->getTestHost().getAlias() );
	delete station;
}

/**
 * Emits the signal to inform that a new field was added
 * @param module
 * @param fieldName
 * @param fieldNameToShow
 * @param defaultValue
 */
void NMGHostInformationManager::notifyAddedField ( QString module, QString fieldName, QString fieldNameToShow, QString defaultValue )
{
	emit addedField ( module, fieldName, fieldNameToShow, defaultValue );
}

/**
 * Sets the focus of a base widget of the widget
 * @param baseWidget
 */
void NMGHostInformationManager::setBaseWidgetFocus ( NMGHostInfoBaseWidgetFocus baseWidget )
{
	createHostWidget();
	hostWidget->setWidgetFocus ( baseWidget );
}

/**
 * Show the host widget, when it's hidden the selected host is notified to the combo notifyTo
 * @param host
 * @param notifyTo
 */
void NMGHostInformationManager::showHostWidget ( QString host, NMGHostInformationComboWidget * notifyTo, const QString & fieldToFocus )
{
	createHostWidget();
	if ( host.isNull() ) host = "";
	NMGStation * station = getStation ( host );
	hostWidget->updateWidgetWithStation ( station );
	if ( !station ) hostWidget->setTestHostAddress ( host );
	if ( !fieldToFocus.isNull() ) hostWidget->setFieldFocus ( fieldToFocus );
	hostWidget->show();
	comboToNotify = notifyTo;
}

/**
 * Creates a new Host combo widget
 * @param parent The parent of the new widget
 * @param name Name of the widget
 * @return A new instance
 */
NMGHostInformationComboWidget * NMGHostInformationManager::createHostComboWidget ( QWidget *parent, const char *name )
{
	NMGHostInformationComboWidget * w = new NMGHostInformationComboWidget ( parent, name );
	return w;
}

/**
 * Adds the address in the manager to the combo box
 * @param w Combo to add the addresses (can't be null)
 */
void NMGHostInformationManager::addAddressToCombo ( NMGHostInformationComboWidget * w )
{
	for ( NMGHostManagerMapIterator it = hostsMapByAddress.begin(); it != hostsMapByAddress.end(); ++it )
		w->addAddressToCombo ( it.key() );
	for ( NMGHostManagerMapIterator it = hostsMapByAlias.begin(); it != hostsMapByAlias.end(); ++it )
		if ( !it.key().isEmpty() ) w->addAddressToCombo ( it.key() );
	comboList.append ( w );
}

/**
 * Sets the visible module of the widget
 * @param module
 */
void NMGHostInformationManager::setCurrentWidgetModule ( QString module )
{
	createHostWidget();
	hostWidget->setCurrentModule ( module );
}

/**
 * Creates the host widget if it hasn't been created
 */
void NMGHostInformationManager::createHostWidget()
{
	if ( hostWidget ) return;
	NMGMainWindow * w = NMGApplicationInformation::getApplicationInformation()->getMainWindow();
	hostWidget = new NMGHostInformationWidget ( ( QMainWindow * ) w, "hostInformationWidget", Qt::Dialog );
	hostWidget->setWindowModality ( Qt::ApplicationModal );
	connect ( this, SIGNAL ( addedField ( QString, QString, QString, QString ) ),
	          hostWidget, SLOT ( addHostField ( QString, QString, QString, QString ) ) );

	QList<NMGStation *> list = hostsMapByAddress.values();
	for ( QList<NMGStation *>::iterator it = list.begin(); it != list.end(); ++it )
		updateWidgetWithNewStation ( *it );
}

/**
 * Slot executed when the widget is hidden
 */
void NMGHostInformationManager::hostWidgetHidden()
{
	NMGStation * station = hostWidget->getStation();
	if ( station )
	{
		QString address = station->getTestHost().getAddress();
		if ( not existsStation ( address ) )
		{
			addStation ( station );
			updateWidgetWithNewStation ( station );
		}
		else
		{
			//the widget executed a 'new' instruction and it must be undone
			//before we pass the changed attributes
			NMGStation * originalStation = getStation ( address );
			if ( originalStation->getTestHost().getAddress() != address )
			{
				hostsMapByAddress.remove ( originalStation->getTestHost().getAddress() );
				originalStation->getTestHost().setAddress ( address );
				hostsMapByAddress.insert ( originalStation->getTestHost().getAddress(), originalStation );
				notifyAddressToComboList ( originalStation->getTestHost().getAddress() );
			}
			if ( originalStation->getTestHost().getAlias() != station->getTestHost().getAlias() )
			{
				hostsMapByAlias.remove ( originalStation->getTestHost().getAlias() );
				originalStation->getTestHost().setAlias ( station->getTestHost().getAlias() );
				hostsMapByAlias.insert ( originalStation->getTestHost().getAlias(), originalStation );
			}
			originalStation->getTestHost().setHostIPV4 ( station->getTestHost().isHostIPv4() );
			originalStation->getManagementHost().setAddress ( station->getManagementHost().getAddress() );
			originalStation->setManagementAddressPort ( station->getManagementAddressPort() );
			originalStation->getManagementHost().setAlias ( station->getManagementHost().getAlias() );
			originalStation->getManagementHost().setHostIPV4 ( station->getManagementHost().isHostIPv4() );
			originalStation->setManagementUsername ( station->getManagementUsername() );
			originalStation->setManagementPassword ( station->getManagementPassword() );
			originalStation->getStationFieldsManager().setFields ( station->getStationFieldsManager() );

			delete station;
			station = originalStation;
		}
	}
	if ( comboToNotify ) comboToNotify->notifyStation ( station );
	else
	{
		NMGModule * m = NMGApplicationInformation::getApplicationInformation()->getMainWindow()->getCurrentModule();
		if ( station && m ) m->notifyNewStation ( station );
	}
	comboToNotify = 0;
}

/**
 * Updates the combo lists widgets with a new station
 * @param station
 */
void NMGHostInformationManager::updateWidgetWithNewStation ( NMGStation * station )
{
	if ( !station->getTestHost().getAddress().isEmpty() )
		hostWidget->addTestHostAddressToList ( station->getTestHost().getAddress() );
	if ( !station->getTestHost().getAlias().isEmpty() )
		hostWidget->addTestHostAliasToList ( station->getTestHost().getAlias() );
}

/**
 * Notifies the address to the combos of the list
 * @param address
 */
void NMGHostInformationManager::notifyAddressToComboList ( QString address )
{
	for ( NMGHostComboListIterator combo = comboList.begin(); combo != comboList.end(); ++combo )
		( *combo )->addAddressToCombo ( address );
}

/**
 * Removes the combo from the combo list for updating new stations
 * @param combo
 */
void NMGHostInformationManager::comboToBeDeleted ( NMGHostInformationComboWidget * combo )
{
	comboList.removeAll ( combo );
}

/**
 * Using the Information widget, all stations are loaded to manage their fields in an integrated environment.
 */
void NMGHostInformationManager::loadInformationWidgetForManagement()
{
	/*QStringSet moduleList = QStringSet(); // Contains a list of all modules managed by stations
	for ( NMGHostManagerMapIterator it = hostsMapByAddress.begin(); it != hostsMapByAddress.end(); ++it )
	{
		QString key = it.key();
		NMGStation * station = it.value();
		moduleList.unite ( station->getStationFieldsManager().getModuleFieldList() );
	}*/

	createHostWidget();

	/*for ( QStringSetIterator it = moduleList.begin(); it != moduleList.end(); ++it)
	{
	  QString module = *it;
	  notifyAddedField( module, fieldName, fieldNameToShow, defaultValue );
	}*/

	hostWidget->updateWidgetWithStation();
	hostWidget->setShowAllModuleTabs();
	hostWidget->show();
}

#include "nmghostinformationmanager.moc"
