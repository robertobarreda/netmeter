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
#ifndef NMGINTERPRET_H
#define NMGINTERPRET_H

#include "nmgapplicationinformation.h"
#include "nmgutil.h"

#include <QDomElement>

#include <iostream>
using namespace std;

/**
Interpret or the XML communiation between the gui and the server
*/
class NMGInterpret
{
	public:
		NMGInterpret();
		~NMGInterpret();
		QString attach();
		QMapPluginInformation * parseAttachReply ( QString message );
		QString quit ( QString id );
		QString deattach ( QString id );
		QString command ( NMGCommandType commandType, QString pluginName, QString id, QString testId, QString data );
		NMGIncomingDataInfo * parseIncomingData ( QString message );
		void parseResult ( QDomElement & root,NMGIncomingDataInfo * di );

	private:
		QMapPluginInformation * parseAtachReplyPlugins ( QDomNode &parent );
		NMGPluginInformation * parsePluginInformation ( QDomNode &parent );
		QString getStringCommandType ( NMGCommandType command );
		NMGCommandType getCommandType ( QString command );
		void parseAck ( QDomElement & root,NMGIncomingDataInfo * di );
		void parseNAck ( QDomElement & root,NMGIncomingDataInfo * di );
		void parseStatus ( QDomElement & root,NMGIncomingDataInfo * di );
		void parseError ( QDomElement & root,NMGIncomingDataInfo * di );
};

#endif
