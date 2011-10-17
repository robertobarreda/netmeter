/*
 *   $Id: nmdatetime.cpp,v 1.1.1.1 2007-05-26 17:41:22 rserral Exp $
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
#include "nmdatetime.h"
#include <time.h>
#include <sys/time.h>
#include <iostream>

using namespace std;

static const uint FIRST_DAY     = 2361222;      // Julian day for 1752-09-14
static const int  FIRST_YEAR    = 1752;         // ### wrong for many countries
static const uint SECS_PER_DAY  = 86400;
static const uint MSECS_PER_DAY = 86400000;
static const uint SECS_PER_HOUR = 3600;
static const uint MSECS_PER_HOUR= 3600000;
static const uint SECS_PER_MIN  = 60;
static const uint MSECS_PER_MIN = 60000;


/*********************** NMDATE ******************************/

NMDate::NMDate( int y, int m, int d )
{
	jd = 0;
	setYMD( y,m,d);
}

/**
\fn NMDate::isValid() const
\brief Returns TRUE if this date is valid; otherwise returns FALSE.
*/
bool NMDate::isValid() const
{
	return jd >= FIRST_DAY;
}

/**
\fn NMDate::setYMD( int y, int m, int d )
\brief Constructs a date with year y, month m and day d.

 \a y must be in the range 1752..8000, \a m must be in the range  1..12, and \a d must be in the range 1..31.
\warning If \a y is in the range 0..99, it is interpreted as 2000..2999.
*/
bool NMDate::setYMD( int y, int m, int d )
{
	if ( year() == y && month() == m && day() == d )
	return isValid();
	if ( !isValid(y,m,d) ) return false;

	jd = gregorianToJulian( y, m, d );
	return true;
}

/**
\fn NMDate::isValid( int y, int m, int d )
\brief Returns TRUE if date y/m/d is valid; otherwise returns FALSE.
*/
bool NMDate::isValid( int y, int m, int d )
{
	if ( y >= 0 && y <= 99 )
		y += 2000;
	else if ( y < FIRST_YEAR || (y == FIRST_YEAR && (m < 9 || (m == 9 && d < 14))) )
		return false;
	return true;
}


/**
\fn NMDate::year()
\brief return the year of this date (1752-8000)
*/
int NMDate::year()
{
	int y, m, d;
	julianToGregorian(y, m, d );
	return y;
}

/**
\fn NMDate::month()
\brief return the month of this date (1-12)
*/
int NMDate::month()
{
	int y, m, d;
	julianToGregorian(y, m, d );
	return m;
}

/**
\fn NMDate::day()
\brief return the day of this date (0-23)
*/
int NMDate::day()
{
	int y, m, d;
	julianToGregorian(y, m, d );
	return d;
}

/**
\fn NMDate::operator== ( const NMDate & d ) const
\brief Returns TRUE if this date is equal to d; otherwise returns FALSE.
*/
bool NMDate::operator== ( const NMDate &d) const
{
	return jd == d.jd;
}

/**
\fn NMDate::operator!= ( const NMDate & d ) const
\brief Returns TRUE if this date is diferent to d; otherwise returns FALSE.
*/
bool NMDate::operator!= ( const NMDate & d) const
{
	return jd != d.jd;
}

/**
\fn NMDate::operator< ( const NMDate & d ) const
\brief Returns TRUE if this date is earlier to d; otherwise returns FALSE.
*/
bool NMDate::operator< ( const NMDate & d) const
{
	return jd < d.jd;
}

/**
\fn NMDate::operator<= ( const NMDate & d ) const
\brief Returns TRUE if this date is earlier or equal to d; otherwise returns FALSE.
*/
bool NMDate::operator<= ( const NMDate & d) const
{
	return jd <= d.jd;
}

/**
\fn NMDate::operator> ( const NMDate & d ) const
\brief Returns TRUE if this date is later to d; otherwise returns FALSE.
*/
bool NMDate::operator> ( const NMDate & d) const
{
	return jd > d.jd;
}

