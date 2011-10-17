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
#include "nmpython.h"

/*! NMPython defines a wrapper to the Python C API.
 *  Based on Vijay Mathew Pandyalakal's pyembed
 *
 *  \author Roberto Barreda
 *  \author Vijay Mathew Pandyalakal
 */

#include <iostream>
#include <sstream>

/*** NMPythonEnv **/
NMPythonEnv::NMPythonEnv ( int argc, char** argv )
{
	Py_Initialize();
	PySys_SetArgv ( argc, argv );
}

NMPythonEnv::~NMPythonEnv()
{
	Py_Finalize();
}


/*** NMPythonException **/
NMPythonException::NMPythonException ( const NMString& m )
{
	message = m;
}

NMPythonException::~NMPythonException() throw() {}

NMString NMPythonException::what() throw()
{
	return message;
}

/*** NMPython **/
NMPython::NMPython ( int argc, char** argv )
{
	env = new NMPythonEnv ( argc, argv );
	module = 0;
}

NMPython::~NMPython()
{
	if ( module )
	{
		Py_DECREF ( module );
	}
	if ( env )
	{
		delete env;
	}
}

void NMPython::runInlineScript ( const NMString & s ) throw ( NMPythonException )
{
	int ret = PyRun_SimpleString ( s.c_str() );
	if ( ret )
		throw NMPythonException ( "There was a error in the script." );
}

void NMPython::runFile ( const NMString& f ) throw ( NMPythonException )
{
	FILE* file = fopen ( f.c_str(), "r" );
	if ( !file )
		throw NMPythonException ( "Failed to open file <" + f + ">" );

	int ret = PyRun_SimpleFile ( file, f.c_str() );
	fclose ( file );
	if ( ret )
		throw NMPythonException ( "There was a error in the script." );
}

void NMPython::loadModule ( const NMString & moduleName ) throw ( NMPythonException )
{
	PyObject* name = PyString_FromString ( moduleName.c_str() );
	module = PyImport_Import ( name );
	Py_DECREF ( name );
	if ( !module )
		throw NMPythonException ( "Failed to load module <" + moduleName + ">" );
}

void NMPython::call ( const NMString& funcName ) throw ( NMPythonException )
{
	NMPyArgMap args;
	PyObject* retVal = makeCall ( funcName, args );
	if ( retVal )
	{
		Py_DECREF ( retVal );
	}
}

void NMPython::call ( const NMString& funcName, const NMPyArgMap& args ) throw ( NMPythonException )
{
	PyObject* retVal = makeCall ( funcName, args );
	if ( retVal )
	{
		Py_DECREF ( retVal );
	}
}

void NMPython::call ( const NMString& funcName, const NMPyArgMap& args, long& ret ) throw ( NMPythonException )
{
	PyObject* retVal = makeCall ( funcName, args );
	if ( retVal )
	{
		if ( !PyInt_Check ( retVal ) )
		{
			Py_DECREF ( retVal );
			throw NMPythonException ( "Not an integer value in return." );
		}
		ret = PyInt_AsLong ( retVal );
		Py_DECREF ( retVal );
	}
	else
		raiseFuncFailedException ( funcName );
}

void NMPython::call ( const NMString& funcName, const NMPyArgMap& args, double& ret ) throw ( NMPythonException )
{
	PyObject* retVal = makeCall ( funcName, args );
	if ( retVal )
	{
		if ( !PyFloat_Check ( retVal ) )
		{
			Py_DECREF ( retVal );
			throw NMPythonException ( "Not a real value in return." );
		}
		ret = PyFloat_AsDouble ( retVal );
		Py_DECREF ( retVal );
	}
	else
		raiseFuncFailedException ( funcName );
}

void NMPython::call ( const NMString& funcName, const NMPyArgMap& args, NMString& ret ) throw ( NMPythonException )
{
	PyObject* retVal = makeCall ( funcName, args );
	if ( retVal )
	{
		if ( !PyString_Check ( retVal ) )
		{
			Py_DECREF ( retVal );
			throw NMPythonException ( "Not a string value in return." );
		}
		ret = PyString_AsString ( retVal );
		Py_DECREF ( retVal );
	}
	else
		raiseFuncFailedException ( funcName );
}

void NMPython::call ( const NMString& funcName, const NMPyArgMap& args, NMPyStringList& ret ) throw ( NMPythonException )
{
	PyObject* retVal = makeCall ( funcName, args );
	if ( retVal )
	{
		if ( PyTuple_Check ( retVal ) )
			makeListFromTuple ( retVal, ret );
		else if ( PyList_Check ( retVal ) )
			makeListFromList ( retVal, ret );
		else
		{
			Py_DECREF ( retVal );
			throw NMPythonException ( "Not a tuple or list in return." );
		}
		Py_DECREF ( retVal );
	}
	else
		raiseFuncFailedException ( funcName );
}

void NMPython::call ( const NMString& funcName, const NMPyArgMap& args, NMPyStringMap& ret ) throw ( NMPythonException )
{
	PyObject* retVal = makeCall ( funcName, args );
	if ( retVal )
	{
		if ( !PyDict_Check ( retVal ) )
		{
			Py_DECREF ( retVal );
			throw NMPythonException ( "Not a dictionary object in return." );
		}
		makeMapFromDict ( retVal, ret );
		Py_DECREF ( retVal );
	}
	else
		raiseFuncFailedException ( funcName );
}

