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
#include "nmgmoduleutils.h"

#include <nmgapplicationinformation.h>
#include <nmgimagemanager.h>

/**
 * Construtor of the class
 * @return
 */
NMGModuleUtils::NMGModuleUtils()
{}

/**
 * Destroyer of the class
 * @return
 */
NMGModuleUtils::~NMGModuleUtils()
{}

/**
 * Returns the main windows of the application
 * @return
 */
QMainWindow * NMGModuleUtils::getMainWindow()
{
	return ( QMainWindow * ) NMGApplicationInformation::getApplicationInformation()->getMainWindow();
}

/**
 * Returns the pixmap of name
 * @param name file name of the image
 * @return
 */
QPixmap * NMGModuleUtils::getPixmap ( const QString & name )
{
	return NMGImageManager::loadPixmap ( name );
}

