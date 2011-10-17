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
#ifndef _NMGUTIL_H_
#define _NMGUTIL_H_

#include <nmstring.h>
#include <QString>

#define TAG_ATTACH		"attach"
#define TAG_DEATTACH		"deattach"
#define TAG_EXIT		"quit"
#define TAG_CLIENT		"client"
#define TAG_VERSION		"version"
#define TAG_SERVER		"server"
#define TAG_PLUGINS		"plugins"
#define TAG_PLUGIN		"plugin"
#define TAG_NAME		"name"
#define TAG_CAPABILITIES	"capabilities"
#define TAG_QUIT		"quit"
#define TAG_CONFIGURE		"configure"
#define TAG_ID			"id"
#define TAG_DATA		"data"
#define TAG_TYPE		"type"
#define TAG_MESSAGE		"message"
#define TAG_ACK			"ack"
#define TAG_NACK		"nack"
#define TAG_RESULT		"result"
#define TAG_PENDENT		"pendent"
#define TAG_ERROR		"error"
#define TAG_START		"start"
#define TAG_TEST		"test"
#define TAG_STOP		"stop"
#define TAG_STATUS		"status"
#define TAG_UNKNOWN		"unknown"
#define TAG_HOSTS		"hosts"
#define TAG_HOST		"host"
#define TAG_IP			"ip"
#define TAG_PING		"ping"
#define TAG_PONG		"pong"
#define TAG_			""

enum NMGCommandType {Configure, Start, Stop, Status, Unknown};

class NMGUtil
{
	public:
		inline static QString getCommandTypeName ( NMGCommandType command )
		{
			switch ( command )
			{
				case Start: return "Start";
				case Stop:  return "Stop";
				case Configure: return "Configure";
				case Status: return "Status";
				default: return "Unknown";
			}
		}
};

enum NMGServerReplyType {NAck, Ack, Result, Pendent, StatusServer, Error};

struct NMGIncomingDataInfo
{
	NMGServerReplyType replyType;
	QString module;
	QString id;
	QString status;
	QString data;
	int errorType;
	QString errorCode;
};

#endif
