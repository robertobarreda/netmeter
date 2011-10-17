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
#include "nmgtaskwidget.h"

#include <iostream>
using namespace std;

NMGTaskWidget::NMGTaskWidget ( QWidget *parent, const char *name )
		: QWidget ( parent )
{
	setObjectName ( name );

	id = 0;	

	layout = new QHBoxLayout ( this );
 	layout->setMargin( 0 );
	layout->setSpacing( 0 );

	label = new QLabel ( "Task Name", this );
	layout->addWidget ( label );

	bar = new QProgressBar ( this );
	bar->setTextVisible ( true );
	bar->setRange ( 0, MAXPROGRESS );
	bar->setMaximumWidth ( MAXWIDTHBAR );
	bar->setMinimumWidth ( MINWIDTHBAR );
	layout->addWidget ( bar );
}

NMGTaskWidget::~NMGTaskWidget()
{}

void NMGTaskWidget::mousePressEvent ( QMouseEvent * e )
{
	if ( e->button() == Qt::LeftButton )
		cout << "S'hauria de mostrar info (si escau) sobre la taska amb id ";
		cout << qPrintable ( id->text() ) << endl;
}

void NMGTaskWidget::setToolTip ( QString toolTip )
{
	bar->setToolTip( toolTip );
}

void NMGTaskWidget::setId ( QAction * i )
{
	id = i;
}

QAction * NMGTaskWidget::getId()
{
	return id;
}

void NMGTaskWidget::setName ( QString s )
{
	label->setText ( s );
	setToolTip ( s );
}

void NMGTaskWidget::advance()
{
	bar->setValue ( bar->value() +1 );
}

#include "nmgtaskwidget.moc"
