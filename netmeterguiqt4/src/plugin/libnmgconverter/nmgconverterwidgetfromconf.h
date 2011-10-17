/***************************************************************************
 *   Copyright (C) 2008 by Joan Sala Calero <jsala@ac.upc.edu>             *
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

#ifndef NMGCONVERTERFROMCONF_H
#define NMGCONVERTERFROMCONF_H

#include <nmstring.h>

#include <QSpacerItem>
#include <QPushButton>
#include <QString>
#include <QListIterator>
#include <QCheckBox>
#include <QFrame>
#include <QVBoxLayout>
#include <QLabel>
#include <QIntValidator>
#include <QLineEdit>
#include <QWidget>
#include <QPair>

// xml configuration tags

#define TAG_CONVERTER	"nmgconverter"
#define TAG_FILE	"file"

#define TAG_TP_INT	"int"
#define TAG_TP_FILE	"file"
#define TAG_TP_CHECK	"check"
#define TAG_TP_STRING	"string"

#define TAG_SHORT_DESC	"sdesc"
#define TAG_DESC	"desc"
#define TAG_USAGE	"usage"
#define TAG_IN		"in"
#define TAG_OUT		"out"
#define TAG_PIN		"pin"
#define TAG_POUT	"pout"

#define ATTR_NUM	"num"
#define ATTR_TYPE	"type"
#define ATTR_EXT	"ext"
#define ATTR_MIN	"min"
#define ATTR_MAX	"max"
#define ATTR_FLAG	"flag"

typedef struct
{
	QString flag;
	QString name;
	QString type;
	int max;
	int min;
	QString ext;

} infoparam;

typedef struct 
{
	QList <infoparam> in_info;		// <name, info>
	QList <infoparam> out_info;		// <name, info>
	int N_in;
	int N_out;

} usage_struct;

class NMGConverterWidgetFromConf : QWidget
{
	public:

		NMGConverterWidgetFromConf(QWidget *parent, const char *name);
		~NMGConverterWidgetFromConf();

		void displayWidget(QFrame *frame, QString scriptName, const usage_struct st);
		QGridLayout* createLayout();
		void reset(QFrame *frame);
		bool checkParams();
		QPair<QString,QStringList> getCommand();

	private:

		QString exec_script;

		QList<bool> in_exec_order;			// true -> lineEdit and false -> checkBox
		QList<bool> out_exec_order;			// true -> lineEdit and false -> checkBox

		QStringList listFlagsIN;
		QStringList listFlagsOUT;

		QLabel * labelINPUT;
		QLabel * labelOUTPUT;

		QList<QLabel *> labelListIN;
		QList<QLabel *> labelListOUT;
	
		QList<QLineEdit *> lineListIN;
		QList<QLineEdit *> lineListOUT;

		QList<QCheckBox *> checkListIN;
		QList<QCheckBox *> checkListOUT;
};

#endif