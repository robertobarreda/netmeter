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
#ifndef NMGMETADATA_H
#define NMGMETADATA_H

#include <QHash>

/**
 * \brief NMGMetadata class gives support to NMGMetaDataParser.
 *
 * NMGMetadata contains the metadata information found in *.xff files. Because metadata section
 * is partially different depending on file type, NMGMetadata has been designed considering that
 * situation, so it saves the common properties on one side, and the uncommon properties on
 * another. The last one has been designed as a collection of uncommon properties that are 
 * accesed with iterators and utility methods. In that point, it is important to remark that this 
 * uncommon properties haven't being processed so they are saved as found in file. It is 
 * programmer's responsability to give meaning to uncommon tags.
 *
 * \sa NMGMetaDataParser
 */
class NMGMetaData
{
  public:

    /**
     * \brief Default constructor.
     *
     * It initializates members in class.
     */
    NMGMetaData();

    /**
     * \brief Saves the internal test name.
     * \param name Test name found in metadata section. Since name is extracted from file it has
     * no effect in the file itself if test name is changed from the original one.
     */
    inline void setTestName(QString name) { testName = name; }

    /**
     * \brief Saves the internal test identifier.
     * \param id Test identifier found in metadata section. Since id is extracted from file it has
     * no effect in the file itself if test identifier is changed from the original one.
     */
    inline void setTestIdentifier(QString id) { testIdentifier = id; }

    /**
     * \brief Saves the version of the program used to generate tests in file.
     * \param ver Version found in metadata section. Since ver is extracted from file it has
     * no effect in the file itself if version is changed from the original one.
     */
    inline void setVersion(int ver) { version = ver; }

    /**
     * \brief Saves the test file type.
     * \param type Test file type found in metadata section. Since type is extracted from file 
     * it has no effect in the file itself if file type is changed from the original one.
     */
    inline void setTypeOfTest(QString type) { typeOfTest = type; }

    /**
     * \brief Saves the address, usually as IP format, of test origin.
     * \param source Address found in metadata section. Since source is extracted from file 
     * it has no effect in the file itself if address is changed from the original one.
     */
    inline void setSourceAddress(QString source) { srcAddress = source; }

    /**
     * \brief Saves the address, usually as host name format, for the computer which generates 
     * traffic to test network.
     * \param source Address found in metadata section. Since source is extracted from file 
     * it has no effect in the file itself if management source address is changed from the 
     * original one.
     */
    inline void setManagementSourceAddress(QString source) { srcManagementAddress = source; }

    /**
     * \brief Saves the port number for which the computer generates traffic to test network.
     * \param port Port number found in metadata section. Since port is extracted from file 
     * it has no effect in the file itself if port number is changed from the original one.
     */
    inline void setManagementSourcePort(int port) { srcManagementPort = port; }

    /**
     * \brief Saves the address, usually as IP format, of test destination.
     * \param destination Address found in metadata section. Since destination is extracted from 
     * file it has no effect in the file itself if address is changed from the original one.
     */
    inline void setDestinationAddress(QString destination) { destAddress = destination; }

    /**
     * \brief Saves the address, usually as host name format, for the computer which receives 
     * traffic from test network.
     * \param destination Address found in metadata section. Since destination is extracted from 
     * file it has no effect in the file itself if management destination address is changed from 
     * the original one.
     */
    inline void setManagementDestinationAddress(QString destination) { destManagementAddress = destination; }

    /**
     * \brief Saves the port number for which the computer receives traffic from test network.
     * \param port Port number found in metadata section. Since port is extracted from file 
     * it has no effect in the file itself if port number is changed from the original one.
     */
    inline void setManagementDestinationPort(int port) { destManagementPort = port; }

    /**
     * \brief Returns the internal test name 
     * \return Test name if it exists or empty string if it is not defined.
     */
    inline QString getTestName() const { return testName; }

    /**
     * \brief Returns the internal test identifier.
     * \return Test identifier if it exists or empty string if it is not defined.
     */
    inline QString getTestIdentifier() const { return testIdentifier; }

