/*
 *   $Id: nmmmgenutils.h,v 1.7 2007-03-28 15:43:37 rbarreda Exp $
 *
 *   Copyright (c) 2006 Ernest Nieto
 *   Copyright (c) 2007 Roberto Barreda
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifndef NMMGENUTILS_H
#define NMMGENUTILS_H

#include <nmstring.h>

#define MGEN_PATH_WRONG			"wrong mgen path"
#define DREC_PATH_WRONG			"wrong drec path"
#define DECODE_PATH_WRONG		"wrong decode path"
#define ERROR_RUNNING_MGEN		"error running mgen"
#define ERROR_RUNNING_DREC		"error running drec"
#define ERROR_RUNNING_DECODE		"error running decode"
#define ERROR_RUNNING_KILLALL		"error running killall"
#define CANNOT_RESOLVE_SOURCE_ADDRESS	"cannot resolve the source address"
#define CANNOT_RESOLVE_DEST_ADDRESS	"cannot resolve the destination address"
#define BAD_PASSWORD_SOURCE_HOST	"bad password source host"
#define BAD_PASSWORD_DEST_HOST		"bad password destination host"
#define ERROR_CONNECTING_SOURCE_SCP	"error connecting to source host via scp"
#define ERROR_CONNECTING_DEST_SCP	"error connecting to destination host via scp"
#define MGEN_ADRRESS_ALREADY_IN_USE	"mgen address already in use"

#define MGEN_ERROR_PARSING_SCRIPT	"error parsing mgen script"
#define MGEN_ERROR_LOADING_SCRIPT	"error loading mgen script"
#define MGEN_ERROR_UNKNOWN		"unknown mgen error"
#define DREC_ERROR_PARSING_SCRIPT	"error parsing drec script"
#define DREC_ERROR_LOADING_SCRIPT	"error loading drec script"
#define DREC_ERROR_UNKNOWN		"unknown drec error"

#define MGEN_SCP_TIMEOUT_ERROR		"mgen scp timeout error"
#define MGEN_SCP_IS_CONNECTED_ERROR 	"mgen scp is connected error"
#define MGEN_SCP_UNKNOWN_ERROR		"mgen scp unknown error"
#define DREC_SCP_TIMEOUT_ERROR		"drec scp timeout error"
#define DREC_SCP_IS_CONNECTED_ERROR 	"drec scp is connected error"
#define DREC_SCP_UNKNOWN_ERROR		"drec scp unknown error"
#define DECODE_SCP_TIMEOUT_ERROR	"decode scp timeout error"
#define DECODE_SCP_IS_CONNECTED_ERROR 	"decode scp is connected error"
#define DECODE_SCP_UNKNOWN_ERROR	"decode scp unknown error"

#define MGEN_ERROR_VERSION		"error running version of mgen"
#define MGEN_EMPTY_FILE			"nmmgen returned an empty file" 

#define LOCAL_SUFFIX			".local"

#endif
