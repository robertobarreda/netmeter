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
#include "nmgmoduleparent.h"

/**
 * Constructor of the class
 * @param nms_name
 * @param nm_parent
 * @return
 */
NMGModuleParent::NMGModuleParent ( NMString nms_name, NMParent *nm_parent )
	: NMModule ( nms_name, nm_parent )
{}

/**
 * Destructor of the class
 * @return
 */
NMGModuleParent::~NMGModuleParent()
{}


/*!
    \fn NMModule::getPluginNameGUI()
    \brief Get plugin GUI name (if defined).
    \return Name.
*/
NMString NMGModuleParent::getPluginNameGUI()
{
	return pluginnameGUI;
}

/*!
    \fn NMGModuleParent::getPluginNameCore()
    \brief Get plugin Core name (if defined).
    \return Name.
*/
NMString NMGModuleParent::getPluginNameCore()
{
	return pluginnameCore;
}

/*!
    \fn NMGModuleParent::setPluginNameGUI (const NMString& modulename)
    \brief Set plugin GUI name.
    \param modulename Classname plugin
*/
void NMGModuleParent::setPluginNameGUI ( const NMString& modulename )
{
	pluginnameGUI = modulename;
}

/*!
    \fn NMGModuleParent::setPluginNameCore (const NMString& modulename)
    \brief Set plugin Core name.
    \param modulename Classname plugin
*/
void NMGModuleParent::setPluginNameCore ( const NMString& modulename )
{
	pluginnameCore = modulename;
}

/**
 * Returns the module icon name
 * @return
 */
NMString NMGModuleParent::getModuleIcon()
{
	return moduleIcon;
}

/**
 * Sets the module icon name
 * @param modIcon
 */
void NMGModuleParent::setModuleIcon ( const NMString& modIcon )
{
	moduleIcon = modIcon;
}

/**
 * Returns the module tooltip
 * @return
 */
NMString NMGModuleParent::getModuleToolTip()
{
	return moduleTooltip;
}

/**
 * Sets the module tooltip
 * @param toolTip
 */
void NMGModuleParent::setModuleToolTip ( const NMString& toolTip )
{
	moduleTooltip =toolTip;
}

/**
 * Returns true if the moduleParent is the parent of the mod module instance
 * @param mod
 * @return
 */
bool NMGModuleParent::hasModuleInterface ( NMModuleInterface * mod )
{
//cout << "NMGModuleParent::hasModuleInterface @ "<<getPluginNameGUI()<<" ... " <<mod << endl;
	if ( !mod ) return false;
	list<NMModuleInterface *>::iterator it;
//cout << "... "<<lnmi_interfaces.size()<<endl;
	for ( it = lnmi_interfaces.begin(); it!=lnmi_interfaces.end(); ++it )
	{
//cout << " - " << *it << endl;
		if ( *it==mod ) return true;
	}
//cout << "..."<<endl;
	return false;
}
