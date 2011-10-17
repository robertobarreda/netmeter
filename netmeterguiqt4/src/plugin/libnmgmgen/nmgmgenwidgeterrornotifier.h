/***************************************************************************
 *   Copyright (C) 2005 by Ernest Nieto                                    *
 *   ernest.nieto@gmail.com                                                *
 *   Copyright (C) 2007 by Roberto Barreda                                 *
 *   rbarreda@ac.upc.edu                                                   *
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
#ifndef NMGMGENWIDGETERRORNOTIFIER_H
#define NMGMGENWIDGETERRORNOTIFIER_H

#include <qstring.h>


/* Errors reported by the Core Module */
#define MGEN_PATH_WRONG		"wrong mgen path"
#define DREC_PATH_WRONG		"wrong drec path"
#define DECODE_PATH_WRONG	"wrong decode path"
#define MGEN4_PATH_WRONG	"wrong mgen v4 path"
#define ERROR_RUNNING_MGEN	"error running mgen"
#define ERROR_RUNNING_DREC	"error running drec"
#define ERROR_RUNNING_DECODE	"error running decode"
#define ERROR_RUNNING_KILLALL	"error running killall"
#define CANNOT_RESOLVE_SOURCE_ADDRESS	"cannot resolve the source address"
#define CANNOT_RESOLVE_DEST_ADDRESS	"cannot resolve the destination address"
#define BAD_PASSWORD_SOURCE_HOST	"bad password source host"
#define BAD_PASSWORD_DEST_HOST		"bad password destination host"
#define ERROR_CONNECTING_SOURCE_SFTP	"error connecting to source host via sftp"
#define ERROR_CONNECTING_DEST_SFTP	"error connecting to destination host via sftp"
#define MGEN_ADRRESS_ALREADY_IN_USE	"mgen address already in use"

#define MGEN_EMPTY_FILE                 "nmmgen returned an empty file"
#define MGEN_ERROR_PARSING_SCRIPT	"error parsing mgen script"
#define MGEN_ERROR_LOADING_SCRIPT	"error loading mgen script"
#define MGEN_ERROR_UNKNOWN		"unknown mgen error"
#define DREC_ERROR_PARSING_SCRIPT	"error parsing drec script"
#define DREC_ERROR_LOADING_SCRIPT	"error loading drec script"
#define DREC_ERROR_UNKNOWN		"unknown drec error"

#define MGEN_SFTP_TIMEOUT_ERROR		"mgen sftp timeout error"
#define MGEN_SFTP_IS_CONNECTED_ERROR "mgen sftp is connected error"
#define MGEN_SFTP_UNKNOWN_ERROR		"mgen sftp unknown error"
#define DREC_SFTP_TIMEOUT_ERROR		"drec sftp timeout error"
#define DREC_SFTP_IS_CONNECTED_ERROR 	"drec sftp is connected error"
#define DREC_SFTP_UNKNOWN_ERROR		"drec sftp unknown error"
#define DECODE_SFTP_TIMEOUT_ERROR	"decode sftp timeout error"
#define DECODE_SFTP_IS_CONNECTED_ERROR 	"decode sftp is connected error"
#define DECODE_SFTP_UNKNOWN_ERROR	"decode sftp unknown error"


class NMGMGenWidget;


class NMGMgenWidgetErrorNotifier
{
public:
    NMGMgenWidgetErrorNotifier(NMGMGenWidget * w);

    ~NMGMgenWidgetErrorNotifier();
    
    void notifyErrorToUser(int errorType, QString errorMessage);

private:
    NMGMGenWidget * widget;
};

#endif
