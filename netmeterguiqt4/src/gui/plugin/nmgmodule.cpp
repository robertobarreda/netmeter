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
#include "nmgmodule.h"
#include "nmgmodulemanager.h"
#include "nmgmodulecommunicationmanager.h"

#include <nmgapplicationinformation.h>
#include <nmgpreferencesmanager.h>

#include <iostream>
using namespace std;

/**
 * Initializes the GUI Module and calls the virtual function initModuleGUI
 */
void NMGModule::initModule()
{
	widget = 0;
	initModuleGUI();
}

/**
 * Removes the GUI module from the memory and call the virtual function removeModuleGUI
 */
void NMGModule::removeModule()
{
	removeModuleGUI();
}

/**
 * Null: to reimplement in the sublcass
 */
void NMGModule::initModuleGUI()
{}

/**
 * Null: to reimplement on the subclass
 */
void NMGModule::removeModuleGUI()
{}

/**
 * Returns the plugin name
 * @return
 */
QString NMGModule::getName()
{
	QString name = getModuleGUIName();
	if ( name.isEmpty() ) name = getModuleName();
	if ( name.isEmpty() ) name = getModuleCoreName();
	return name;
}

/**
 * Returns the name of the module
 * @return The name of the module
 */
QString NMGModule::getModuleName()
{
	return moduleName;
}

/**
 * Returns the capability of the module
 * @return The capabilities of the module
 */
QString NMGModule::getModuleCapabilities()
{
	return moduleCapabilities;
}

/**
 * Return the version of the module
 * @return The version of the module
 */
QString NMGModule::getModuleVersion()
{
	return moduleVersion;
}

/**
 * Return the core-plugin which matches with this GUI-plugin
 * @return The name of the core-module which matches with this module
 */
QString NMGModule::getModuleCoreName()
{
	return moduleCoreMatching;
}

/**
 * Return name of the plugin to be shown at the GUI
 * @return Name of the plguin to show at the GUI
 */
QString NMGModule::getModuleGUIName()
{
	return moduleGUIName;
}

/**
 * Returns the name of the XMl file which contains the information fo the widget of the module
 * @return
 */
QString NMGModule::getXMLWidgetConfigurationFile()
{
	return NMGPreferencesManager::self()->getXMLDirectory() + "/" + getModuleName().toLower() + XML_WIDGET_CONFIGURATION_FILE_EXTENSION;
}

/**
 * Returns the widget wich is specified in the xml file
 * @return QWidget of the Module
 */
QWidget * NMGModule::getWidget()
{
	if ( !widget )
	{
		widget = factory.makeWidgetFromXML( getXMLWidgetConfigurationFile() );
		if ( !widget )
		{
			cerr << "Couldn't create the widget of the module " << qPrintable( getModuleName() ) << "." << endl;
			return NULL;
		}
	}
	return widget;
}

/**
 * Returns the popup menu associated to the module
 * @return
 */
QMenu * NMGModule::getPopupMenu()
{
	return 0;
}

/**
 * Returns the toolbar associated to the module
 * @return
 */
QToolBar * NMGModule::getToolBar()
{
	return 0;
}

/**
 * Sends the specified command with the data to the subscribed list of NMG's plugins
 * @param commandType Command type (Configure,Start,Stop,Status).
 * @param data Data which will go between the tags <data> ...</data>.
 * @param receiver Name of the plugin that must recive the data message.
 * @return True if the command performs right or False otherwise
 */
bool NMGModule::sendGUICommand( NMGCommandType commandType, const QString & data, const QString & receiver )
{
	return NMGModuleManager::self()->getCommunicationManager().commandExecuted( this, commandType, data, receiver );
}

/**
 * Sends the specified command with the data to the server
 * @param commandType Command type (Configure,Start,Stop,Status)
 * @param data Data which will go between the tags <data> ...</data>
 * @return True if the command performs right or False otherwise
 */
bool NMGModule::sendCommand( NMGCommandType commandType, const QString & data )
{
	return NMGConnection::getConnection()->
	       sendCommand( commandType, getModuleCoreName().toLower(), getTestId(), data );
}

