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
#include "nmgmodulesubscriptionlist.h"
#include "nmgmodulemanager.h"

/**
 * Constructor of the class
 */
NMGModuleSubscriptionList::NMGModuleSubscriptionList() : QStringList()
{}

/**
 * Destructor of the class
 */
NMGModuleSubscriptionList::~NMGModuleSubscriptionList()
{}

/**
 * Returns true if the module with id is in the subsciption
 * @param id
 * @return TRUE if exists the subscription 
 */
bool NMGModuleSubscriptionList::existsSubscription ( QString id )
{
	return contains ( id );
}

/**
 * Adds a subscription to a module (it it doesn't exist previously)
 * @param id
 */
void NMGModuleSubscriptionList::addSubscription ( QString id )
{
	if ( !contains ( id ) ) append ( id );
}

/**
 * Removes the subscription (if existed) to a module
 * @param id
 */
void NMGModuleSubscriptionList::removeSubscription ( QString id )
{
	removeAll ( id );
}

/**
 * Notifies to the subscribed modules that a new command executed
 * @param subscriptorName
 * @param commandType
 * @param data
 * @param subscribedId
 */
bool NMGModuleSubscriptionList::notifyExecutedCommand ( QString subscriptorName, NMGCommandType commandType,
        const QString & data, const QString & subscribedId )
{
	int i;
        bool retStatus = true; 
        // TODO (rbarred) Evaluate if it's worthwhile to return the real result of mod->commandReceived or always true
	if ( ( i = indexOf ( subscribedId ) ) >= 0 )
	{
		// If the receiver is set, the message is only sent to defined.
		QString id = at ( i );
		NMGModule * mod = NMGModuleManager::self()->getInstanceManager().getModule ( id );
		//if ( mod ) return mod->commandReceived ( subscriptorName, commandType, data );
		if ( mod ) mod->commandReceived ( subscriptorName, commandType, data );
		return true;
	}

	for ( i = 0; i < size(); ++i ) // for each item in the list
	{
		// If the receiver is unset, the message is sent to all subscribed plugins in the list.
		QString id = at ( i );
        	NMGModule * mod = NMGModuleManager::self()->getInstanceManager().getModule ( id );
		//if ( mod ) retStatus = (retStatus && mod->commandReceived ( subscriptorName, commandType, data ));
                if ( mod ) mod->commandReceived ( subscriptorName, commandType, data );
	}
        return retStatus;
}
