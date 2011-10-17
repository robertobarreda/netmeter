/***************************************************************************
 *   Copyright (C) 2008 by Joan Sala Calero <jsala@ac.upc.edu>             *
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

#ifndef NMGCONVERTERWIDGET_H
#define NMGCONVERTERWIDGET_H

#include "ui_nmgconverterwidgetbase.h"
#include "nmgconverterwidgetfromconf.h"

#include <nmgimodulecommunication.h>
#include <nmgisender.h>
#include <progressmanager.h>
#include <nmgmodulepreferencesmanager.h>
#include <nmgmodtabwidgetmanager.h>
#include <nmstring.h>

#include <QMenu>
#include <QToolBar>
#include <QProcess>
#include <QFile>
#include <QMap>


namespace Ui {
     class NMGConverterWidgetBase;
 } // namespace Ui


class NMGConverterWidget : public QWidget, public Ui::NMGConverterWidgetBase
{
	Q_OBJECT

	public:

		/** Constructor && Destructor of the widget */

		NMGConverterWidget ( NMGISender * s, NMGIModuleCommunication * mc, QWidget *parent = 0,
				const char *name = 0, NMGModulePreferencesManager * prefMan = 0 );
		~NMGConverterWidget();
		
		void notifyResult ( QString data );
		void notifyError ( int errorType, QString errorMessage );
		void notifyStatus ( QString status );
		

	public slots:
		
		inline QMenu * getPopupMenu() { return menu; }

		QToolBar * getToolBar();
		void incrementProgressItemBar();
		void buttonGraphicClicked();
		void buttonStatisticClicked();
		void scriptSelected();
		void executeScript();
		void runCancelled(ProgressItem *);
		void printstdErr();
		void printstdOut();
		void processFinish(int exitCode, QProcess::ExitStatus exitStatus);

	private:

		void initProgress(QString title);
		void finishProgress(QString title);
		void createMenu();
		void updateWidgetBecauseOfTestEnd();
		void loadFile(QString filepath);
		void loadlistScripts();
		void addScript(QString name, QString short_desc, QString desc, usage_struct st);
		void displayWidget(QString scriptName);
		void printCommand(QString script, QStringList params);
		bool check_executable(QString path);
		
		usage_struct parseUsage(QDomElement usage);
		QList<infoparam> parseInfo(QDomElement list);

		QMenu * menu;
		QToolBar * toolBar;
		NMGModTabWidgetManager * tabWidgetManager;
		NMGModulePreferencesManager * preferencesManager;
		ProgressItem * progressItem;

		NMGISender * sender;
		NMGIModuleCommunication * moduleCommunication;
		NMGConverterWidgetFromConf * conf;

                QString idSubscribeStatistic, idSubscribeGraphic;
		QString lastResultFile, lastUsedScript;

		QMap<QString, QString> mapScripts; 		// < script_name , long_desc >
		QMap<QString, QString> mapShort;		// < script_name , short_desc >
		QMap<QString, usage_struct> mapUsage;		// < script_name , usage >

		QProcess *myProcess;
};

#endif
