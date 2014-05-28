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
//
// Description:
//   This file declares the Filter base class.
//
// Notes:
//   1. The modification history of this file is maintained in SVN.
//
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

#ifndef _FILTER_H_
#define _FILTER_H_

#include "DataTypes.h"
#include "InputWorkOutput.h"


class Filter : public InputWorkOutput
{
public:
    Filter(void)
    {
    }

    virtual ~Filter()
    {
    }

    inline size_t size(void) const {return _h.size();}
    inline RealArray get(void) const {return _h;}

protected:
    RealArray _h;               // filter coefficients
};

#endif
