/***************************************************************************
 *   Copyright (C) 2008 by Roberto Barreda <rbarreda@ac.upc.edu>           *
 *   Copyright (C) 2006 Mindteck				           *
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

/*! NMPython defines a wrapper to the Python C API.
 *  Based on Vijay Mathew Pandyalakal's pyembed
 *
 *  \author Roberto Barreda
 *  \author Vijay Mathew Pandyalakal
 */

#ifndef NMPYTHON_H
#define NMPYTHON_H

#include "nmstring.h"

#include <map>
#include <vector>
#include <exception>

#include <Python.h>

/*! Manages the Python environment.
 * All python api calls must be wrapped in
 * the context of a Python_env object. */
class NMPythonEnv
{
	public:

		NMPythonEnv ( int argc, char** argv );
		~NMPythonEnv();
};

/*! Exception thrown by Python api calls. */
class NMPythonException : public std::exception
{
	public:

		NMPythonException ( const NMString & m );
		virtual ~NMPythonException() throw();

		virtual NMString what() throw();

	private:

		NMString message;
};

enum Type { nmpyLong, nmpyReal, nmpyString };

typedef std::map<NMString, Type> NMPyArgMap;
typedef std::map<NMString, NMString> NMPyStringMap;
typedef std::vector<NMString> NMPyStringList;

/*! Provide wrappers to low-level python calls. */
class NMPython
{
	public:

		NMPython ( int argc, char** argv );
		virtual ~NMPython();

		/*! Executes a python statement or script.
		 * @param s Python script to compile and run. */
		void runInlineScript ( const NMString & s ) throw ( NMPythonException );

		/*! Executes a python script file.
		 * @param f Name of Python script file to compile and run.
		 */
		void runFile ( const NMString & f ) throw ( NMPythonException );

		/*! Loads a python module to the environment.
		 * @param moduleName
		 */
		void loadModule ( const NMString & moduleName ) throw ( NMPythonException );

		/*! Executes a function in the module.
		 * that returns nothing and takes no arguments.
		 * @param funcName
		 */
		void call ( const NMString & funcName ) throw ( NMPythonException );

		/*! Executes a function in the module.
		 * A return value, if any, is ignored.
		 * @param funcName
		 * @param args This should be empty if the function takes no arguments.
		 */
		void call ( const NMString & funcName, const NMPyArgMap & args ) throw ( NMPythonException );

		/*! Executes a function in the module.
		 * The function should return an integer and this is copied to ret.
		 * @param funcName
		 * @param args This should be empty if the function takes no arguments.
		 * @param ret Functions return value, if any is copied here.
		 */
		void call ( const NMString & funcName, const NMPyArgMap & args, long & ret ) throw ( NMPythonException );

		/*! Executes a function in the module.
		 * The function should return a real number and this is copied to ret.
		 * @param funcName
		 * @param args This should be empty if the function takes no arguments.
		 * @param ret Functions return value, if any is copied here.
		 */
		void call ( const NMString & funcName, const NMPyArgMap & args, double & ret ) throw ( NMPythonException );

		/*! Executes a function in the module.
		 * Any return values, despite type, is copied to ret.
		 * @param funcName
		 * @param args This should be empty if the function takes no arguments.
		 * @param ret Functions return value, if any is copied here.
		 */
		void call ( const NMString & funcName, const NMPyArgMap & args, NMString & ret ) throw ( NMPythonException );

		/*! Executes a function in the module.
		 * The function should return a list or a tuple, and this is copied to ret.
		 * @param funcName
		 * @param args This should be empty if the function takes no arguments.
		 * @param ret Functions return value, if any is copied here.
		 */
		void call ( const NMString & funcName, const NMPyArgMap & args, NMPyStringList & ret ) throw ( NMPythonException );

		/*! Executes a function in the module.
		 * The function should return a dictionary, and this is copied to ret.
		 * @param funcName
		 * @param args This should be empty if the function takes no arguments.
		 * @param ret Functions return value, if any is copied here.
		 */
		void call ( const NMString & funcName, const NMPyArgMap & args, NMPyStringMap & ret ) throw ( NMPythonException );

	private:

		/*! Retrieves a function handle from the python module.
		 * @param funcName
		 * @return PyObject*
		 */
		PyObject* getFunctionObject ( const NMString & funcName ) throw ( NMPythonException );

		/*! Converts an PyArgMap  to a tuple of PyObjects
		 * so that they can be passed as arguments to a python function.
		 * @param args PyArgMap  to convert
		 * @return PyObject* args converted to a python tuple.
		 */
		PyObject* createArgs ( const NMPyArgMap & args ) throw ( NMPythonException );

		/*! Makes the actual function call, with the help of the above functions.
		 * @param funcName
		 * @param args
		 * @return PyObject* the value returned by the python function, or null if no value is returned.
		 */
		PyObject* makeCall ( const NMString & funcName, const NMPyArgMap & args ) throw ( NMPythonException );

		/*! Generic function to raise an exception when a python function call fails.*/
		static void raiseFuncFailedException ( const NMString & funcName ) throw ( NMPythonException );

		/*! Functions to convert complex python data structures to their NM C++ counterparts */
		static void makeListFromTuple ( PyObject* tuple, NMPyStringList & out );
		static void makeListFromList ( PyObject* list, NMPyStringList & out );
		static void makeMapFromDict ( PyObject* dict, NMPyStringMap & out );
		static NMString makeString ( PyObject* val );

	private:

		NMPythonEnv* env;
		PyObject* module;
		PyObject* argsTuple;
};

#endif
