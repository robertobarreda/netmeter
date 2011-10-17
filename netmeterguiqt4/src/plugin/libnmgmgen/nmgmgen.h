/***************************************************************************
 *   Copyright (C) 2005 by Ernest Nieto                                    *
 *   ernest.nieto@gmail.com                                                *
 *   Copyright (C) 2007 by Joan Sala                                       *
 *   jsala@ac.upc.edu                                                      *
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
#ifndef NMGMGEN_H
#define NMGMGEN_H

#include <nmgmodule.h>
#include <QMenu>

#include "nmgmgenwidget.h"
#include "nmgmgenflowmanager.h"
#include "nmgmgenflow.h"
#include "nmgmgenxmltags.h"


#define MOD_CLASS		NMGMGen
#define MOD_NAME		"NMGMGen"
#define MOD_CAPABILITIES	"test"
#define MOD_VERSION		"0.1"
#define CORE_MATCHING		"MGEN"
#define GUI_NAME		"Traffic Generator"


/**
@author Ernest Nieto
*/
class NMGMGen : public NMGModule
{
public:
    
    NMGMGen();
    ~NMGMGen();
    
    void initModuleGUI();
    void removeModuleGUI();
    QWidget * getWidget();
    QMenu * getPopupMenu();
    QToolBar * getToolBar();
    void errorReceived(int errorType, QString errorMessage);
    void statusReceived(QString data);
    void dataReceived(QString data);
    
private:
    void createWidget();
    
private:
    NMGMGenWidget * widget;
    NMGMGenFlowManager flowManager;
};


NMMODULE_CLASS(MOD_CLASS)
NMMODULE_NAME((char *) MOD_NAME)
NMMODULE_CAPABILITIES((char *) MOD_CAPABILITIES)
NMMODULE_VERSION((char *) MOD_VERSION)
NMMODULE_CORENAME((char *)CORE_MATCHING)
NMMODULE_GUINAME((char *)GUI_NAME)
NMMODULE_MODULEICON((char *)"mgenicon.png")
NMMODULE_TOOLTIP((char *) "Module for the MGEN utility to study the one way delay")

#endif