/**
\fn NMDate::operator>= ( const NMDate & d ) const
\brief Returns TRUE if this date is later or equal to d; otherwise returns FALSE.
*/
bool NMDate::operator>= ( const NMDate & d) const
{
	return jd >= d.jd;
}

/**
\fn  NMDate currentTime()
\brief Return the current local date
*/
NMDate NMDate::currentDate()
{
	// posix compliant system
	struct timeval tv;
	gettimeofday( &tv, 0 );
	time_t ltime = tv.tv_sec;
	tm *t;

	t = localtime( &ltime );

	NMDate date;
	date.setYMD( t->tm_year + 1900, t->tm_mon + 1 , t->tm_mday);
	return date;
}

/**
\fn NMDate::toString();
\brief Return a NMString with format YYYY-MM-DD
*/
NMString NMDate::toString()
{
	NMString m = NMString::number( month());
	if( m.Length() == 1)
		m = "0" + m;
	
	NMString d = NMString::number( day());
	if( d.Length() == 1)
		d = "0" + d;

	return NMString::number( year()) + "-" + m + "-" + d;
}

/**
\fn NMDate::fromString( const NMString &s)
\brief Returns the NMDate represented by the string s
*/
NMDate NMDate::fromString( const NMString &s)
{
	if( s.IsEmpty()) {
		cerr << "NMDate::fromString Warning: Parameter empty" << endl;
		return NMDate();
	}
	
	int year = s.subString( 0, 4).toInt();
	int month = s.subString( 5, 2).toInt();
	int day = s.subString( 8, 2).toInt();

	if ( day && month && year)
		return NMDate( year, month, day);
	return NMDate();
}

uint NMDate::gregorianToJulian( int y, int m, int d)
{
	uint c, ya;
	if ( y <= 99 )
		y += 2000;
	if ( m > 2 ) {
		m -= 3;
	} else {
        	m += 9;
		y--;
	}
	c = y;                                      // NOTE: Sym C++ 6.0 bug
	c /= 100;
	ya = y - 100*c;
	return 1721119 + d + (146097*c)/4 + (1461*ya)/4 + (153*m+2)/5;
}

void NMDate::julianToGregorian( int &y, int &m, int &d)
{
	uint x;
	uint j = jd - 1721119;
	y = (j*4 - 1)/146097;
	j = j*4 - 146097*y - 1;
	x = j/4;
	j = (x*4 + 3) / 1461;
	y = 100*y + j;
	x = (x*4) + 3 - 1461*j;
	x = (x + 4)/4;
	m = (5*x - 3)/153;
	x = 5*x - 3 - 153*m;
	d = (x + 5)/5;
	if ( m < 10 ) {
		m += 3;
	} else {
		m -= 9;
		y++;
	}
}


/*********************** NMTIME ******************************/

NMTime::NMTime( int h, int m, int s, int ms)
{
	ds = 0;
	setHMS( h, m, s, ms );
}

/**
\fn NMTime::isValid() const
\brief Returns TRUE if this time is valid; otherwise returns FALSE.
*/
bool NMTime::isValid() const
{
	return ds < MSECS_PER_DAY;
}

/**
\fn NMTime::hour() const
\brief return the hour of this time (0-24)
*/
int NMTime::hour() const
{
	return ds / MSECS_PER_HOUR;
}

/**
\fn NMTime::minute() const
\brief return the minutes of this time (0-59)
*/
int NMTime::minute() const
{
	return (ds % MSECS_PER_HOUR)/MSECS_PER_MIN;
}

/**
\fn NMTime::second() const
\brief return the seconds of this time (0-59)
*/
int NMTime::second() const
{
	return (ds / 1000)%SECS_PER_MIN;
}

/**
\fn NMTime::hour() const
\brief return the miliseconds of this time (0-99)
*/
int NMTime::msec() const
{
	return ds % 1000;
}

/**
\fn NMTime::addSecs( int sec)
\brief Add sec seconds to current NMTime
*/
void NMTime::addSecs( int sec)
{
	if (sec < 0) return;
	ds = (int)(ds + sec*1000) % MSECS_PER_DAY;
}

