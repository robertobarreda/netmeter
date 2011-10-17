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
#include "nmggraphicserror.h"
#include <nmgmoduleutils.h>
#include <QMessageBox>

NMGGraphicsError::NMGGraphicsError(QString title, 
                                   QString error, 
                                   NMGGraphicsError::Error typeOfError)
  :title(title), error(error), type(typeOfError)
{}

void NMGGraphicsError::raiseError()
{
  switch(type)
  {
    case WARNING:
      QMessageBox::warning(NMGModuleUtils::getMainWindow(),title,error);
      break;
    case CRITICAL:
      QMessageBox::critical(NMGModuleUtils::getMainWindow(),title,error);
      break;
  }
}
