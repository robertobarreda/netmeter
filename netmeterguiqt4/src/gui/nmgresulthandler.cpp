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
#include "nmgresulthandler.h"

#include <QWidget>
#include <QLayout>
#include <QTextEdit>
#include <QVBoxLayout>

#include <iostream>
using namespace std;

NMGResultHandler * NMGResultHandler::instance = 0;

NMGResultHandler::NMGResultHandler ( QObject *parent, const char *name )
		: QObject ( parent )
{
	setObjectName ( name );
}

NMGResultHandler::~NMGResultHandler()
{}

NMGResultHandler * NMGResultHandler::self()
{
	if ( instance == 0 ) instance = new NMGResultHandler();
	return instance;
}

void NMGResultHandler::setParentTab ( QTabWidget * p )
{
	parent = p;
}

void NMGResultHandler::showResult ( QString module, QString data )
{
	cout << "Module: " << qPrintable ( module ) << endl << "DATA: " << qPrintable ( data ) << endl;

	QWidget * tab = new QWidget ( parent );
	QVBoxLayout * layout = new QVBoxLayout ( tab );
	layout->setContentsMargins ( 1, 1, 1, 1 ); 
	layout->setSpacing ( 1 );

/* 	pixmapLabel1_2 = new QLabel( TabPage_3, "pixmapLabel1_2" );
	pixmapLabel1_2->setScaledContents( TRUE );
	TabPageLayout_3->addWidget( pixmapLabel1_2 );
	textLabel1_9 = new QLabel( TabPage_3, "textLabel1_9" );
	textLabel1_9->setFrameShape( QLabel::StyledPanel );
	textLabel1_9->setFrameShadow( QLabel::Sunken );
	TabPageLayout_3->addWidget( textLabel1_9 );
	layout35 = new QHBoxLayout( 0, 0, 6, "layout35");
	textLabel2_3 = new QLabel( TabPage_3, "textLabel2_3" );
	layout35->addWidget( textLabel2_3 );
	pixmapLabel2 = new QLabel( TabPage_3, "pixmapLabel2" );
	pixmapLabel2->setScaledContents( TRUE );
	layout35->addWidget( pixmapLabel2 );
	TabPageLayout_3->addLayout( layout35 );
*/
	QTextEdit * textEdit1 = new QTextEdit ( tab );
	textEdit1->setLineWrapMode ( QTextEdit::WidgetWidth );
	textEdit1->setText ( data );
	layout->addWidget ( textEdit1 );

	parent->insertTab ( -1, tab, module );
}

#include "nmgresulthandler.moc"
