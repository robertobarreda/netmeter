/***************************************************************************
 *   Copyright (C) 2005 by Pau Capella   *
 *   pcapella@ac.upc.edu   *
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

#ifndef NMMODULEFACTORY_H
#define NMMODULEFACTORY_H

#include<iostream>
#include "nmparent.h"
#include "nmstring.h"
#include "nmmodule.h"
#include "nmmoduleinterface.h"


typedef char* (*NMMODULE_TEXT)();
typedef NMModuleInterface* (*NMMODULE_INTERFACE)();

/**
@author Pau Capella
@brief Factory than help to make modules.
*/

class NMModuleFactory : public NMParent{

public:
    
    NMModuleFactory(NMParent *nm_parent);
    ~NMModuleFactory();
    
    virtual NMModule* makeModule(const NMString nms_path);
    NMModuleInterface *makePluginInterface(void *handle);    
    virtual NMModule* createNewModule(NMString name, NMParent * parent);    
    virtual void makeModulePlantilla(void *, NMModule* ) { };
};

#endif
