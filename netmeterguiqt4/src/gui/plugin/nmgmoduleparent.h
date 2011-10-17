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
#ifndef NMGMODULEPARENT_H
#define NMGMODULEPARENT_H

#include <nmmodule.h>

class NMGModuleParent : public NMModule
{
	public:
		NMGModuleParent ( NMString nms_name=0, NMParent *nm_parent=0 );
		~NMGModuleParent();
		NMString getPluginNameGUI();
		NMString getPluginNameCore();
		NMString getModuleIcon();
		NMString getModuleToolTip();
		void setPluginNameGUI ( const NMString& modulename );
		void setPluginNameCore ( const NMString& modulename );
		void setModuleIcon ( const NMString& moduleIcon );
		void setModuleToolTip ( const NMString& toolTip );
		bool hasModuleInterface ( NMModuleInterface * mod );

	private:
		NMString pluginnameGUI;
		NMString pluginnameCore;
		NMString moduleIcon;
		NMString moduleTooltip;
};

#endif
