/***************************************************************************
 *   Copyright (C) 2005 by Ernest Nieto                                    *
 *   ernest.nieto@gmail.com                                                *
 *   Copyright (C) 2007 by Roberto Barreda                                 *
 *   rbarreda@ac.upc.edu                                                   *
 *   Copyright (C) 2007 by Joan Sala                                       *
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
#ifndef NMGMGENXMLINTERPRET_H
#define NMGMGENXMLINTERPRET_H

#include <QDomDocument>
#include <QString>
#include <QFile>
#include <QXmlStreamWriter>

#include <nmgmetadataparser.h>
#include <nmgmetadata.h>
#include <nmgmetadatatags.h>
#include "nmgxmlflowsortedlist.h"

namespace Ui {
    class NMGMGenMainWidget;
} 

class NMGMGenWidget;

class NMGMGenXMLInterpret
{
public:
    NMGMGenXMLInterpret(NMGMGenWidget * w);

    ~NMGMGenXMLInterpret();

    QString generateXMLTestDocument(bool putUserAndPass);
    QString generateXMLSync();
    bool updateWidgetFromXMLDocument(QString filename);
    QString getResultFilePathFromResult(const QString & xmlMessage);
    int parseStatusMessage(const QString & xmlMessage);

private:
    void createXMLConfMGen(QDomDocument & doc, NMGQDomSortElement & root);
    void createXMLConfDrec(QDomDocument & doc, NMGQDomSortElement & root);

private:
    NMGMGenWidget * widget;
    int lastFlowId;


};

#endif
