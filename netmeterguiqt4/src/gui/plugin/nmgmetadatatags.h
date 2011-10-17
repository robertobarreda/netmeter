/***************************************************************************
 *   Copyright (C) 2007 by Eduard Bellot                                   *
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
#ifndef NMGMETADATATAGS_H
#define NMGMETADATATAGS_H

// metadata tags

#define TAG_XML                         "?xml"
#define TAG_TEST                        "test"
#define TAG_METADATA                    "metadata"
#define TAG_NAME                        "name"
#define TAG_IDENTIFIER			"identifier"
#define TAG_TYPE                        "type"
#define TAG_VERSION		        "version"
#define TAG_RESULT_PATH			"result_path"
#define TAG_SOURCE			"source"
#define TAG_DESTINATION			"destination"
#define TAG_ADDRESS			"address"
#define TAG_MGEN_PATH			"mgen_path"
#define TAG_DREC_PATH			"drec_path"
#define TAG_DECODE_PATH			"decode_path"
#define TAG_NETPERF_PATH                "netperf_path"
#define TAG_MANAGEMENT			"management"
#define TAG_NETSERVER_PATH              "netserver_path"
#define TAG_PORT			"port"
#define TAG_USERNAME			"username"
#define TAG_PASSWORD			"password"

#define TAG_CONF_MGEN                   "conf_mgen"
#define TAG_ON				"on"
#define TAG_OFF				"off"
#define TAG_TOS				"tos"
#define TAG_MOD				"mod"
#define TAG_RSVP                        "rsvp"

#define TAG_CONF_DREC			"conf_drec"
#define TAG_PORTS			"ports"
#define TAG_RESV			"resv"
#define TAG_UNRESV			"unresv"

#define TAG_CONF_NETPERF		"conf_netperf"
#define TAG_DURATION                    "duration"
#define TAG_PROTO                       "proto"
#define TAG_SMS                         "sms"
#define TAG_SSS                         "sss"
#define TAG_RMS                         "rms"
#define TAG_RSS                         "rss"

#define TAG_CONF_NETSERVER              "conf_netserver"
#define TAG_IPVERSION                   "ipv"

// metadata attributes

#define ATTR_VERSION			"version"

#define ATTR_RATE			"rate"
#define ATTR_DEST			"dest"
#define ATTR_SIZE			"size"
#define ATTR_FLOW_ID			"flow_id"
#define ATTR_PATTERN			"pattern"
#define ATTR_TIME			"time"
#define ATTR_NAME			"name"
#define ATTR_DECODE_PATH	        "MGEN_DecodePath"

#define ATTR_SPEC			"spec"
#define ATTR_STYLE			"style"
#define ATTR_TYPE                       "type"
#define ATTR_NODELAY                    "nodelay"
	
#endif
