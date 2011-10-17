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
#include "nmgmetadataparser.h"
#include "nmgmetadatatags.h"

NMGMetaDataParser::NMGMetaDataParser(NMGMetaData& metadata)
  : QXmlStreamReader(), metadata(metadata)
{
  isMetadataFinished = FALSE;
  stopParsing = FALSE;
  completedPercent = 0;
  status = "Parser initialized";
}

bool NMGMetaDataParser::isMetaDataRead() const
{
  return isMetadataFinished;
}

bool NMGMetaDataParser::read(QIODevice* device)
{
  setDevice(device);
  completedPercent = 1;
  status = "Reading metedata";

  while (!atEnd() && !isMetadataFinished && !stopParsing) 
  {
    readNext();
  
    if (isStartElement()) 
    {
      if (name() == TAG_TEST) readTest();
      else raiseError(QObject::tr("The file is not a test file."));
    }
  }
  return !error();
}

void NMGMetaDataParser::readTest()
{
  Q_ASSERT(isStartElement() && name() == TAG_TEST);

  while (!atEnd() && !isMetadataFinished && !stopParsing) 
  {
    readNext();

    if (isStartElement()) 
    {
      if (name() == TAG_METADATA) readMetadata();
      else raiseError(QObject::tr("The parser didn't find the expected metadata information"));
    }
  }
}

void NMGMetaDataParser::readMetadata()
{
  Q_ASSERT(isStartElement() && name() == TAG_METADATA);

  while (!atEnd() && !stopParsing) 
  {
    readNext();

    if (isEndElement()) 
    {
      isMetadataFinished = TRUE;
      completedPercent = 100;
      status = "Metadata read";
      break;
    }
    
    if (isStartElement()) 
    {
      if (name() == TAG_NAME) readName();
      else if (name() == TAG_IDENTIFIER) readIdentifier();
      else if (name() == TAG_TYPE) readTypeOfFile();
      else if (name() == TAG_VERSION) readVersion();
      else if (name() == TAG_SOURCE) readSource();
      else if (name() == TAG_DESTINATION) readDestination();
      else readUncommonElement();
    }
  }
}

void NMGMetaDataParser::readName()
{
  Q_ASSERT(isStartElement() && name() == TAG_NAME);
  metadata.setTestName(readElementText());
}

void NMGMetaDataParser::readIdentifier()
{
  Q_ASSERT(isStartElement() && name() == TAG_IDENTIFIER);
  metadata.setTestIdentifier(readElementText());
}

void NMGMetaDataParser::readTypeOfFile()
{
  Q_ASSERT(isStartElement() && name() == TAG_TYPE);
  metadata.setTypeOfTest(readElementText());
}

void NMGMetaDataParser::readVersion()
{
  Q_ASSERT(isStartElement() && name() == TAG_VERSION);
  QString version = readElementText();
  if(version.isEmpty()) metadata.setVersion(-1);
  else metadata.setVersion(version.toInt());
}

void NMGMetaDataParser::readSource()
{
  Q_ASSERT(isStartElement() && name() == TAG_SOURCE);

  while (!atEnd() && !stopParsing) 
  {
    readNext();

    if (isEndElement()) 
    {
      completedPercent = 25;
      break;
    }
    if (isStartElement()) 
    {
      if (name() == TAG_ADDRESS) readSourceAddress();
      else if (name() == TAG_MANAGEMENT) readSourceManagement();
      else readUncommonElement();
    }
  }
}

void NMGMetaDataParser::readSourceAddress()
{
  Q_ASSERT(isStartElement() && name() == TAG_ADDRESS);
  metadata.setSourceAddress(readElementText());
}

void NMGMetaDataParser::readSourceManagement()
{
  Q_ASSERT(isStartElement() && name() == TAG_MANAGEMENT);

  while (!atEnd() && !stopParsing) 
  {
    readNext();

    if (isEndElement()) break;

    if (isStartElement()) 
    {
      if (name() == TAG_ADDRESS) readSourceManagementAddress();
      else if (name() == TAG_PORT) readSourceManagementPort();
      else readUncommonElement();
    }
  }  
}

void NMGMetaDataParser::readSourceManagementAddress()
{
  Q_ASSERT(isStartElement() && name() == TAG_ADDRESS);
  metadata.setManagementSourceAddress(readElementText());
}

void NMGMetaDataParser::readSourceManagementPort()
{
  Q_ASSERT(isStartElement() && name() == TAG_PORT);
  QString port = readElementText();
  if(port.isEmpty()) metadata.setManagementSourcePort(-1);
  else metadata.setManagementSourcePort(port.toInt());
}
    
