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
#ifndef NMGPREFERENCESMODULEWIDGET_H
#define NMGPREFERENCESMODULEWIDGET_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QToolBox>
#include <QCheckBox>

class NMGPreferencesModuleWidget : public QDialog
{
		Q_OBJECT
	public:
		NMGPreferencesModuleWidget ( QWidget *parent = 0, const char *name = 0, Qt::WindowFlags fl = 0 );
		~NMGPreferencesModuleWidget();
		void accept();
		void reject();

	private:
		void loadExistingModules();
		QString transformCapabilities ( QString capabilities );

	private:
		QToolBox* toolBox1;
		QWidget* page1;
		QCheckBox* checkBox1;
		QCheckBox* checkBox2;
		QWidget* page2;
		QPushButton* buttonOk;
		QPushButton* buttonCancel;
		QVBoxLayout* preferencesModuleBaseWidgetLayout;
		QVBoxLayout* page1Layout;
		QSpacerItem* spacer2;
		QHBoxLayout* layout1;
		QSpacerItem* horizontal_Spacing2;
};

#endif
