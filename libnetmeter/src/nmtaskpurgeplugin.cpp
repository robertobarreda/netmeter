/*
 *   $Id: nmtaskpurgeplugin.cpp,v 1.1.1.1 2007-05-26 17:41:22 rserral Exp $
 *
 *   Copyright (c) 2006 Pau Capella
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "nmtaskpurgeplugin.h"
#include <nmmodulemanager.h>

NMTaskPurgePlugin::NMTaskPurgePlugin( int testid, NMString plugin, const char *name, NMParent *parent)
 : NMTask( testid, plugin, "", NMDateTime(), name, parent)
{
}


NMTaskPurgePlugin::~NMTaskPurgePlugin()
{
}

void NMTaskPurgePlugin::run()
{
	NMModuleInterface *iface = NMModuleManager::self()->getModule(plugin())->findPluginInterface(testId());
	NMModuleManager::self()->getModule(plugin())->removePluginInterface(iface);
}