// private functions
PyObject* NMPython::getFunctionObject ( const NMString& funcName ) throw ( NMPythonException )
{
	if ( !module )
		throw NMPythonException ( "No module loaded." );
	PyObject* func = PyObject_GetAttrString ( module, ( char* ) funcName.c_str() );
	if ( !func || !PyCallable_Check ( func ) )
		throw NMPythonException ( "<" + funcName + "> is not a function or is not callable." );

	return func;
}

PyObject* NMPython::createArgs ( const NMPyArgMap& args ) throw ( NMPythonException )
{
	if ( !module )
		throw NMPythonException ( "No module loaded." );
	size_t sz = args.size();
	if ( !sz )
		return 0;
	PyObject* tuple = PyTuple_New ( sz );
	size_t i = 0;
	PyObject* val = 0;
	NMPyArgMap::const_iterator it;
	for ( it = args.begin(); it != args.end(); it++ )
	{
		switch ( it->second )
		{
			case nmpyLong:
			{
				long l = strtol ( it->first.c_str(), 0, 10 );
				val = PyInt_FromLong ( l );
				break;
			}
			case nmpyReal:
			{
				double d = strtod ( it->first.c_str(), 0 );
				val = PyFloat_FromDouble ( d );
				break;
			}
			case nmpyString:
			{
				val = PyString_FromString ( it->first.c_str() );
				break;
			}
			default:
			{
				Py_DECREF ( tuple );
				throw NMPythonException ( "Not a supported type for argument passing." );
			}
		}
		if ( !val )
		{
			Py_DECREF ( tuple );
			throw NMPythonException ( "Failed to convert <" + NMString ( it->second ) + "> to PyObject." );
		}
		PyTuple_SetItem ( tuple, i++, val );
		Py_DECREF ( val );
	}
	return tuple;
}

PyObject* NMPython::makeCall ( const NMString& funcName, const NMPyArgMap& args ) throw ( NMPythonException )
{
	PyObject* func = getFunctionObject ( funcName );
	PyObject* args_tuple = createArgs ( args );
	Py_XINCREF ( args_tuple );
	PyObject* ret = PyObject_CallObject ( func, args_tuple );
	Py_XDECREF ( args_tuple );
	Py_DECREF ( func );
	return ret;
}

// local functions
void NMPython::raiseFuncFailedException ( const NMString& funcName ) throw ( NMPythonException )
{
	throw NMPythonException ( "Call to function <" + funcName + "> failed." );
}

void NMPython::makeListFromTuple ( PyObject* tuple, NMPyStringList& out )
{
	if ( tuple )
	{
		int size = PyTuple_Size ( tuple );
		for ( int i=0; i<size; i++ )
		{
			PyObject* val = PyTuple_GetItem ( tuple, i );
			out.push_back ( makeString ( val ) );
		}
	}
}

void NMPython::makeListFromList ( PyObject* list, NMPyStringList& out )
{
	if ( list )
	{
		int size = PyList_Size ( list );
		for ( int i=0; i<size; i++ )
		{
			PyObject* val = PyList_GetItem ( list, i );
			out.push_back ( makeString ( val ) );
		}
	}
}

void NMPython::makeMapFromDict ( PyObject* dict, NMPyStringMap& out )
{
	if ( dict )
	{
		PyObject* key;
		PyObject* value;
		int pos = 0;
		while ( PyDict_Next ( dict, &pos, &key, &value ) )
			out[makeString ( key ) ] = makeString ( value );
	}
}

NMString NMPython::makeString ( PyObject* val )
{
	if ( val )
	{
		NMString out;
		if ( PyInt_Check ( val ) )
			out = NMString ( PyInt_AsLong ( val ) );
		else if ( PyLong_Check ( val ) )
			out = NMString ( PyLong_AsLong ( val ) );
		else if ( PyFloat_Check ( val ) )
			out = NMString ( PyFloat_AsDouble ( val ) );
		else if ( PyString_Check ( val ) )
			out = "\"" + NMString ( PyString_AsString ( val ) ) +  "\"";
		else if ( PyTuple_Check ( val ) )
		{
			out = "(";
			NMPyStringList ret;
			makeListFromTuple ( val, ret );
			size_t sz = ret.size();
			for ( size_t i=0; i<sz; i++ )
			{
				out += ret[i];
				if ( i != ( sz - 1 ) )
					out += ",";
			}
			out += ")";
		}
		else if ( PyList_Check ( val ) )
		{
			out = "[";
			NMPyStringList ret;
			makeListFromList ( val, ret );
			size_t sz = ret.size();
			for ( size_t i=0; i<sz; i++ )
			{
				out += ret[i];
				if ( i != ( sz - 1 ) )
					out += ",";
			}
			out += "]";
		}
		else if ( PyDict_Check ( val ) )
		{
			out = "{";
			NMPyStringMap ret;
			makeMapFromDict ( val, ret );
			size_t sz = ret.size();
			size_t i = 0;
			NMPyStringMap::const_iterator it;
			for ( it=ret.begin(); it != ret.end(); it++ )
			{
				out += it->first + ":" + it->second;
				if ( i != ( sz - 1 ) )
					out += ",";
				i++;
			}
			out += "}";
		}
		else
			return "";
		return out;
	}
	else
		return "";
}
