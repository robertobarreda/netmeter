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
#include "nmgmodulecommunicationmanager.h"
#include "nmgmodulemanager.h"

#include <nmgapplicationinformation.h>
#include <nmgmainwindow.h>

/**
 * Contructor of the class
 * @param parent Qt'parent parameter
 * @param name Qt's name parameter
 * @return
 */
NMGModuleCommunicationManager::NMGModuleCommunicationManager ( QObject *parent, const char *name )
		: QObject ( parent )
{
	setObjectName ( name );
}

/**
 * Destructor of the class
 * @return
 */
NMGModuleCommunicationManager::~NMGModuleCommunicationManager()
{}

/**
 * Request the execution of the module indicated passing as arg the data
 * @param mod Module which wants the result of the execution
 * @param moduleName Module name wanted to be executed
 * @param data XML document with root <data>
 * @return Identifier of the created module or "" if an error ocurred
 */
QString NMGModuleCommunicationManager::request ( NMGModule * mod, QString moduleName, QString data )
{
	if ( !NMGModuleManager::self()->existsModule ( moduleName ) ) return "";

	QString sourceModuleId = NMGModuleManager::self()->getInstanceManager().getTestIdByModule ( mod );
	QString moduleCreatedId = NMGModuleManager::self()->createModuleInterface ( moduleName );
	if ( moduleCreatedId.isNull() ) return "";

	NMGModule * executor = NMGModuleManager::self()->getInstanceManager().getModule ( moduleCreatedId );
	if ( !executor ) return "";

	requestMap.insert ( moduleCreatedId, sourceModuleId );

	if ( !executor->setExecutionData ( data ) )
	{
		requestMap.remove ( moduleCreatedId );
		NMGModuleManager::self()->getInstanceManager().removeModule ( moduleCreatedId );
		NMGModuleManager::self()->deletePluginInterface ( executor );
		return "";
	}

	return moduleCreatedId;
}

/**
 * Ends a the execution of a request
 * @param id Identifier of the module requested
 */
void NMGModuleCommunicationManager::endRequest ( QString id )
{
	if ( !isRequested ( id ) ) return;

	NMGModule * executor = NMGModuleManager::self()->getInstanceManager().getModule ( id );
	if ( !executor ) return;

	requestMap.remove ( id );
	NMGModuleManager::self()->getInstanceManager().removeModule ( id );
	NMGModuleManager::self()->deletePluginInterface ( executor );
}

/**
 * Returns true if the id is requested by a module
 * @param id Id of the module requested
 * @return
 */
bool NMGModuleCommunicationManager::isRequested ( QString id )
{
	return requestMap.contains ( id );
}

/**
 * Returns the id destination of a request
 * @param id Id of the module requested
 * @return Id or "" if it wasnt' requested
 */
QString NMGModuleCommunicationManager::getRequestDestination ( QString id )
{
	NMGModuleCommunicationManagerRequestMapIterator it = requestMap.find ( id );
	return ( it == requestMap.end() ) ? "" : *it;
}

/**
 * Subscribes a module to the events (commands) of the caller module
 * @param mod Module which will generate the events
 * @param moduleNameSubscribed Module that will be created and subscribed to the events of the caller module
 * @param data XML document for the contructor of the module (root -> <data>)
 * @return Identifier of the created module or "" if an error ocurred
 */
QString NMGModuleCommunicationManager::subscribe ( NMGModule * mod, QString moduleNameSubscribed, QString data )
{
	if ( !NMGModuleManager::self()->existsModule ( moduleNameSubscribed ) ) return "";

	QString subscriberModuleId = NMGModuleManager::self()->getInstanceManager().getTestIdByModule ( mod );
	QString moduleCreatedId = NMGModuleManager::self()->createModuleInterface ( moduleNameSubscribed );
	if ( moduleCreatedId.isNull() ) return "";

	NMGModule * subscribed = NMGModuleManager::self()->getInstanceManager().getModule ( moduleCreatedId );
	if ( !subscribed ) return "";

	if ( !subscribed->setExecutionData ( data ) )
	{
		NMGModuleManager::self()->getInstanceManager().removeModule ( moduleCreatedId );
		NMGModuleManager::self()->deletePluginInterface ( subscribed );
		return "";
	}

	getSubscriptionListOrCreate ( subscriberModuleId )->addSubscription ( moduleCreatedId );

	NMGApplicationInformation::getApplicationInformation()->getMainWindow()->addWidgetFromExistingModule ( subscribed );

	return moduleCreatedId;
}