/**
\fn NMTime::setHMS( int h, int m, int s, int ms)
\brief Sets the time to hour h, minute m, seconds s and milliseconds ms.
\a h must be in the range 0..23, \a m and \a s must be in the range 0..59, and \a ms must be in the range 0..999. 
\return TRUE if the set time is valid; otherwise returns FALSE.
*/
bool NMTime::setHMS( int h, int m, int s, int ms)
{
	if ( !isValid(h,m,s,ms) ) {
		ds = MSECS_PER_DAY;
        	return false;
	}

	ds = (h*SECS_PER_HOUR + m*SECS_PER_MIN + s)*1000 + ms;
	return true;
}

bool NMTime::isValid( int h, int m, int s, int ms )
{
    return (uint)h < 24 && (uint)m < 60 && (uint)s < 60 && (uint)ms < 1000;
}

/**
\fn NMTime::operator== ( const NMTime &t) const
\brief Returns TRUE if this time is equal to t; otherwise returns FALSE.
*/
bool NMTime::operator== ( const NMTime &t) const
{
	return ds == t.ds;
}

/**
\fn NMTime::operator!= ( const NMTime & t) const
\brief Returns TRUE if this time is diferent to t; otherwise returns FALSE.
*/
bool NMTime::operator!= ( const NMTime &t) const
{
	return ds != t.ds;
}

/**
\fn NMTime::operator< ( const NMTime & t) const
\brief Returns TRUE if this time is earlier to t; otherwise returns FALSE.
*/
bool NMTime::operator< ( const NMTime &t) const
{
	return ds < t.ds;
}

/**
\fn NMTime::operator<= ( const NMTime &t) const
\brief Returns TRUE if this time is earlier or equal to t; otherwise returns FALSE.
*/
bool NMTime::operator<= ( const NMTime &t) const
{
	return ds <= t.ds;
}

/**
\fn NMTime::operator> ( const NMTime &t) const
\brief Returns TRUE if this time is later to t; otherwise returns FALSE.
*/
bool NMTime::operator> ( const NMTime &t) const
{
	return ds > t.ds;
}

/**
\fn NMTime::operator>= ( const NMTime &t) const
\brief Returns TRUE if this time is later or equal to t; otherwise returns FALSE.
*/
bool NMTime::operator>= ( const NMTime &t) const
{
	return ds >= t.ds;
}

/**
\fn  NMTime::currentTime()
\brief Return the current local time
*/
NMTime NMTime::currentTime()
{
// posix compliant system
	struct timeval tv;
	gettimeofday( &tv, 0 );
	time_t ltime = tv.tv_sec;
	tm *t;

	t = localtime( &ltime );

	NMTime time;
	time.setHMS( t->tm_hour, t->tm_min, t->tm_sec);
	return time;
}

/**
\fn NMTime::toString();
\brief Return a NMString with format HH:MM:SS
*/
NMString NMTime::toString()
{
	NMString h = NMString::number( hour());
	if( h.Length() == 1)
		h = "0" + h;

	NMString m = NMString::number( minute());
	if( m.Length() == 1)
		m = "0" + m;

	NMString s = NMString::number( second());
	if( s.Length() == 1)
		s = "0" + s;

	return h + ":" + m + ":" + s;
}

/**
\fn NMTime::fromString( const NMString &s)
\brief Returns the NMTime represented by the string s
*/
NMTime NMTime::fromString( const NMString &s)
{
	if( s.IsEmpty()) {
		cerr << "NMTime::fromString Warning: Parameter empty" << endl;
		return NMTime();
	}
	
	int hour = s.subString( 0, 2).toInt();
	int minute = s.subString( 3, 2).toInt();
	int second = s.subString( 6, 2).toInt();

	return NMTime( hour, minute, second, 0);   // set time
}


/*********************** NMDATETIME ******************************/

NMDateTime::NMDateTime()
: m_date(), m_time()
{
}

NMDateTime::NMDateTime( NMDate date, NMTime time)
: m_date(date), m_time(time)
{
}

NMDateTime::~NMDateTime()
{
}

/**
\fn NMDateTime::isNull()
\brief Returns TRUE if this datetime is NULL; otherwise returns FALSE.
*/
bool NMDateTime::isNull()
{
	return m_date.isNull() && m_time.isNull();
}

