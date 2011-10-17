/***************************************************************************
 *   Copyright (C) 2006 by David Lacort                                    *
 *   Copyright (C) 2007 by Eduard Bellot                                   *
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
#ifndef NMGGRAPHICS_H
#define NMGGRAPHICS_H

#include <nmgmodule.h>
#include "nmggraphicswidget.h"

#define MOD_CLASS	  NMGGraphics
#define MOD_NAME          "NMGGraphics"
#define MOD_CAPABILITIES  "Analysis"
#define MOD_VERSION	  "2.0"
#define CORE_MATCHING	  "Graphics"
#define GUI_NAME          "Graphic Analysis"

/**
 * \brief NMGGraphics undertakes responsability for communicating Graphic Analysis plugin with
 * the other parts of Netmeter GUI.
 */
class NMGGraphics : public NMGModule
{

  public:

    /** \brief Default constructor. */
    NMGGraphics();

    /** \brief Default destructor. */
    ~NMGGraphics();

    /** \brief Called before loading the plugin. */ 
    void initModuleGUI();

    /** \brief Called before unloading the plugin. */
    void removeModuleGUI();

    /** \brief Returns the main graphical widget used in the plugin. */
    QWidget * getWidget();

    /** \brief Returns a specific plugin menu, if any. */
    QMenu * getPopupMenu();

    /** \brief Returns a specific plugin toolbar, if any. */
    QToolBar * getToolBar();

    /*! \brief Responds a remote call execution (SUBSCRIPTION or REQUEST).
      *  \param data request configuration in xml format
      * \return TRUE if xml request is correctly formated */
    bool setExecutionData ( QString data );
    
    /*! \brief Command received from a subscripted module
    * \param moduleName Name of the module subscripted
    * \param command Commnad executed by the module
    * \param data xml message from the subscriptor */
    void commandReceived ( QString moduleName, NMGCommandType command, const QString & data = "" );

  private:
  
    void createWidget();
  
    NMGGraphicsWidget * widget;
};

NMMODULE_CLASS(MOD_CLASS)
NMMODULE_NAME((char *) MOD_NAME)
NMMODULE_CAPABILITIES((char *) MOD_CAPABILITIES)
NMMODULE_VERSION((char *) MOD_VERSION)
NMMODULE_CORENAME((char *)CORE_MATCHING)
NMMODULE_GUINAME((char *)GUI_NAME)
NMMODULE_MODULEICON((char *)"graphicon.png")
NMMODULE_TOOLTIP((char *) "Module for the graphic visualization of tests")

#endif
