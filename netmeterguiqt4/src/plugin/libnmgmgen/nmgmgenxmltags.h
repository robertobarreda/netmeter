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
#ifndef NMGMGENXMLTAGS_H
#define NMGMGENXMLTAGS_H


#define ID_MGEN_PATH	"MGEN_MGenPath"
#define ID_DREC_PATH	"MGEN_DrecPath"
#define ID_DECODE_PATH	"MGEN_DecodePath"
#define ID_MGEN4_PATH	"MGEN_MGen4Path"

#define ID_PREFERENCE_LAST_TEST_PATH	"MGenLastTestPath"

#define TAG_TEST			"test"
#define TAG_METADATA			"metadata"
#define TAG_RESULT			"result"

#define TAG_CONF_MGEN			"conf_mgen"
#define TAG_FLOWS			"flows"
#define TAG_START			"start"
#define TAG_PORT			"port"
#define TAG_INTERFACE			"interface"
#define TAG_TTL				"ttl"
#define TAG_ON				"on"
#define ATTR_RATE			"rate"
#define ATTR_PATTERN			"pattern"
#define ATTR_TIME			"time"
#define ATTR_DEST			"dest"
#define ATTR_SIZE			"size"
#define ATTR_FLOW_ID			"flow_id"
#define TAG_RSVP			"rsvp"
#define ATTR_MIN			"min"
#define ATTR_MAX			"max"
#define ATTR_PEAK_RATE			"peak_rate"
#define ATTR_BUCKET_SIZE		"bucket_size"
#define TAG_TOS				"tos"
#define TAG_BURST			"burst"
#define ATTR_INTERVAL_TXT		"interval_txt"
#define ATTR_INTERVAL			"interval"
#define ATTR_DURATION_TXT		"duration_txt"
#define ATTR_DURATION			"duration"
#define TAG_TRAFFIC_CLASS		"traffic_class"
#define TAG_MOD				"mod"
#define TAG_OFF				"off"
#define ATTR_TIME			"time"
#define ATTR_RSVP_SOURCE_PARAM		"source_param"
#define ATTR_RSVP_RESV_STYLE		"resv_style"
#define ATTR_RSVP_DEST_PARAM		"dest_param"
#define ATTR_NAME			"name"
#define TAG_TEST_MGEN			"test_mgen"
#define TAG_MGEN_VERSION		"mgen_version"
#define TAG_RESULT_PATH			"result_path"
#define TAG_IDENTIFIER			"identifier"
#define TAG_SOURCE			"source"
#define TAG_ADDRESS			"address"
#define TAG_MGEN_PATH			"mgen_path"
#define TAG_MANAGEMENT			"management"
#define TAG_USERNAME			"username"
#define TAG_PASSWORD			"password"
#define TAG_DESTINATION			"destination"
#define TAG_DREC_PATH			"drec_path"
#define TAG_DECODE_PATH			"decode_path"
#define TAG_CONF_DREC			"conf_drec"
#define TAG_PORTS			"ports"
#define TAG_PORT			"port"
#define TAG_RESV			"resv"
#define TAG_UNRESV			"unresv"
#define TAG_MGEN_SYNC			"mgen_sync"
#define ATTR_SPEC			"spec"
#define ATTR_STYLE			"style"
#define TAG_DECODE_FILE			"decode_file"
#define TAG_RESULT_FILE			"result_file"
#define TAG_RESULT_GRAPH_OWD		"result_graph_owd"
#define TAG_RESULT_GRAPH_OWDD		"result_graph_owdd"
#define TAG_RESULT_GRAPH_IPDV		"result_graph_ipdv"
#define TAG_RESULT_GRAPH_IPDVD		"result_graph_ipdvd"
#define TAG_TIME_FILE			"time_file"
#define TAG_TEST_FILE			"test_file"
#define TAG_STATUS			"status"
#define TAG_LOCAL			"local"
#define TAG_DISTR_PATH			"distr_path"
	
#define ATTR_XMLNS_XSI			"xmlns:xsi"
#define VALUE_XMLNS_XSI			"http://www.w3.org/2001/XMLSchema-instance"
#define ATTR_XSI_SCHEMA_LOCATION	"xsi:noNamespaceSchemaLocation"
#define VALUE_XSI_SCHEMA_LOCATION	"test_mgen.xsd" 
#define ATTR_VERSION			"version"
#define VALUE_VERSION			"1.0"
#define ATTR_ENCONDING			"encoding"
#define VALUE_ENCODING			"UTF-8"
#define ATTR_TYPE			"type"
#define VALUE_TYPE			"text/xsl"
#define ATTR_HREF			"href"
#define VALUE_HREF			"test_mgen.xslt"

#define SEPARADOR_MGEN3			":"
#define SEPARADOR_MGEN4			"/"

#endif

