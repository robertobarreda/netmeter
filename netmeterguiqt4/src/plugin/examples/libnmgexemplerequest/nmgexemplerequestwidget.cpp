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
#include "nmgexemplerequestwidget.h"

#include <QLineEdit>
#include <QPushButton>

NMGExempleRequestWidget::NMGExempleRequestWidget ( NMGIModuleCommunication * mc, QWidget *parent, const char *name )
		: QWidget ( parent )
{
	setObjectName ( name );
	setupUi ( this );

	moduleCommunication = mc;
	connect ( buttonAdd, SIGNAL ( clicked() ), this, SLOT ( buttonAddClicked() ) );
}

NMGExempleRequestWidget::~NMGExempleRequestWidget()
{}

void NMGExempleRequestWidget::buttonAddClicked()
{
	QString value1 = txtValue1->text();
	QString value2 = txtValue2->text();

	QString xmlMessage = "<data> <value1>" + value1 + "</value1> <value2>" + value2 + "</value2> </data>";
	moduleCommunication->request ( "NMGExempleAdder", xmlMessage );
}

#include "nmgexemplerequestwidget.moc"
