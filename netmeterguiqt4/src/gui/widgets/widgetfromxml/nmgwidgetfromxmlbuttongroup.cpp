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
#include "nmgwidgetfromxmlbuttongroup.h"

#include <QObject>
#include <QList>

/**
 * Constructor of the class
 * @param parent QT's parent parameter
 * @param name QT's name parameter
 * @return
 */
NMGWidgetFromXMLButtonGroup::NMGWidgetFromXMLButtonGroup( QWidget *parent, const char *name )
		: QGroupBox( name, parent ), NMGWidgetFromXMLInputWidget()
{
	layout = 0;
}

/**
 * Destructor of the class
 * @return
 */
NMGWidgetFromXMLButtonGroup::~NMGWidgetFromXMLButtonGroup()
{
}

/**
 * Returns the id of the button group
 * @return Id of the button group
 */
QString NMGWidgetFromXMLButtonGroup::getId()
{
	return objectName();
}

/**
 * Rerurn the text of the selected item of the QRadioButton Group
 * @return
 */
QString NMGWidgetFromXMLButtonGroup::getInput()
{
	QList<QRadioButton *> l = findChildren<QRadioButton *>();
	QString res = "";
	int i = 0;

	while ( res.isEmpty() and l.at( i ) )
	{
		if ( l.at( i )->isChecked() ) res = l.at( i )->text();
		i++;
	}

	//l.clear();
	while ( !l.isEmpty() )
		delete l.takeFirst();
	return res;
}

/**
 * Sets the item selected of the QRadioButton Group
 * @param value
 */
void NMGWidgetFromXMLButtonGroup::setInput( QString value )
{
	QList<QRadioButton *> l = findChildren<QRadioButton *>();

	QRadioButton * b;
	int i = 0;

	while (( b = ( QRadioButton * ) l.at( i ) ) and b->text() != value ) i++;
	b->setChecked( true );
	//l.clear();
	while ( !l.isEmpty() )
		delete l.takeFirst();
}

/**
 *
 * @param l
 */
void NMGWidgetFromXMLButtonGroup::setLayout( QLayout * l )
{
	layout = l;
	this->setLayout( layout );
}

void NMGWidgetFromXMLButtonGroup::newOption( QString value, bool checked )
{
	QRadioButton * b = new QRadioButton( this );
	b->setObjectName( QString( objectName() ) + CHILDS_SUFFIX );
	b->setText( value );
	b->setChecked( checked );
	layout->addWidget( b );
}


#include "nmgwidgetfromxmlbuttongroup.moc"
