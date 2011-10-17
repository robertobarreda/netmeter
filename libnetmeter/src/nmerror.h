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
#ifndef NMERROR_H
#define NMERROR_H

#include "nmstring.h"
/**
@author Pau Capella
@brief Provides NetMeter error information.
*/
class NMError{

public:

    /**
	\enum NMErrorType
	\param warning Warning usually for plugins.
	\param user Error usually for plugins.
	\param fatal Critical error in NetMeter core.
	\param debug For developers.
   */
    enum NMErrorType {
	fatal = 1,
	user = 5,
	warning = 10,
	debug = 20
    };

    NMError();
    NMError(NMErrorType errtype, const NMString &errmsg);
    ~NMError();

    void setError(NMErrorType errtype, const NMString &errmsg);

    NMString getMessage() const;
    int getType() const;

    friend std::ostream & operator<<(std::ostream &str, const NMError &obj);

private:
	NMString message;
	NMErrorType type;
};

#endif
