/*
 * This file is protected by Copyright. Please refer to the COPYRIGHT file
 * distributed with this source distribution.
 *
 * This file is part of REDHAWK Basic Components DataConverter.
 *
 * REDHAWK Basic Components DataConverter is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * REDHAWK Basic Components DataConverter is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//
// Filename: %P%
//     Date: %G% %U%
//  Version: %R%.%L%
//   Author: ...
//
// Description:
//   This file contains the declarations of the internal, working data types.
//
// Note:
//   The modification history of this file is maintained in SVN.
//
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

#ifndef DATATYPES_H
#define DATATYPES_H

#include <valarray>
#include <complex>

//
// Change in processor? Different primitive floating types (32-bit float vs.
// 64-bit double)? Change them here!
//
typedef double                 Real;
typedef std::complex<Real>     Complex;
typedef std::valarray<Real>    RealArray;
typedef std::valarray<Complex> ComplexArray;


#endif
