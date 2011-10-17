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
#ifndef NMGEXEMPLESUBSCRIPTEDWIDGET_H
#define NMGEXEMPLESUBSCRIPTEDWIDGET_H

#include "ui_nmgexemplesubscriptedwidgetbase.h"

#include <nmgisender.h>
#include <nmgimodulecommunication.h>

namespace Ui {
    class NMGExempleSubscriptedWidgetBase;
} // namespace Ui

/**
@author Ernest Nieto
*/
class NMGExempleSubscriptedWidget : public QWidget, public Ui::NMGExempleSubscriptedWidgetBase
{
		Q_OBJECT
	public:
		NMGExempleSubscriptedWidget( NMGISender * sender, NMGIModuleCommunication * mc, QWidget *parent = 0, const char *name = 0 );
		~NMGExempleSubscriptedWidget();

	public slots:
		void buttonStartClicked();
		void buttonStopClicked();
		void buttonStatusClicked();
		void buttonSubscribeClicked();

	private:
		NMGISender * sender;
		NMGIModuleCommunication * moduleCommunication;
};

#endif
