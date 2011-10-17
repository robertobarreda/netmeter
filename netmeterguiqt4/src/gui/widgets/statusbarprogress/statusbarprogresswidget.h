/***************************************************************************
 *   Copyright (C) 2007 by Roberto Barreda <rbarreda@ac.upc.edu>           *
 *   Copyright (C) 2005 by Pau Capella <pau.capella@ac.upc.edu> 	   *
 *    and kmail authors							   *
 *                                                                         *
 *   This software is based in kmail, http://kmail.kde.org                 *
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
#ifndef STATUSBARPROGRESSWIDGET_H
#define STATUSBARPROGRESSWIDGET_H

#include "progressdialog.h"

#include <QFrame>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QStackedWidget>
#include <QEvent>
#include <QHBoxLayout>

class StatusbarProgressWidget : public QFrame
{
		Q_OBJECT

	public:
		StatusbarProgressWidget ( ProgressDialog* progressDialog, QWidget* parent, bool button = true );

	public slots:
		void slotClean();
		void slotProgressItemAdded ( ProgressItem *i );
		void slotProgressItemCompleted ( ProgressItem *i );
		void slotProgressItemProgress ( ProgressItem *i, unsigned int value );

	protected slots:
		void slotProgressDialogVisible ( bool );
		void slotShowItemDelayed();
		void slotBusyIndicator();

	protected:
		void setMode();
		void connectSingleItem();
		void activateSingleItemMode();
		virtual bool eventFilter ( QObject *, QEvent * );

	private:
		QProgressBar * m_progressbar;
		QLabel * m_label;
		QPushButton* m_button;

		enum Mode { None, /*Label,*/ Progress };

		uint m_mode;
		bool m_showbutton;

		QHBoxLayout *m_box;
		QStackedWidget *m_stack;
		ProgressItem *m_currentitem;
		ProgressDialog* m_progressdialog;
		QTimer *m_delaytimer;
		QTimer *m_busytimer;
};

#endif
