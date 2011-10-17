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
#ifndef NMGWIDGETFROMXML_H
#define NMGWIDGETFROMXML_H

#include "nmgwidgetfromxmltags.h"
#include "nmgwidgetfromxmlinputdata.h"
#include "nmgwidgetfromxmlinterface.h"
#include "nmgwidgetfromxmleventhandler.h"

#include "nmgwidgetfromxmlvariable.h"
#include "nmgwidgetfromxmlbutton.h"
#include "nmgwidgetfromxmltextline.h"
#include "nmgwidgetfromxmlbuttongroup.h"

#include <QMap>
#include <QDomNode>
#include <QDomDocument>
#include <QString>
#include <QWidget>
#include <QLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QLineEdit>
#include <QLabel>
#include <QFrame>
#include <QValidator>
#include <QObject>
#include <QGridLayout>
#include <QBoxLayout>
#include <QHideEvent>
#include <QAbstractButton>

typedef QMap<QString, NMGWidgetFromXMLInputWidget *> QInputMap;
typedef QInputMap::iterator QInputMapIterator;

#define ACTION_VALUE_PREFIX	"$"

#define DEFAULT_MARGIN	5
#define DEFAULT_SPACING	5
#define DEFAULT_TEXT	"(null)"

#define DEFAULT_NAME	"unnamed_nmg"


#define getAttr(node,attr)	node.toElement().hasAttribute(attr)?node.toElement().attribute(attr):""

/**
@short XML Reader for creating interfaces
*/
class NMGWidgetFromXML : public NMGWidgetFromXMLInterface, public NMGWidgetFromXMLInputData
{
		Q_OBJECT
	public:
		NMGWidgetFromXML( QWidget *parent = 0, const char *name = 0 );
		~NMGWidgetFromXML();
		bool setXmlFile( const QString fileName );
		QString getInput( QString id );
		void setInput( QString id, QString value );
		void addInput( NMGWidgetFromXMLInputWidget *iw );
		QAbstractButton * getButton( QString id );
		void cleanAllInput();
		void setModal();
		void hideWidget();
		void hideEvent( QHideEvent * );
		void show();

	signals:
		void widgetHidden();

	private:
		void showContentFile( QDomNode &node, int indent = 0 );

		void createGUIFromXML();
		void readGUIInfo( QDomNode &parent );
		void readGUIDescription( QDomNode &parent );
		void readGUIActions( QDomNode &parent );

		void parseChilds( QDomNode &parent, QWidget* widgetParent, QBoxLayout * layout = 0, 
			bool isTopLevel = true, QGridLayout * gridLayout = 0 );

		void createVariableFromNode( QDomNode &parent );
		QBoxLayout * createLayoutFromNode( QDomNode &parent, QWidget* widgetParent, 
			bool isTopLevel = false );
		QGridLayout * createGridLayoutFromNode( QDomNode &parent, QWidget* widgetParent, 
			bool isTopLevel = false );

		QPushButton * createButtonFromNode( QDomNode &parent, QWidget* widgetParent );
		QLineEdit * createTextFromNode( QDomNode &parent, QWidget* widgetParent );
		QLabel * createLabelFromNode( QDomNode &parent, QWidget* widgetParent );
		QFrame * createFrameFromNode( QDomNode &parent, QWidget* widgetParent );
		QGroupBox * createOptionsFromNode( QDomNode &parent, QWidget* widgetParent );

		void getGlobalAttributes( QDomNode &node );

		QValidator * createValidator( QString validatorType, QObject * parent = 0 );

	private:
		NMGWidgetFromXMLEventHandler eventHandler;

		QDomDocument domTree;
		uint numButtons;
		uint numLayouts;
		QWidget *parentWidget;

		QInputMap inputMap;

		/* Attributes of the last node */
		int lastRow, lastCol;
		QString lastId;
		int lastGridRows, lastGridCols;
};

#endif
