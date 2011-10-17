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
#ifndef NMGMAIN_H
#define NMGMAIN_H

#include <nmgconnection.h>
#include <nmgapplicationinformation.h>
#include <nmghostinformationmanager.h>
#include <nmgpreferencesmanager.h>
#include <nmgconfigurationfilemanager.h>
#include <nmmodulemanager.h>
#include <nmgmodule.h>
#include <nmgmodulemanager.h>

#include <QApplication>

/**
It initializes all the variables and destroys the objects dynamically created

@author Ernest Nieto
*/
class NMGMain
{
	public:
		NMGMain ( QApplication &a );
		~NMGMain();

	private:
		void loadModules();

		NMGModuleManager * moduleManager;
		NMGConnection * nmConnection;
		NMGHostInformationManager * hostManager;
		NMGPreferencesManager * preferencesManager;
		NMGConfigurationFileManager * fileManager;
};

#endif
