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
#ifndef NMGMODTABWIDGETMANAGER_H
#define NMGMODTABWIDGETMANAGER_H

#include <QObject>
#include <QString>
#include <QTimer>

class NMGModule;

class NMGModTabWidgetManager : public QObject
{
		Q_OBJECT
	public:
		NMGModTabWidgetManager( NMGModule * mod, QObject *parent = 0, const char *name = 0 );
		~NMGModTabWidgetManager();
		void setTabImage( const QString & image );
		void setTabAnimation( const QString & image1, const QString& image2, int delayMseg = 500 );
		void stopTabAnimation();
		void removeTabImage();

	private slots:
		void changeImage();

	private:
		NMGModule * module;
		bool animationEnabled;
		QString animationImage1;
		QString animationImage2;
		int animationDelayMseg;
		QTimer timer;
		int animationStep;
};

#endif
