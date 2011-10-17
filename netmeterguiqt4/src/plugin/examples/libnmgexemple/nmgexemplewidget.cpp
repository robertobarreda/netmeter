/***************************************************************************
 *   Copyright (C) 2005 by Ernest Nieto                                    *
 *   ernest.nieto@gmail.com                                                *
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
#include "nmgexemplewidget.h"

#include <nmghostinformationmanager.h>

#include <QAction>
#include <QMessageBox>
#include <QVBoxLayout>

#include <iostream>
using namespace std;

NMGExempleWidget::NMGExempleWidget( NMGISender * s, QWidget *parent, const char *name )
		: QWidget( parent )
{
	setObjectName ( name );
	layout = new QVBoxLayout( this );
	setLayout(layout);

	t = new QLineEdit( this );
	layout->addWidget( t );

	b = new QPushButton( "Send", this );
	layout->addWidget( b );
	sender = s;
	connect( b, SIGNAL( clicked() ), this, SLOT( actionSend() ) );

	b = new QPushButton( "Info Host", this );
	layout->addWidget( b );
	connect( b, SIGNAL( clicked() ), this, SLOT( hostClicked() ) );

	menu = new QMenu( this );
	menu->addAction( "Show a message box", this, SLOT( showMessageBox() ), Qt::CTRL + Qt::Key_N );
	menu->addSeparator();
	menu->addAction( "Warn me", this, SLOT( warnUser() ), Qt::CTRL + Qt::Key_L );
}


NMGExempleWidget::~NMGExempleWidget()
{}

void NMGExempleWidget::actionSend()
{
	sender->sendCommand( Start, t->text() );
}

void NMGExempleWidget::hostClicked()
{
	NMGHostInformationManager::self()->showHostWidget( t->text() );
}

QMenu * NMGExempleWidget::getPopupMenu()
{
	return menu;
}

void NMGExempleWidget::showMessageBox()
{
	QMessageBox::information( 0, "Message box...", "Dummy message box..." );
}

void NMGExempleWidget::warnUser()
{
	QMessageBox::warning( 0, "Warning message box...", "Dummy message box..." );
}

#include "nmgexemplewidget.moc"
