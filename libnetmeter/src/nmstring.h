/***************************************************************************
 *   Copyright (C) 2004 by David Lacort Ankli                              *
 *   david.lacort@gmail.com                                                          *
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
#ifndef NMSTRING_H
#define NMSTRING_H

// Colored output
#define BLACK        "\033[01;30m"
#define DARKGRAY     "\033[01;30m"
#define BLUE         "\033[00;34m"
#define LIGHTBLUE    "\033[01;34m"
#define GREEN        "\033[00;32m"
#define LIGHTGREEN   "\033[01;32m"
#define CYAN         "\033[00;36m"
#define LIGHTCYAN    "\033[01;36m"
#define RED          "\033[00;31m"
#define LIGHTRED     "\033[01;31m"
#define PURPLE       "\033[00;35m"
#define LIGHTPURPLE  "\033[01;35m"
#define BROWN        "\033[00;33m"
#define YELLOW       "\033[01;33m"
#define LIGHTGRAY    "\033[00;37m"
#define WHITE        "\033[01;37m"
// Restore default color output
#define ENDCOLOR     "\033[m"

#include <iostream>

/**
  * @author David Lacort Ankli    
  * @brief This class implements the strings for NetMeter
  */

class NMString
{                            
public:
  // Constructors
  NMString();
  NMString(const NMString &nmsSrc);
  NMString(const char *cptr_src);
  NMString(int i_src);
  
  // Destructor
  ~NMString();
  
  // Assignments
  NMString& operator =(const NMString &nmsStr);
  NMString& operator +=(const NMString &nmsStr);
  
  // Comparisons
  bool operator ==(const NMString &nmsStr) const;
  bool operator !=(const NMString &nmsStr) const;
  bool operator <(const NMString &nmsStr) const;
  
  // Concatenation
  NMString operator +(const NMString &nmsStr) const;
  friend NMString operator +(const char *cptr_Str1, const NMString &nmsStr2);

  // Array functionalities
  NMString operator[](const int i_index) const;
  int getWord(NMString *nmsWord);
  bool endOfString() const;
  void resetOffset() {m_i_offset = 0;};
      
  // C string operator
  char* c_str(void) const;
  int search(const NMString &nmsStr) const;
  NMString subString(int i_index, int i_length) const;
  NMString trimRight() const;
  NMString trimLeft() const;
  NMString trim() const;
    
  // Numeric
  static NMString number(int i_num, int i_base=10);
  int toInt() const;

  // Query attributes of string
  int Length() const;
  int Offset() const;
  bool IsEmpty() const;

  static NMString random();
  

private:
  int m_i_length;
  int m_i_offset;
  char *m_cptr_theString;

public:

  friend std::ostream & operator<<(std::ostream &str, const NMString &obj);

};


#endif
