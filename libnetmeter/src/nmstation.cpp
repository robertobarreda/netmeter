/*
 *   $Id: nmstation.cpp,v 1.1.1.1 2007-05-26 17:41:22 rserral Exp $
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
#include "nmstation.h"

/**
\fn NMHost::NMStation()
\brief Creates a new station object
*/
NMStation::NMStation()
:m_management(), m_test()
{
}

/**
\fn NMHost::NMStation()
\brief Destroy station object
*/
NMStation::~NMStation()
{
}

/**
\fn NMStation::management()
\brief Return the management host
*/
NMHost NMStation::management()
{
	return m_management;
}

/**
\fn NMStation::test()
\brief Return the test host
*/
NMHost NMStation::test()
{
	return m_test;
}

/**
\fn NMStation::password()
\brief Return the password for the management host
*/
NMString NMStation::password()
{
	return m_managepassword;
}

/**
\fn NMStation::username()
\brief Return the username for the management host
*/
NMString NMStation::username()
{
	return m_manageusername;
}

/**
\fn NMStation::setManagement( const NMHost host)
\brief Set the host for management station
\param host 
*/	
void NMStation::setManagement( const NMHost host)
{
	m_management = host;
}

/**
\fn NMStation::setTest( const NMHost host)
\brief Set the host for test station
\param host 
*/
void NMStation::setTest( const NMHost host)
{
	m_test = host;
}

/**
\fn NMStation::setPassword( const NMString password)
\brief Set the password for the management station
\param password
*/
void NMStation::setPassword( const NMString password)
{
	m_managepassword = password;
}

/**
\fn NMStation::setUsername (const NMString username)
\brief Set the username for the management station
\param password
*/
void NMStation::setUsername (const NMString username)
{
	m_manageusername = username;
}


