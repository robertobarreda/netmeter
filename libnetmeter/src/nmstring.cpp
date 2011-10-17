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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "nmstring.h"

NMString::NMString()
/**
  @brief Creates an instance without data.
*/
{
  m_cptr_theString = 0;
  m_i_length = 0;
  m_i_offset = 0;
}

NMString::NMString(const NMString &nmsSrc)
/**
  @brief Creates an instance with the source data (Makes a copy of the data).
  
  @param nmsSrc   The string that must be copied
*/
{
  int i_length = nmsSrc.Length();
  m_cptr_theString = (char*)malloc(i_length+1);
  memset(m_cptr_theString,0x0,i_length+1);
  strncpy(m_cptr_theString,nmsSrc.c_str(),i_length);
  m_i_length = i_length;
  m_i_offset = 0;
}

NMString::NMString(const char *cptr_data)
/**
  @brief Creates an instance with the source data.
  
  @param cptr_data   The array of chars that must be copied
*/
{
  int i_length;

  if (!cptr_data) {
     m_cptr_theString = 0;
     m_i_length = 0;
     m_i_offset = 0;
     return;
  }
  i_length = strlen(cptr_data);
  m_cptr_theString = (char*)malloc(i_length+1); 
  memset(m_cptr_theString,0x0,i_length+1);
  strncpy(m_cptr_theString,cptr_data,i_length);
  m_i_length = i_length;
  m_i_offset = 0;
}

NMString::NMString(int i_src)
/**
  @brief Creates an instance with the source data.
  
  @param i_src   The int that must be converted and copied
*/
{
  // Added by rserral (g++-4 fix)
  m_cptr_theString = 0;
  m_i_length = 0;
  m_i_offset = 0;
  // End Added rserral
  *this = number(i_src);
}

NMString::~NMString()
/**
  @brief Destroys the instance.
*/
{
  free(m_cptr_theString);
  m_cptr_theString = 0;
}

NMString &NMString::operator =(const NMString &nmsStr)
/**
  @brief Assignation.
  
  @param nmsStr   The string with the original data
*/
{
  char *tmp;
  int i_newLength = nmsStr.Length();
  if (!m_cptr_theString)
  	m_cptr_theString = (char *)malloc(i_newLength+1);
  else {
    tmp = (char*)malloc(i_newLength+1);
    if (tmp) {
      free(m_cptr_theString);
      m_cptr_theString = tmp;
    } else {
      return *this;
    }
  }
  memset(m_cptr_theString,0x0,i_newLength+1);
  strncpy(m_cptr_theString,nmsStr.c_str(),i_newLength);
  m_i_length = i_newLength;
  m_i_offset = 0;
  
  return *this;
}

NMString NMString::operator +(const NMString &nmsStr) const
/**
  @brief Concatenate two strings.
  
  @param nmsStr   The second string in the concatenation
  @return   Returns the string with the current and the param string concatenated
*/
{
  NMString nmsResult;
  int i_newLength;
  char *cptr_newString;
  
  i_newLength = m_i_length + nmsStr.Length();
  cptr_newString = new char[i_newLength+1];
  memset(cptr_newString,0x0,i_newLength+1);
  // Copy the string in the new string (without the final 0x0)
  strncpy(cptr_newString,m_cptr_theString,m_i_length);
  // Copy the string of the parameter NMString beside the first one with the 0x0
  strncpy(&cptr_newString[m_i_length],nmsStr.c_str(),nmsStr.Length()+1);
  nmsResult = NMString(cptr_newString);
  delete []cptr_newString;
  
  return nmsResult;
}

NMString operator +(const char *cptr_Str1, const NMString &nmsStr2)
/**
  @brief Concatenate two strings.
  
  @param nmsStr   The second string in the concatenation
  @return   Returns the string with the current and the param string concatenated
*/
{
  NMString nmsResult;
  int i_newLength;
  char *cptr_newString;
  
  i_newLength = strlen(cptr_Str1) + nmsStr2.Length();
  cptr_newString = new char[i_newLength+1];
  memset(cptr_newString,0x0,i_newLength+1);
  // Copy the 1st string (the char pointer) in the new string (without the final 0x0)
  strncpy(cptr_newString,cptr_Str1,strlen(cptr_Str1));
  // Copy the string of the parameter NMString beside the first one with the 0x0
  strncpy(&cptr_newString[strlen(cptr_Str1)],nmsStr2.c_str(),nmsStr2.Length()+1);
  nmsResult = NMString(cptr_newString);
  delete []cptr_newString;
  
  return nmsResult;
}