/**
 * The server has sent  a result to a start request
 * @param data Data which was between the tags <data> ...</data> (including the tags)
 */
void NMGModule::dataReceivedParent( QString data )
{
	/* look if somebody requested the info */
	QString moduleId = NMGModuleManager::self()->getInstanceManager().getTestIdByModule( this );
	QString destinationId = NMGModuleManager::self()->getCommunicationManager().getRequestDestination( moduleId );
	
	if ( destinationId.isEmpty() ) dataReceived( data );
	else
	{
		NMGModule * destination = NMGModuleManager::self()->getInstanceManager().getModule( destinationId );
		if ( !destination ) cerr << "Error: destination request module doesn't exist " << qPrintable( destinationId ) << endl;
		else destination->moduleExecutionResults( getModuleName(), data );
	}
}

/**
 * The server has sent  a result to a start request
 * @param data Data which was between the tags <data> ...</data> (including the tags)
 */
void NMGModule::dataReceived( QString data )
{
	cout << "Data received at " << qPrintable( getModuleGUIName() ) << " not handled:" << qPrintable( data ) << endl;
}

/**
 * The server sent a error message to the module
 * @param errorType Type of the error (fatal = 1, user = 5, warning = 10, debug = 20)
 * @param errorMessage Error Message
 */
void NMGModule::errorReceived( int errorType, QString errorMessage )
{
	cout << "Error at " << qPrintable( getModuleGUIName() ) << " not handled: type=";
	cout << errorType << " message=" << qPrintable( errorMessage ) << endl;
}

/**
 * The server sent a status reply
 * @param data Data which was between the tags <data> ...</data> (including the tags)
 */
void NMGModule::statusReceived( QString data )
{
	cout << "Status reply at " << qPrintable( getModuleGUIName() ) << " not handled:" << qPrintable( data ) << endl;
}

/**
 * Executed when the module has been subscribe to another module and an event occurs
 * @param moduleName Name of the module
 * @param command Event (Start, Stop, Configure, ...)
 */
void NMGModule::commandReceived( QString moduleName, NMGCommandType command, const QString & data )
{
	cout << "Command not handled by " << qPrintable( getModuleGUIName() );
	cout << " from " << qPrintable( moduleName ) << " (command: " << command << ")." << endl;
}

/**
 * Executed when a module execution made by this module has finished and the result arrives
 * @param moduleName Name of the module
 * @param data XML document with the info between the tags <data> (including the tags)
 */
void NMGModule::moduleExecutionResults( QString moduleName, QString data )
{
	cout << "Module execution results not handled by " << qPrintable( getModuleGUIName() );
	cout << " from " << qPrintable( moduleName ) << " having data: " << qPrintable( data ) << endl;
}

/**
 * Sets the parameter for the execution (used for request or subscriptions)
 * @param data XML document with root data
 * @return True if all is ok or 0 if parsing the data was not succesful
 */
bool NMGModule::setExecutionData( QString data )
{
	cout << "setExecutionData not defined at " << qPrintable( getModuleGUIName() );
	cout << " (data: " << qPrintable( data ) << ")." << endl;
	return false;
}

/**
 * Request the execution of the module indicated passing as arg the data
 * @param moduleName Module name wanted to be executed
 * @param data XML document with root <data>
 * @return Identifier of the created module or "" if an error ocurred
 */
QString NMGModule::request( QString moduleName, QString data )
{
	return NMGModuleManager::self()->getCommunicationManager().request( this, moduleName, data );
}

/**
 * End the execution of a request
 * @param id
 */
void NMGModule::endRequest( QString id )
{
	NMGModuleManager::self()->getCommunicationManager().endRequest( id );
}

/**
 * Notify the end of the execution of the current module passing as arg the returned data
 * @param data XML document with root <data>
 */
void NMGModule::endGUIRequest( QString data )
{
	NMGModuleManager::self()->emitFinishedGUIRequest(currentTestId, data);
}

/**
 * Subscribes a module to the events of the caller module
 * @param moduleNameSubscriptor Module that will be created and subscribed to the events of the caller module
 * @param data XML document from the contructor of the module (root -> <data>)
 * @return Identifier of the created module or "" if an error ocurred
 */
