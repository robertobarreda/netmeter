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
#ifndef NMGNETPERFWIDGETERRORNOTIFIER_H
#define NMGNETPERFWIDGETERRORNOTIFIER_H

#include <QString>

/* Errors reported by the Core Module */
#define CANNOT_RESOLVE_SOURCE_ADDRESS	"cannot resolve the source address"
#define CANNOT_RESOLVE_DEST_ADDRESS	"cannot resolve the destination address"

#define NETPERF_PATH_WRONG		"wrong netperf path"
#define NETPERF_ADRRESS_ALREADY_IN_USE	"netperf address already in use"
#define NETPERF_INVALID_OPTION		"invalid netperf option"
#define NETPERF_ERROR_CONNECT_NETSERVER	"no netserver listening to netperf"
#define NETPERF_ERROR_PERMISSION	"not enough permission to run netperf"
#define NETPERF_ERROR_UNKNOWN		"unknown netperf error"

#define NETSERVER_PATH_WRONG		"wrong netserver path"
#define NETSERVER_INVALID_OPTION	"invalid netserver option"
#define NETSERVER_ERROR_PERMISSION	"not enough permission to run netserver"
#define NETSERVER_ERROR_UNKNOWN		"unknown netserver error"

#define BAD_PASSWORD_SOURCE_HOST	"bad password source host"
#define NETPERF_SSH_TIMEOUT_ERROR	"netperf ssh timeout error"
#define NETPERF_SSH_UNKNOWN_ERROR	"netperf ssh unknown error"

#define BAD_PASSWORD_DEST_HOST		"bad password destination host"
#define NETSERVER_SSH_TIMEOUT_ERROR	"netserver ssh timeout error"
#define NETSERVER_SSH_UNKNOWN_ERROR	"netserver ssh unknown error"

#define DECODE_SFTP_TIMEOUT_ERROR	"decode sftp timeout error"
#define DECODE_SFTP_IS_CONNECTED_ERROR  "decode sftp is connected error"
#define DECODE_SFTP_UNKNOWN_ERROR	"decode sftp unknown error"

#define XML_CONFIG_ERROR		"wrong XML configuration received"
#define XML_OUTPUT_ERROR		"error parsing result"

class NMGNetperfWidget;

class NMGNetperfWidgetErrorNotifier
{
	public:
		/**
		* Contructor of the class
		* @return
		*/
		inline NMGNetperfWidgetErrorNotifier ( NMGNetperfWidget * w ) : widget ( w ) {}
		/**
		* Destructor of the class
		* @return
		*/
		inline ~NMGNetperfWidgetErrorNotifier() {}
		/**
		* Notifies an error to the user
		* @param errorType Type of the error (fatal = 1, user = 5, warning = 10, debug = 20)
		* @param errorMessage Error Message
		*/
		void notifyErrorToUser ( int errorType, QString & errorMessage );

	private:
		NMGNetperfWidget * widget;
};

#endif