/**
 * Ends a subscription
 * @param mod Module which was generating the events
 * @param id Identifier of the subscribed module
 */
void NMGModuleCommunicationManager::endSubscribe ( NMGModule * mod, QString id )
{
	QString subscriberModuleId = NMGModuleManager::self()->getInstanceManager().getTestIdByModule ( mod );
	if ( subscriberModuleId.isEmpty() ) return;

	NMGModuleSubscriptionList * list = getSubscriptionList ( id );
	if ( !list ) return;

	list->removeSubscription ( id );
}

/**
 * Removes pending subscriptions to or from any module before deleting or unloading the module identified by ID
 * @param id Identifier of the module that will be removed
 */
void NMGModuleCommunicationManager::endPendingSubscriptions ( QString id )
{
	if ( id.isEmpty() ) return;

        // Whe must check all the lists in the map
	NMGModuleCommunicationManagerSubscriptionMapIterator it =  subscriptionMap.begin();
	while ( it != subscriptionMap.end() )
	{
		NMGModuleSubscriptionList * list = it.value();
		if ( it.key() == id )
		{
			// Remove subscription list because we removed the module that made the subscritpion (subscriptor)
			delete list;
			it = subscriptionMap.erase ( it );
		}
		else
		{
			if ( list )
			{
				// Check if the module is been subscribed in this list and remove the subscription
				list->removeSubscription ( id );
				if ( list->isEmpty() )
				{
                                        // Delete the list and remove it from the map if it's empty
					delete list;
					it = subscriptionMap.erase ( it );
				}
				else ++it;
			}
			else ++it;
		}
	}
}

/**
 * Informs to the subscripted if there are any of a new command
 * @param mod Module subscripted
 * @param commandType
 */
bool NMGModuleCommunicationManager::commandExecuted ( NMGModule * mod, NMGCommandType commandType,
        const QString & data, const QString & receiver )
{
	if ( !mod ) return false;

	QString subscriberModuleId = NMGModuleManager::self()->getInstanceManager().getTestIdByModule ( mod );
	if ( subscriberModuleId.isEmpty() ) return false;

	NMGModuleSubscriptionList * list = getSubscriptionList ( subscriberModuleId );
	if ( !list ) return false;
	if ( !list->existsSubscription ( receiver ) ) return false;

	return list->notifyExecutedCommand ( mod->getModuleName(), commandType, data, receiver );
}

/**
 * Creates a new instance of the module
 * @param moduleName
 * @param data
 * @return
 */
QString NMGModuleCommunicationManager::executeModule ( QString moduleName, QString data )
{
	if ( !NMGModuleManager::self()->existsModule ( moduleName ) ) return "";

	QString moduleCreatedId = NMGModuleManager::self()->createModuleInterface ( moduleName );
	if ( moduleCreatedId.isNull() ) return "";

	NMGModule * modCreated = NMGModuleManager::self()->getInstanceManager().getModule ( moduleCreatedId );
	if ( !modCreated ) return "";

	if ( !modCreated->setExecutionData ( data ) )
	{
		NMGModuleManager::self()->getInstanceManager().removeModule ( moduleCreatedId );
		NMGModuleManager::self()->deletePluginInterface ( modCreated );
		return "";
	}

	NMGApplicationInformation::getApplicationInformation()->getMainWindow()->addWidgetFromExistingModule ( modCreated );

	return moduleCreatedId;
}

/**
 * Returns the subscription list for the module with id indicated
 * @param id Identifier of the module
 * @return Returns the module list, if there is no module list for the module, a new one is created and added
 */
NMGModuleSubscriptionList * NMGModuleCommunicationManager::getSubscriptionListOrCreate ( QString id )
{
	NMGModuleSubscriptionList * list = getSubscriptionList ( id );
	if ( !list )
	{
		list = new NMGModuleSubscriptionList ();
		subscriptionMap.insert ( id, list );
	}
	return list;
}

/**
 * Returns the subscription list for the module with id indicated
 * @param id Identifier of the module
 * @return Returns the module list, if there is no module list returns 0
 */
NMGModuleSubscriptionList * NMGModuleCommunicationManager::getSubscriptionList ( QString id )
{
	NMGModuleCommunicationManagerSubscriptionMapIterator it = subscriptionMap.find ( id );
	return ( it == subscriptionMap.end() ) ? 0 : *it;
}

#include "nmgmodulecommunicationmanager.moc"
