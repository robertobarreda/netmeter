/*
 *   $Id: nmmgenstatusmanager.h,v 1.1 2006-04-30 16:32:44 enieto Exp $
 *
 *   Copyright (c) 2006 Ernest Nieto
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
#ifndef NMMGENSTATUSMANAGER_H_
#define NMMGENSTATUSMANAGER_H_

class NMMgenStatusManager
{
public:
	typedef enum{NotStarted=0, SendingDrecScriptFile=10, DrecStarted=20, SendingMgenScriptFile=30, 
	MgenStarted=40, MgenFinished = 50, DecodeStarted = 60, DecodeFinished = 70,
	ReceivingDecodeResultFile=80, TestFinished = 90, TestError=100} MgenStatus;
	
	NMMgenStatusManager() {}
	virtual ~NMMgenStatusManager() {}
	
	virtual void setStatus(MgenStatus status)=0;
};
  
#endif