    /**
     * \brief Returns the version of the program used to generate tests in file.
     * \return Version if it exists or -1 if it is not defined.
     */
    inline int getVersion() const { return version; }

    /**
     * \brief Returns test file type.
     * \return Type of test if it exists or empty string if it is not defined.
     */
    inline QString getTypeOfTest() const { return typeOfTest; }

    /**
     * \brief Returns the address, usually as IP format, of test origin.
     * \return Source address if it exists or empty string if it is not defined.
     */
    inline QString getSourceAddress() const { return srcAddress; }

    /**
     * \brief Returns the address, usually as host name format, for the computer which generates 
     * traffic to test network.
     * \return Source address if it exists or empty string if it is not defined.
     */
    inline QString getManagementSourceAddress() const { return srcManagementAddress; }

    /**
     * \brief Returns the port number for which the computer generates traffic to test network.
     * \return Port number if it exists or -1 if it is not defined.
     */
    inline int getManagementSourcePort() const { return srcManagementPort; }

    /**
     * \brief Returns the address, usually as IP format, of test destination.
     * \return Destination address if it exists or empty string if it is not defined.
     */
    inline QString getDestinationAddress() const { return destAddress; }

    /**
     * \brief Returns the address, usually as host name format, for the computer which receives 
     * traffic from test network.
     * \return Destination address if it exists or empty string if it is not defined.
     */
    inline QString getManagementDestinationAddress() const { return destManagementAddress; }

    /**
     * \brief Returns the port number for which the computer receives traffic from test network.
     * \return Port number if it exists or -1 if it is not defined.
     */
    inline int getManagementDestinationPort() const { return destManagementPort; }    

    /**
     * \brief Allows to save a new uncommon element found in metadata by parser
     *
     * To add a new element, parser must find an uncommon tag as follows: \<tag\>content\</tag\> 
     * where content can contain more uncommon tags or text. It is saved with an uncommon tag 
     * identifier (e.g. tag) and an uncommon tag string (e.g. \<tag\>content\</tag\>). With this 
     * design, uncommon tags are registered but not processed.
     *
     * \param tag Uncommon tag identifier.
     * \param tagBody Data contained between the starting of the uncommon tag and its ending.
     */
    inline void addUncommonElement(QString tag, QString tagBody) { uncommonTags.insert(tag, tagBody); };

    /**
     * \brief Returns an entire string containing all data of a specific uncommon tag identifier.
     *
     * The identifier must be a simple text without especial xml characters (e.g. tag instead of 
     * \<tag\>).
     *
     * \param tag Uncommon tag identifier.
     * \return Data for the uncommon tag identifier or default-constructed value if it doesn't 
     * exists (in this case an empty string).
     */
    inline QString getUncommonElement(QString tag) const { return uncommonTags.value(tag); };

    /**
     * \brief Verifies that a specific uncommon tag identifier exists in the uncommon elements 
     * collection.
     * \param tag Uncommon tag identifier.
     * \return True if the uncommon tag exists in the collection or false if not.
     */
    inline bool isSetUncommonElement(QString tag) const { return uncommonTags.contains(tag); };

    /**
     * \brief Returns the total number of uncommon elements.
     * \return Number of uncommon elements.
     */
    inline int getNumberOfUncommonElements() const { return uncommonTags.size(); };

    /**
     * \brief Returns an STL-style iterator pointing to the first item in the uncommon elements 
     * collection.
     */
    QHash<QString,QString>::const_iterator beginUncommonElements() const { return uncommonTags.constBegin(); };

    /**
     * \brief Returns an STL-style iterator pointing to the imaginary item after the last item in 
     * the uncommon elements collection.
     */
    QHash<QString,QString>::const_iterator endUncommonElements() const { return uncommonTags.constEnd(); };

  private:

    QString testName;
    QString testIdentifier;
    QString typeOfTest;
    QString srcAddress;
    QString srcManagementAddress;
    int srcManagementPort;
    QString destAddress;
    QString destManagementAddress;
    int destManagementPort;
    QHash<QString,QString> uncommonTags;
    int version;
};

#endif