void NMGMetaDataParser::readDestination()
{
  Q_ASSERT(isStartElement() && name() == TAG_DESTINATION);
  
  while (!atEnd() && !stopParsing) 
  {
    readNext();

    if (isEndElement()) 
    {
      completedPercent = 50;
      break;
    }
    if (isStartElement()) 
    {
      if (name() == TAG_ADDRESS) readDestinationAddress();
      else if (name() == TAG_MANAGEMENT) readDestinationManagement();
      else readUncommonElement();
    }
  }  
}

void NMGMetaDataParser::readDestinationAddress()
{
  Q_ASSERT(isStartElement() && name() == TAG_ADDRESS);
  metadata.setDestinationAddress(readElementText());
}

void NMGMetaDataParser::readDestinationManagement()
{
  Q_ASSERT(isStartElement() && name() == TAG_MANAGEMENT);

  while (!atEnd() && !stopParsing) 
  {
    readNext();

    if (isEndElement()) break;

    if (isStartElement()) 
    {
      if (name() == TAG_ADDRESS) readDestinationManagementAddress();
      else if (name() == TAG_PORT) readDestinationManagementPort();
      else readUncommonElement();
    }
  }
}

void NMGMetaDataParser::readDestinationManagementAddress()
{
  Q_ASSERT(isStartElement() && name() == TAG_ADDRESS);
  metadata.setManagementDestinationAddress(readElementText());
}

void NMGMetaDataParser::readDestinationManagementPort()
{
  Q_ASSERT(isStartElement() && name() == TAG_PORT);
  QString port = readElementText();
  if(port.isEmpty()) metadata.setManagementDestinationPort(-1);
  else metadata.setManagementDestinationPort(port.toInt());
}

void NMGMetaDataParser::readUncommonElement()
{
  Q_ASSERT(isStartElement());

  QString content;
  // processing start element and its attributes if exist
  writeStartElement(name().toString(), attributes(), content);

  while (!atEnd() && !stopParsing) 
  {
    readNext();
    if (isEndElement()) 
    {
      writeEndElement(name().toString(), content);
      metadata.addUncommonElement(name().toString(), content);
      break;
    }
    else if(isStartElement()) readExtraContentUncommonElement(content);
    else if(isCharacters()) content.append(text().toString());
  }
}

void NMGMetaDataParser::readExtraContentUncommonElement(QString& content)
{
  Q_ASSERT(isStartElement());

  // processing start element and its attributes if exist
  writeStartElement(name().toString(), attributes(), content);

  while (!atEnd()) 
  {
    readNext();
    if (isEndElement()) 
    {
      writeEndElement(name().toString(), content);
      break;
    }
    else if(isStartElement()) readExtraContentUncommonElement(content);
    else if(isCharacters()) content.append(text().toString());
  }
}

void NMGMetaDataParser::writeStartElement(QString name, 
                                          QXmlStreamAttributes atrib, 
                                          QString& content)
{
  content.append("<");
  content.append(name);
  QVector<QXmlStreamAttribute>::iterator it;
  for(it = atrib.begin(); it != atrib.end(); it++) {
    content.append(" ");
    content.append((*it).name().toString());
    content.append("=\"");
    content.append((*it).value().toString());
    content.append("\"");
  }
  content.append(">");
}

void NMGMetaDataParser::writeEndElement(QString name, QString& content)
{
  content.append("</");
  content.append(name);
  content.append(">");
}

/* Alternative implementation to read uncommon elements
void NMGMetaDataParser::readUncommonElement()
{
  Q_ASSERT(isStartElement());

  QString content;
  QXmlStreamWriter writer(&content);
  writer.writeStartElement(name().toString());
  writer.writeAttributes(attributes());

  while (!atEnd()) 
  {
    readNext();
    if (isEndElement()) 
      {
      writer.writeEndElement();
      metadata.addUncommonElement(name().toString(), content);
      break;
      }
    else if(isStartElement()) readExtraContentUncommonElement(content, writer);
    else if(isCharacters()) writer.writeCharacters(text().toString());
  }
}

void NMGMetaDataParser::readExtraContentUncommonElement(QString& content,
                                                        QXmlStreamWriter& writer)
{
  Q_ASSERT(isStartElement());

  writer.writeStartElement(name().toString());
  writer.writeAttributes(attributes());

  while (!atEnd()) 
  {
    readNext();

    if (isEndElement()) 
    {
      writer.writeEndElement();
      break;
    }
    else if(isStartElement()) readExtraContentUncommonElement(content, writer);
    else if(isCharacters()) writer.writeCharacters(text().toString());
  }
}
*/

void NMGMetaDataParser::stop()
{
  stopParsing = TRUE;
  raiseError(QObject::tr("Parser cancelled by user"));
}

int NMGMetaDataParser::getCompletedPercent() const
{
  return completedPercent;
}

QString NMGMetaDataParser::getStatus() const
{
  return status;
}
