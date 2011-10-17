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
#ifndef NMGMESSAGECENTER_H
#define NMGMESSAGECENTER_H

#include <QObject>
#include <QMap>
#include <QTimer>

typedef QMap<QString,QString> QMessageMap;
typedef QMessageMap::iterator QMessageMapIterator;

/**
It has the responsibility of resending the messages which haven't been acknowleged and deleting the acknowleged messages.
*/
class NMGMessageCenter : public QObject
{
		Q_OBJECT
	public:
		NMGMessageCenter ( QObject *parent = 0, const char *name = 0 );
		~NMGMessageCenter();
		void addMessage ( QString id, QString message );
		void deleteMessage ( QString id );
		bool existsMessageWithId ( QString id );
		QString getMessage ( QString id );
		QString getModuleName ( QString id );

	private slots:
		void checkMessages();

	private:
		QMessageMap messageMap;
		QTimer timer;
};

#endif
