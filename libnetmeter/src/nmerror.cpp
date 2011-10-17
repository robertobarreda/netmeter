/***************************************************************************
 *   Copyright (C) 2005 by Pau Capella   *
 *   pcapella@ac.upc.edu   *
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
#include "nmerror.h"

NMError::NMError()
{
	message = "";
}

NMError::~NMError()
{
}

/**
  @brief NMError construct with the error tpye and the message.
*/
NMError::NMError(NMErrorType errtype, const NMString &errmsg)
{
	type = errtype;
	message = errmsg;
}

/**
  @brief Set an error.
  @param errtype Error type.
  @param errmsg Message
*/
void NMError::setError(NMErrorType errtype, const NMString &errmsg)
{
	type = errtype;
	message = errmsg;
}

/**
  @brief Return the error message.
*/
NMString NMError::getMessage() const
{
	return message;
}

/**
  @brief Return the error type.
*/
int NMError::getType() const
{
	return type;
}


std::ostream &operator<<(std::ostream &str, const NMError &obj)
{
	switch (obj.type) {
		case NMError::warning:
			str << "WARNING: " << obj.message << '\n';
			break;
		case NMError::user:
			str << "ERROR: " << obj.message << '\n';
			break;
		case NMError::fatal:
			str << "CRITICAL ERROR: "<< obj.message << '\n';
			break;
		case NMError::debug:
			str << "DEBUG: " << obj.message << '\n';
			break;
	}
	return str;
}





