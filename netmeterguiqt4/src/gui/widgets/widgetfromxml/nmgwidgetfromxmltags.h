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
 
#ifndef WIDGETFROMXMLTAGS_H_
#define WIDGETFROMXMLTAGS_H_
 
 /* Set of tags of the XML doc */ 
#define TAG_ROOT	"guimoduledescription"
#define TAG_INFO	"info"
#define TAG_DESCRIPTION	"description"
#define TAG_LAYOUT	"layout"
#define TAG_BUTTON	"button"
#define TAG_TEXT	"text"
#define TAG_LABEL	"label"
#define TAG_FRAME	"frame"
#define TAG_ACTIONS	"actions"
#define TAG_ACTION	"action"
#define TAG_SEND	"send"
#define TAG_HIDE	"hide"
#define TAG_DATA	"data"
#define TAG_PROTOCOL	"protocol"
#define TAG_SOURCE	"source"
#define TAG_DESTINATION	"destination"
#define TAG_NAME	"name"
#define TAG_LOGIN	"login"
#define TAG_PASSWORD	"password"
#define TAG_START	"start"
#define TAG_STOP	"stop"
#define TAG_CONFIGURE	"configure"
#define TAG_STATUS	"status"
#define TAG_UNKNOWN	"unknown"
#define TAG_OPTIONS	"options"
#define TAG_OPTION	"option"
#define TAG_VAR		"var"
#define TAG_ASSIGN	"assign"
#define TAG_VALIDATION	"validation"
#define TAG_VALIDATE	"validate"
#define TAG__	"."


/* Set of attributes of the XML doc */ 
#define ATTR_TYPE	"type"
#define ATTR_ROWS	"rows"
#define ATTR_COLS	"cols"
#define ATTR_ROW	"row"
#define ATTR_COL	"col"
#define ATTR_ID		"id"
#define ATTR_VALUE	"value"
#define ATTR_TEXT	"text"
#define ATTR_CHECKED	"checked"
#define ATTR_VALIDATE	"validate"
#define ATTR_MESSAGE	"message"
#define ATTR_MODULE	"module"

/* Set of attributes values of the XML doc */ 
#define TYPE_HLAYOUT	"hlayout"
#define TYPE_VLAYOUT	"vlayout"
#define TYPE_GRIDLAYOUT	"grid"
#define TYPE_CONFIGURE	"configure"
#define TYPE_TRUE	"true"
#define TYPE_FALSE	"false"
#define TYPE_INTEGER	"integer"
#define TYPE_PINTEGER	"positive integer"
#define TYPE_NINTEGER	"negative integer"
#define TYPE_DOUBLE	"double"
#define TYPE_PDOUBLE	"positive double"
#define TYPE_NDOUBLE	"negative double"
#define TYPE_NOTNULL	"not null"

#define TYPE_MAX_INTEGER	1000000000
#define TYPE_MIN_INTEGER	-1000000000
#define TYPE_MAX_DOUBLE		1000000000
#define TYPE_MIN_DOUBLE		-1000000000
#define TYPE_DECIMALS_DOUBLE 	10

#endif /* WIDGETFROMXMLTAGS_H_ */

