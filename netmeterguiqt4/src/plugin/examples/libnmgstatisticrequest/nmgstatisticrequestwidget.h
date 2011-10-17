/***************************************************************************
 *   Copyright (C) 2008 by Roberto Barreda <rbarreda@ac.upc.edu>           *
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
#ifndef NMGSTATISTICREQUESTWIDGET_H
#define NMGSTATISTICREQUESTWIDGET_H

#include "ui_nmgstatisticrequestwidgetbase.h"
#include <nmgimodulecommunication.h>
#include <nmgmodulepreferencesmanager.h>

#define ID_PREFERENCE_LAST_STATISTIC_PATH	"StatisticLastTestPath"

class NMGStatisticRequestWidget : public QWidget, public Ui::NMGStatisticRequestWidgetBase
{
		Q_OBJECT
	public:
		NMGStatisticRequestWidget ( NMGIModuleCommunication * mc, NMGModulePreferencesManager * prefMan,
		                            QWidget *parent = 0, const char *name = 0 );
		~NMGStatisticRequestWidget();

	public slots:
		void btnFileClicked();
		void btnReqClicked();
		void btnSubClicked();

	private:
		NMGISender * sender;
		NMGIModuleCommunication * moduleCommunication;
		NMGModulePreferencesManager * preferencesManager;
};

#endif
