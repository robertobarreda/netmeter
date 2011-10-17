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
#include "nmgmodulepreferencesmanager.h"
#include "nmgmodule.h"

#include <nmgpreferencesmanager.h>

/**
 * Contructor of the class
 * @param mod
 * @return
 */
NMGModulePreferencesManager::NMGModulePreferencesManager ( NMGModule * mod )
{
	module = mod;
}

/**
 * Destroyer of the class
 * @return
 */
NMGModulePreferencesManager::~NMGModulePreferencesManager()
{}

/**
 * Returns true if exists the value identified by id
 * @param id
 * @return
 */
bool NMGModulePreferencesManager::existsValue ( QString id )
{
	return NMGPreferencesManager::self()->existsValue ( transformId ( id ) );
}

/**
 * Removes the value identified by id if exists
 * @param id
 */
void NMGModulePreferencesManager::removeValue ( QString id )
{
	NMGPreferencesManager::self()->existsValue ( transformId ( id ) );
}

/**
 * Adds a value identified by id with value val
 * @param id
 * @param val
 */
void NMGModulePreferencesManager::addValue ( QString id, QString val )
{
	NMGPreferencesManager::self()->addValue ( transformId ( id ), val );
}

/**
 * Retruns the value identified by id (or "" if there is no value with the id)
 * @param id
 * @return
 */
QString NMGModulePreferencesManager::getValue ( QString id )
{
	return NMGPreferencesManager::self()->getValue ( transformId ( id ) );
}

/**
 * Transforms the id adding the module name
 * @param id
 * @return
 */
QString NMGModulePreferencesManager::transformId ( QString id )
{
	return module->getModuleName() +"_"+id;
}
