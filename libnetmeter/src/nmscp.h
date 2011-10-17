/***************************************************************************
 *   Copyright (C) 2008 by Roberto Barreda <rbarreda@ac.upc.edu>           *
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
#ifndef NMSCP_H
#define NMSCP_H

#include "nmexternallocal.h"

/*! \brief Permit to transfer files to/from remote hosts
 *  \sa NMExternalLocal documentation for more information about NMScp. */
class NMScp : public NMExternal
{
	public:
		/*! \brief Create an instance of NMScp to transfer files.
		 *  \param name identifier.
		 *  \param listener TRUE if you want to subscribe to events, otherwise FALSE (Default = TRUE).
		 *  \param parent NMParent reference. */
		inline NMScp ( const char *name = 0, bool listener = true, NMParent *parent = 0 )
				: NMExternal ( name, listener, parent ), m_scp ( 0 ), m_host (), m_username ( "" ), m_password ( "" ),
				m_port ( 22 ), m_noHostKeyCheck ( false ), m_setQuietMode ( true ),
				m_setCompression ( false ), m_recursivelyCopyDirectory ( false ) {}

		/*! \brief Default class destructor. */
		~NMScp();

		/*! \brief Run the current remote program.
		 *  \param localFile Path to the local file.
		 *  \param remoteFile Remote destination path where we want to put the local file.
		 *  \return On succes, 0 is returned. On  failure, NMExternal::Error code  is returned. */
		NMExternal::Error put ( const NMString & localFile, const NMString & remoteFile );

		/*! \brief Run the current remote program.
		 *  \param remoteFile Remote path of the file we want to copy locally.
		 *  \param localFile Local file destination path.
		 *  \return On succes, 0 is returned. On  failure, NMExternal::Error code  is returned. */
		NMExternal::Error get ( const NMString & remoteFile, const NMString & localFile );

		/*! \brief Set the host to connect.
		 *  \param host Host to connect to.
		 *  \param connectionPort Connection port. (Default = 22).
		 *  \param noHostKeyCheck Activate HostKeyCheck option of ssh.  */
		inline void setHost ( const NMHost & host, int connectionPort = 22, bool noHostKeyCheck = false )
		{ m_host = host; m_port = connectionPort; m_noHostKeyCheck = noHostKeyCheck; }

		/*! \brief Set username for scp host.
		    \param username User name. */
		inline void setUsername ( const NMString & username ) { m_username = username; }

		/*! \brief Set password for scp host.
		    \param password Password. */
		inline void setPassword ( const NMString & password ) { m_password = password; }

		/*! \brief Set username and password for scp host.
		    \param username User name.
		    \param password Password. */
		inline void setUsernameAndPassword ( const NMString & username, const NMString & password ) 
		{ m_username = username; m_password = password; }

		/*! \brief Disables the progress meter as well as warning and diagnostic messages (by default is set ON).
		    \param quiet TRUE to enable quiet mode. */
		inline void setQuietMode ( bool quiet ) { m_setQuietMode = quiet; }

		/*! \brief Enable compression on scp transfer (by default is set OFF).
			   Compression is desirable on modem lines  and  other  slow  connections,
			   but will only slow down things on fast networks.
		    \param compress TRUE to enable compression. */
		inline void setCompression ( bool compress ) { m_setCompression = compress; }

		/*! \brief Recursively copy entire directories (by default is set OFF).
		    \param recursive TRUE if we want to transfer a directory. */
		inline void recursivelyCopyDirectory ( bool recursive ) { m_recursivelyCopyDirectory = recursive; }

		/*! \brief Does Nothing. Use get(...) or put(...) instead. */
		inline NMExternal::Error run() { return NMExternal::NoError; }

	private:
		/*! \brief Execute the scp command.
		 *  \param m_command Copy command depending if we want to put or get a file
		 *  \return On succes, 0 is returned. On  failure, NMExternal::Error code  is returned. */
		NMExternal::Error runScp ( const NMString & m_command );

		/*! \brief Try to login to the scp host server.
		 *  \return On succes, 0 is returned. On  failure, NMExternal::Error code  is returned. 
		 *  \sa NMExternal::Error */
		NMExternal::Error converseScp();

	private:
		NMExternalLocal * m_scp; // El external local will run SCP app.
		NMHost m_host;
		NMString m_username, m_password;
		int m_port;
		bool m_noHostKeyCheck, m_setQuietMode, m_setCompression, m_recursivelyCopyDirectory;

};

#endif
