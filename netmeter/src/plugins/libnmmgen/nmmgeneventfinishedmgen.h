/*
 *   $Id: nmmgeneventfinishedmgen.h,v 1.1 2006-06-03 19:37:12 enieto Exp $
 *
 *   Copyright (c) 2006 Pau Capella
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
#ifndef NMMGENEVENTFINISHEDMGEN_H_
#define NMMGENEVENTFINISHEDMGEN_H_

class NMMgenEventFinishedMgen
{
public:
	NMMgenEventFinishedMgen() {}
	virtual ~NMMgenEventFinishedMgen() {}
	virtual void finishedMgen()=0;
};
  
#endif