/**
\fn NMDateTime::isValid()
\brief Returns TRUE if this datetime is valid; otherwise returns FALSE.
*/
bool NMDateTime::isValid()
{
	return m_date.isValid() && m_time.isValid();
}

/**
\fn  NMDateTime::setDate( const NMDate &date)
\brief Constructs a datetime with date date
*/
void NMDateTime::setDate( const NMDate &date)
{
	m_date = date;
}

/**
\fn  NMDateTime::setTime( const NMTime &time)
\brief Constructs a datetime with time time
*/
void NMDateTime::setTime( const NMTime &time)
{
	m_time = time;
}

/**
\fn  NMDateTime::date()
\brief Return the date
*/
NMDate NMDateTime::date()
{
	return m_date;
}

/**
\fn  NMDateTime::time()
\brief Return the time 
*/
NMTime NMDateTime::time()
{
	return m_time;
}

/**
\fn  NMDateTime::operator== ( const NMDateTime &dt) const
\brief Returns TRUE if this datetime is equal to dt; otherwise returns FALSE.
*/
bool NMDateTime::operator== ( const NMDateTime &dt) const
{
	return  m_time == dt.m_time && m_date == dt.m_date;
}

/**
\fn  NMDateTime::operator!= ( const NMDateTime &dt) const
\brief Returns TRUE if this datetime is diferent to dt; otherwise returns FALSE.
*/
bool NMDateTime::operator!= ( const NMDateTime &dt) const
{
	return  m_time != dt.m_time || m_date != dt.m_date;
}

/**
\fn  NMDateTime::operator< ( const NMDateTime &dt) const
\brief Returns TRUE if this datetime is earlier to dt; otherwise returns FALSE.
*/
bool NMDateTime::operator< ( const NMDateTime &dt) const
{
	if ( m_date < dt.m_date )
		return true;
	return m_date == dt.m_date ? m_time < dt.m_time : false;
}

/**
\fn  NMDateTime::operator<= ( const NMDateTime &dt) const
\brief Returns TRUE if this datetime is earlier or equal to dt; otherwise returns FALSE.
*/
bool NMDateTime::operator<= ( const NMDateTime &dt) const
{
	if ( m_date < dt.m_date )
		return true;
	return m_date == dt.m_date ?  m_time <= dt.m_time : false;
}

/**
\fn  NMDateTime::operator> ( const NMDateTime &dt) const
\brief Returns TRUE if this datetime is later to dt; otherwise returns FALSE.
*/
bool NMDateTime::operator> ( const NMDateTime &dt) const
{
	if ( m_date > dt.m_date )
		return true;
	return m_date == dt.m_date ? m_time > dt.m_time : false;
}

/**
\fn  NMDateTime::operator>= ( const NMDateTime &dt) const
\brief Returns TRUE if this datetime is earlier or equal to dt; otherwise returns FALSE.
*/
bool NMDateTime::operator>= ( const NMDateTime &dt) const
{
	if ( m_date > dt.m_date )
		return true;
	return m_date == dt.m_date ?  m_time >= dt.m_time : false;
}


/**
\fn NMDateTime::toString();
\brief Return a NMString with format YYYY-MM-DDTHH:MM:SS
*/
NMString NMDateTime::toString()
{
	NMString s = "";
	if( isValid() )
		 s = m_date.toString() + "T" + m_time.toString();
	return s;
}

/**
\fn NMDateTime::fromString( const NMString &s)
\brief Returns the NMDateTime represented by the string s
*/
NMDateTime NMDateTime::fromString( const NMString &s)
{
	if( s.IsEmpty()) {
		cerr << "NMDateTime::fromString Warning: Parameter empty" << endl;
		return NMDateTime();
	}
	
	return NMDateTime( NMDate::fromString( s.subString( 0, 10)), NMTime::fromString( s.subString( 11, 8)));
}

/**
\fn NMDateTime::currentDateTime()
\brief Return the current DateTime
*/
NMDateTime NMDateTime::currentDateTime()
{
	return NMDateTime( NMDate::currentDate(), NMTime::currentTime());
}