NMString &NMString::operator +=(const NMString &nmsStr)
/**
  @brief Concatenate a string with the current one.
  
  @param nmsStr   The second string in the concatenation
*/
{
  int i_newLength;
  char *tmp;

  i_newLength = m_i_length + nmsStr.Length();
  if (!m_cptr_theString)
  	m_cptr_theString = (char *)malloc(i_newLength+1);
  else {
    tmp = (char *)malloc(i_newLength+1);
    if (tmp) {
      memcpy(tmp, m_cptr_theString, m_i_length);
      free(m_cptr_theString);
      m_cptr_theString = tmp;
    } else {
      return *this;
    }
  }
  strncpy(&m_cptr_theString[m_i_length],nmsStr.c_str(),nmsStr.Length()+1);
  m_i_length = i_newLength;
  
  return *this;
}

bool NMString::operator ==(const NMString &nmsStr) const
/**
  @brief If the content of both strings are the same, return true. Otherwise return false.
  
  @param nmsStr   The second string to be compared
  @return   Return true if the content of both strings are identical, false otherwise
*/
{
  return ((m_i_length==nmsStr.Length()) && (strcmp(m_cptr_theString,nmsStr.c_str())==0));
}

bool NMString::operator !=(const NMString &nmsStr) const
/**
  @brief If the content of both strings are the same, return true. Otherwise return false.
  
  @param nmsStr   The second string to be compared
  @return   Return true if the content of both strings are identical, false otherwise
*/
{
  return ((m_i_length!=nmsStr.Length()) || (strcmp(m_cptr_theString,nmsStr.c_str())!=0));
}

bool NMString::operator <(const NMString &nmsStr) const
{
    return std::string(m_cptr_theString) < std::string(nmsStr.c_str());
} 

NMString NMString::operator[](const int i_index) const
/**
  @brief Returns the string that starts in the parameter index.
  
  @param i_index   The index where the returned string have to start
  @return    Returns the string
*/
{
  if (i_index>=m_i_length)
    return '\0';
  return NMString(&m_cptr_theString[i_index]);
}

int NMString::getWord(NMString *nmsWord)
/**
  @brief Fetches the next word.
  
  @param nmsWord   The holder of the word (if success)
  @return   Returns the index of first char behind the word if success or -1 on error
*/
{
  // Search the first char not blank
  while(m_i_offset<m_i_length && m_cptr_theString[m_i_offset]==' ')
  {
    m_i_offset++;
  }
  
  // if found some char not blank...
  if (m_i_offset<m_i_length)
  {
    int i_firstChar = m_i_offset;
    // we have the first char, so we'll search the end of the word
    while(m_i_offset<m_i_length && m_cptr_theString[m_i_offset]!=' ')
    {
      m_i_offset++;
    }
    
    *nmsWord = this->subString(i_firstChar, m_i_offset-i_firstChar);
    return m_i_offset;
  }
  else
    return -1;
}

bool NMString::endOfString() const
/**
  @brief Returns true if thre's no more words beyond the offset
*/
{
  int i_index = m_i_offset;
  // Search the first char not blank
  while(i_index<m_i_length && m_cptr_theString[i_index]==' ')
  {
    i_index++;
  }
  return !(i_index<m_i_length);
}

char* NMString::c_str(void) const
/**
  @brief Returns the pointer to the array of chars that holds the string data.
  
  @return   Returns a pointer to the array of chars
*/
{
  return m_cptr_theString;
}

int NMString::search(const NMString &nmsStr) const
/**
  @brief Searchs the input string in the data and returns the index where it was found.

  @param nmsStr   A NMString parameter that holds the string to be searched
  @return   Returns the index where the string was found, -1 if the string is not in the data
*/
{
  char *cptr_strAux;
  
  cptr_strAux = strstr(m_cptr_theString,nmsStr.c_str());
  if (cptr_strAux == NULL)
    return -1;
  else
    return (cptr_strAux - m_cptr_theString);
}