QString NMGModule::subscribe( QString moduleNameSubscriptor, QString data )
{
	return NMGModuleManager::self()->getCommunicationManager().subscribe( this, moduleNameSubscriptor, data );
}

/**
 * Ends a subscription
 * @param id Identifier of the module subscriptor
 */
void NMGModule::endSubscribe( QString id )
{
	NMGModuleManager::self()->getCommunicationManager().endSubscribe( this, id );
}

/**
 * Executes the module with the indicated data
 * @param moduleName Name of the module
 * @param data XML document for the contructor of the module (root -> <data>)
 * @return Id if all OK or "" if error
 */
QString NMGModule::executeModule( QString moduleName, QString data )
{
	return NMGModuleManager::self()->getCommunicationManager().executeModule( moduleName, data );
}

/**
 * Returns true if exists a module with the specified name
 * @param moduleName Name of the module
 * @return
 */
bool NMGModule::existsModule( QString moduleName )
{
	return NMGModuleManager::self()->existsModule( moduleName );
}

/**
 * Executed whan a new station is created or modified
 * @param station Station created after closing the host information widget
 */
void NMGModule::notifyNewStation( NMGStation * )
{}

/**
 * Returns the test id of the Plugin
 * @return
 */
QString NMGModule::getTestId()
{
	return currentTestId;
}

/**
 * Sets the test id of the plugin
 * @param newTestId New test identifier
 */
void NMGModule::setTestId( QString newTestId )
{
	currentTestId = newTestId;
}


/**
 * Start routine (for compatibility with the NMModuleManager)
 * @param
 */
void NMGModule::start( void * )
{}

/**
 * Stop routine (for compatibility with the NMModuleManager)
 * @param
 */
void NMGModule::stop( void * )
{}

/**
 * Status routine (for compatibility with the NMModuleManager)
 * @param
 */
void NMGModule::status( void * )
{}

/**
 * Configure routine (for compatibility with the NMModuleManager)
 * @param
 */
void NMGModule::configure( void * )
{}

/**
 * Returns the key of the module for the field station manager
 * @return
 */
QString NMGModule::getModuleStationKey()
{
	return getName();
}

/**
 * Adds a field to the field manager
 * @param fieldId
 * @param fieldToShow
 * @param defaultValue
 */
void NMGModule::addFieldToFieldsManager( QString fieldId, QString fieldToShow, QString defaultValue )
{
	NMGHostInformationManager::self()->getFieldsManager().addField( getModuleStationKey(), fieldId, fieldToShow, defaultValue );
}

/**
 * Returns true if the station exists
 * @param hostAddressOrAlias Test host address or alias
 * @return
 */
bool NMGModule::existsStation( QString hostAddressOrAlias )
{
	return NMGHostInformationManager::self()->existsStation( hostAddressOrAlias );
}

/**
 * Gets a station
 * @param hostAddressOrAlias Test host address or alias
 * @return
 */
NMGStation * NMGModule::getStation( QString hostAddressOrAlias )
{
	return NMGHostInformationManager::self()->getStation( hostAddressOrAlias );
}

/**
 * Adds a station
 * @param
 */
void NMGModule::addStation( NMGStation *station )
{
	return NMGHostInformationManager::self()->addStation( station );
}

/**
 * Removes a station
 * @param hostAddressOrAlias
 */
void NMGModule::removeStation( QString hostAddressOrAlias )
{
	return NMGHostInformationManager::self()->removeStation( hostAddressOrAlias );
}

/**
 * Creates a new widget with a combo box containing the hosts and a button to show the station widget
 * @param parent Parent of the widget
 * @param name Name of the object
 * @return
 */
NMGHostInformationComboWidget * NMGModule::createHostComboWidget( QWidget *parent, const char *name )
{
	return NMGHostInformationManager::self()->createHostComboWidget( parent, name );
}

/**
 * Returns the tab widget manager
 * @return
 */
NMGModTabWidgetManager & NMGModule::getTabWidgetManager()
{
	return tabWidgetManager;
}

/**
 * Returns the module preferences manager
 * @return
 */
NMGModulePreferencesManager & NMGModule::getPreferencesManager()
{
	return preferencesManager;
}
