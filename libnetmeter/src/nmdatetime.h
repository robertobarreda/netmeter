/*
 *   $Id: nmdatetime.h,v 1.1.1.1 2007-05-26 17:41:22 rserral Exp $
 *
 *   Copyright (c) 2006 Pau Capella and Trolltech AS
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
#ifndef NMDATETIME_H
#define NMDATETIME_H

#include "nmstring.h"

typedef unsigned int uint;

/**
	@author Pau Capella <pau.capella@ac.upc.edu> and Trolltech AS <info@trolltech.com>
	@brief provides calendar date 
*/
class NMDate {
public:
	NMDate() { jd = 0; }
	NMDate( int y, int m, int d );
	~NMDate() {}
	
	/**
	\fn NMDate::isNull() const
	\brief Returns TRUE if this date is null; otherwise returns FALSE.
	*/
	bool isNull() const { return jd == 0; }
	bool isValid() const;

	bool setYMD( int y, int m, int d );
	int year();
	int month();
	int day();


	bool operator== ( const NMDate &d) const;
	bool operator!= ( const NMDate &d) const;
	bool operator< ( const NMDate &d) const;
	bool operator<= ( const NMDate &d) const;
	bool operator> ( const NMDate &d) const;
	bool operator>= ( const NMDate &d) const;

	static NMDate currentDate();
	NMString toString();
	static NMDate fromString( const NMString &s);

private:
	bool isValid( int y, int m, int d);
	uint gregorianToJulian( int y, int m, int d);
	void julianToGregorian( int &y, int &m, int &d);

	uint jd;
};


/**
	@author Pau Capella <pau.capella@ac.upc.edu> and Trolltech AS <info@trolltech.com>
	@brief provides clock time
*/

class NMTime {
public:
	NMTime() { ds = 0; }
	NMTime( int h, int m, int s=0, int ms=0);   // set time
	~NMTime() {}

	bool isNull() const { return ds == 0; }
	bool isValid() const;                 // valid time

	int hour() const;                 // 0..23
	int minute() const;                 // 0..59
	int second() const;                 // 0..59
	int msec() const;                 // 0..999

	void addSecs( int sec);

	bool setHMS( int h, int m, int s, int ms=0);

	bool operator== ( const NMTime &t) const;
	bool operator!= ( const NMTime &t) const;
	bool operator< ( const NMTime &t) const;
	bool operator<= ( const NMTime &t) const;
	bool operator> ( const NMTime &t) const;
	bool operator>= ( const NMTime &t) const;

	static NMTime currentTime();
	NMString toString();
	static NMTime fromString( const NMString &s);

private:
	bool isValid( int h, int m, int s, int ms);

	uint ds;
};

/**
	@author Pau Capella <pau.capella@ac.upc.edu> and Trolltech AS <info@trolltech.com>
	@brief contains a calendar date and a clock time.
*/
class NMDateTime{

public:
	NMDateTime();
	NMDateTime( NMDate date, NMTime time);
	~NMDateTime();

	bool isNull();
	bool isValid();

	void setDate( const NMDate &date);
	void setTime( const NMTime &time);
	NMDate date();
	NMTime time();

	bool operator== ( const NMDateTime &dt) const;
	bool operator!= ( const NMDateTime &dt) const;
	bool operator< ( const NMDateTime &dt) const;
	bool operator<= ( const NMDateTime &dt) const;
	bool operator> ( const NMDateTime &dt) const;
	bool operator>= ( const NMDateTime &dt) const;

	static NMDateTime currentDateTime();

	NMString toString();
	static NMDateTime fromString( const NMString &s);

private:
	NMDate m_date;
	NMTime m_time;
};

#endif
