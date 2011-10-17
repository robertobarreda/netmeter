/***************************************************************************
 *   Copyright (C) 2007 by Eduard Bellot                                   *
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
#ifndef NMGGRAPHICSERROR_H
#define NMGGRAPHICSERROR_H

#include <QString>

/**
 * \brief NMGGraphicsError is the class used by the Graphic Analysis plugin to emit possible
 * errors to the user. Errors are emitted as modal dialog windows.
 */
class NMGGraphicsError
{
  public:

    /** \brief Supported error types */
    enum Error 
    {
      WARNING,
      CRITICAL
    };

    /**
     * \brief Class constructor.
     * \param title Window title.
     * \param error Message to be shown.
     * \param typeOfError Error type from supported error types.
     */
    NMGGraphicsError(QString title, QString error, NMGGraphicsError::Error typeOfError);

    /** \brief Shows the specified error. */
    void raiseError();

    /** \brief Returns the window title assigned to the error. */
    inline QString getErrorTitle() const { return title; };

    /** \brief Returns the message assigned to the error. */
    inline QString getError() const { return error; };

    /** \brief Returns the type of error assigned to the error. */
    inline Error getErrorType() const { return type; };

    /** 
     * \brief Saves a new window title for the error.
     * \param errorTitle New window title.
     */
    inline void setErrorTitle(QString errorTitle) { title = errorTitle; }

    /**
     * \brief Saves a new message for the error.
     * \param errorMsg New error message.
     */
    inline void setError(QString errorMsg) { error = errorMsg; }

    /**
     * \brief Changes the type of error.
     * \brief New type of error.
     */
    inline void setErrorType(NMGGraphicsError::Error errorType) { type = errorType; } 

  private:
    QString title;
    QString error;
    Error type;
};

#endif
