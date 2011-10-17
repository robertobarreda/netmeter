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
#ifndef NMGTRACEWIDGET_H
#define NMGTRACEWIDGET_H

#include <QTimer>
#include <QStringList>

#include "ui_nmgtracewidgetbase.h"

#include <progressmanager.h>
#include <nmgisender.h>

namespace Ui {
	class NMGTraceWidgetBase;
}

class NMGTraceWidget: public QWidget, public Ui::NMGTraceWidgetBase 
{
Q_OBJECT

public:
	NMGTraceWidget(NMGISender * s, QWidget * parent = 0, const char * name = 0);
	~NMGTraceWidget();

	void resultReceived( QString data);
	void statusReceived(QString data);
	void showHost(const int &pos, const QString &address, const QString &time);
	void errorRecieved(int err, QString Message);

signals:
	void send(QString value);

private slots:
	
	virtual void startTrace();
	virtual void stopTrace();

	void infoHostSource();
	void infoHostDestination();
	void slotProgressItemCanceled( ProgressItem *);

private:
	ProgressItem *m_progress;
	NMGISender *sender;
};

#endif
