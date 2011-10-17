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
#include "nmgwidgetfromxmlaction.h"

#include "nmgwidgetfromxml.h"
#include <nmgmodulemanager.h>
#include <nmgconnection.h>

//#include <qregexp.h>

#include <iostream>
using namespace std;

/**
 * Contructor of the class
 * @param parent QDomNode with the tag action
 * @return
 */
NMGWidgetFromXMLAction::NMGWidgetFromXMLAction( NMGWidgetFromXMLInterface * wi, NMGWidgetFromXMLInputData * id, QDomNode &parent )
{
	widgetInterface  = wi;
	inputData = id;
	//read attributes if interestes
	for ( QDomNode node = parent.toElement().firstChild(); !node.isNull(); node = node.nextSibling() )
	{
		if ( node.isElement() )
		{
			QString name = node.nodeName();
			if ( name == TAG_SEND ) addActionSend( node );
			else if ( name == TAG_HIDE ) addActionHide( node );
			else if ( name == TAG_ASSIGN ) addActionAssign( node );
			else if ( name == TAG_VALIDATION ) addActionValidation( node );
			else cerr << "Unkonwn action description tag " << qPrintable( name ) << endl;
		}
	}
}


/**
 * Destructor of the class
 * @return
 */
NMGWidgetFromXMLAction::~NMGWidgetFromXMLAction()
{
	while ( !actionList.isEmpty() )
	{
		ActionInfo * ai = actionList.takeFirst();
		while ( !ai->list.isEmpty() )
			delete ai->list.takeFirst();
		delete ai;
	}
}

/**
 * Sets the source of input data
 * @param id Source of input data for the class
 */
void NMGWidgetFromXMLAction::setInputData( NMGWidgetFromXMLInputData * id )
{
	inputData = id;
}

/**
 * Executes the set of actions which are in the list of the class
 */
void NMGWidgetFromXMLAction::execute()
{
	foreach( ActionInfo * ai, actionList )
	{
		if ( ai->action == Send )
		{
			QString res = putValues( ai->data );
			NMGCommandType type = Configure;
			if ( ai->type == "start" ) type = Start;
			else if ( ai->type == "stop" ) type = Stop;
			else if ( ai->type == "configure" ) type = Configure;
			else if ( ai->type == "status" ) type = Status;
			else cerr << "Message type not known: " << qPrintable( ai->type ) << "." << endl;

			QString testId = NMGModuleManager::self()->getInstanceManager().getModuleByWidget( widgetInterface )->getTestId();
			NMGConnection::getConnection()->sendCommand( type, ai->module, testId, res );
		}
		else if ( ai->action == Hide )
		{
			widgetInterface->hideWidget();
		}
		else if ( ai->action == Assign )
		{
			inputData->setInput( ai->type, ai->data );
		}
		else if ( ai->action == Validation )
		{
			QString message = "";

			foreach( ValidationItem * vi, ai->list )
			{
				QString value = inputData->getInput( vi->id );
				if ( vi->type == TYPE_NOTNULL and value.replace( " ", "" ) == "" )
					message += ( vi->message + "\n" );
			}
			if ( message != "" )
			{
				QMessageBox::critical( 0, "Validation error", message );
				return;
			}
		}
	}
}

/**
 * Adds a send action to the action list
 * @param parent QDomNode with the tag send
 */
void NMGWidgetFromXMLAction::addActionSend( QDomNode &parent )
{
	QString type = getAttr( parent, ATTR_TYPE );
	QString module = getAttr( parent, ATTR_MODULE );
	QDomNode node = parent.toElement().firstChild();
	if ( node.isElement() and node.nodeName() == TAG_DATA )
	{
		ActionInfo * ai = new ActionInfo();
		ai->action = Send;
		ai->type = type;
		ai->module = module;
		ai->data = getTree( node );
		actionList.append( ai );
	}
	else cerr << "Tag " << TAG_DATA << " expected for the send action." << endl;
}

/**
 * Adds a hide action to the action list
 * @param parent QDomNode with the tag hide
 */
void NMGWidgetFromXMLAction::addActionHide( QDomNode & )
{
	ActionInfo * ai = new ActionInfo();
	ai->action = Hide;
	actionList.append( ai );
}

/**
 * Adds a assign action to an input widget (variable, textline, ...)
 * @param parent QDomNode with the tag assign
 */
void NMGWidgetFromXMLAction::addActionAssign( QDomNode &parent )
{
	ActionInfo * ai = new ActionInfo();
	ai->action = Assign;
	QString id = getAttr( parent, ATTR_ID );
	QString value = getAttr( parent, ATTR_VALUE );
	ai->type = id;
	ai->data = value;
	actionList.append( ai );
}

/**
 * Adds a validation action for a widget
 * @param parent QDomNode with the tag validation
 */
void NMGWidgetFromXMLAction::addActionValidation( QDomNode &parent )
{
	ActionInfo * ai = new ActionInfo();
	ai->action = Validation;
	for ( QDomNode node = parent.toElement().firstChild(); !node.isNull(); node = node.nextSibling() )
	{
		if ( node.isElement() )
		{
			QString name = node.nodeName();
			if ( name == TAG_VALIDATE )
			{
				ValidationItem * i = new ValidationItem();
				i->id = getAttr( node, ATTR_ID );
				i->type = getAttr( node, ATTR_TYPE );
				i->message = getAttr( node, ATTR_MESSAGE );
				ai->list.append( i );
			}
			else cerr << "Unkonwn validation tag " << qPrintable( name ) << endl;
		}
	}
	actionList.append( ai );
}

/**
 *
 * @param parent
 * @return
 */
QString NMGWidgetFromXMLAction::getTree( QDomNode & parent )
{
	QString res = "";
	for ( QDomNode node = parent.toElement().firstChild(); !node.isNull(); node = node.nextSibling() )
	{
		if ( node.isElement() )
		{
			QString name = node.nodeName();
			res += "<" + name + ">";
			if ( node.hasChildNodes() ) res += getTree( node );
			res += "</" + name + ">";
		}
		else if ( node.isText() )
		{
			res += node.toText().nodeValue();
		}
	}
	return res;
}

/**
 *
 * @param request
 * @return
 */
QString NMGWidgetFromXMLAction::putValues( QString request )
{
	QString res = request;
	QString toReplace;
	int pos = 0, prefixLength = QString( ACTION_VALUE_PREFIX ).length();
//Old version. Provably TODO (remove):
//	while((pos=res.find(QRegExp("(\\$)([a-zA-Z0-9]*)")))!=-1) 
	while (( pos = res.indexOf( ACTION_VALUE_PREFIX, pos ) ) != -1 )
	{
		toReplace = "";
		for ( int i = pos + prefixLength; !res[i].isNull() and res[i].isLetterOrNumber(); i++ ) toReplace += res[i];
		res.replace( ACTION_VALUE_PREFIX + toReplace, inputData->getInput( toReplace ) );
	}
	return res;
}
