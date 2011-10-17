/***************************************************************************
 *   Copyright (C) 2007 by Roberto Barreda <rbarreda@ac.upc.edu>           *
 *   Copyright (C) 2005 by Ernest Nieto <ernest.nieto@gmail.com>           *
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
#include "nmgrandomgenerator.h"

/**
 * Constructor of the class. Initializes the generator with a random seed
 * @return
 */
NMGRandomGenerator::NMGRandomGenerator()
{
	srand(( unsigned ) time( 0 ) );
}

/**
 * Contructor of the class
 * @param s Seed for the random generator
 * @return
 */
NMGRandomGenerator::NMGRandomGenerator( unsigned int s )
{
	srand( s );
}

/**
 * Destructor of the class
 * @return
 */
NMGRandomGenerator::~NMGRandomGenerator()
{}

/**
 * Generates a random number
 * @return A random number
 */
int NMGRandomGenerator::random()
{
	return rand();
}

/**
 * Sets the seed of the random generator
 * @param s New value of the seed
 */
void NMGRandomGenerator::seed( unsigned int s )
{
	srand( s );
}

/**
 * Returns the maximum value of the number that the generagor will produce
 * @return Maximum value
 */
int NMGRandomGenerator::getMaxValue()
{
	return RAND_MAX;
}