NMString NMString::subString(int i_index, int i_length) const
/**
  @brief Returns the part of the string that starts at the index position and is, at least, length bytes long.
  
  @param i_index    The position where the return string starts
  @param i_length   The length of the return string if thre's enough data
  @return   Returns the string
*/
{
  NMString nmsRet;
  char *cptr_stringAux;
  int i_realLength;
  
  if (i_length<0)
    i_length = 0;
  
  // Create a new char[] to make the copy of the data
  i_realLength = (i_length<(m_i_length-i_index)?i_length:(m_i_length-i_index));
  cptr_stringAux = new char[i_realLength+1];
  memset(cptr_stringAux,0x00,i_realLength+1);
  strncpy(cptr_stringAux,&m_cptr_theString[i_index],i_realLength);
  // Create a new string with the copied data
  nmsRet = NMString(cptr_stringAux);
  delete []cptr_stringAux;
  return nmsRet;
}

NMString NMString::trimRight() const
/**
  @brief Returns the string without the blank chars of the right.
*/
{
  int i;
  for(i=m_i_length;i>0 && m_cptr_theString[i-1]==' ';i--); // Go back to the last char not blank.
  return NMString(*this).subString(0,i);
}

NMString NMString::trimLeft() const
/**
  @brief Returns the string without the blank chars of the left.
*/
{
  int i;
  for(i=0;i<m_i_length && m_cptr_theString[i]==' ';i++); // Advance to the first char not blank.
  return NMString(&m_cptr_theString[i]);
}

NMString NMString::trim() const
/**
  @brief Returns the string without the blank chars of both sides
*/
{
  return (*this).trimLeft().trimRight();
}

NMString NMString::number(int i_num, int i_base)
/**
  @brief Returns an string containing the integer i_num in base i_base.
  
  @param i_num      The number to be converted
  @param i_length   The base of the number to be converted
  @return   Returns the string containing the converted integer
*/
{
  NMString nmsRet;
  char cptr_result[32+1];  // 32 chars if base = 2, plus one for the null
  char* cptr_ptrRes;
  bool sign = i_num < 0;
  i_num = abs(i_num);
  switch(i_base)
  {
    case 8:
    case 10:
    case 16:
      if (sign)
      {
        cptr_result[0] = '-';
	cptr_ptrRes = &cptr_result[1];
      }
      else
        cptr_ptrRes = cptr_result;
      sprintf(cptr_ptrRes,(i_base==8?"%o":(i_base==10?"%d":"%x")),i_num);
      cptr_ptrRes = cptr_result;
      break;
    
    default:
      cptr_result[32] = 0;
      cptr_ptrRes = cptr_result+32;
      do 
      {
        *--cptr_ptrRes = "0123456789abcdefghijklmnopqrstuvwxyz"[i_num % i_base];
        i_num /= i_base;
      } while (i_num);
      if (sign) 
        *--cptr_ptrRes = '-';
  }
  nmsRet = NMString(cptr_ptrRes);
  return nmsRet;
}

int NMString::toInt() const
/**
  @brief Returns the integer part at the beginning of the string. Can be hexadecimal (starts with '0x'), octal (starts with '0') or decimal.
*/
{
  return strtol(m_cptr_theString,NULL,0);
}

int NMString::Length() const
/**
  @brief Returns the length of the string
*/
{
  return m_i_length;
}

int NMString::Offset() const
/**
  @brief Returns the current offset
*/
{
  return m_i_offset;
}

bool NMString::IsEmpty() const
{
	if(m_i_length > 0)
		return false;
	else
		return true;
}

NMString NMString::random()
{
	char word[9];

	for(int i=0; i<8; i++) 
		word[i] = (char) ((rand() % 25) + 97);
	word[8] = '\0';
	return word;
}


std::ostream &operator<<(std::ostream &str, const NMString &obj)
{
	str << obj.m_cptr_theString;
	return str;
}

