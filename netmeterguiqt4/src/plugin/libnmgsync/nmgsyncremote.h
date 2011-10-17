/***************************************************************************
 *   Copyright (C) 2005 by Joan Sala                                       *
 *   jsala@ac.upc.edu                                                      *
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

#ifndef NMGSYNCREMOTE_H
#define NMGSYNCREMOTE_H

#include <nmgisender.h>
#include <QString>
#include <QWidget>

class NMGSyncRemote : public QWidget
{
	Q_OBJECT

	public:

                NMGSyncRemote(NMGISender * s, QString currentTestId );
                ~NMGSyncRemote();

                void sendStart( QString data );
		void sendStop();

        private:
                QString testId;
		NMGISender * sender;
};

#endif
