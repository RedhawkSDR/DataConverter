/*
 * This file is protected by Copyright. Please refer to the COPYRIGHT file
 * distributed with this source distribution.
 *
 * This file is part of FilterDecimate.
 *
 * FilterDecimate is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * FilterDecimate is distributed in the hope that it will be useful, but WITHOUT
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
//   This file declares the InputWorkOutput abstract base class.
//
// Notes:
//   1. The modification history of this file is maintained in SVN.
//   2. Needs work!
//
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

#ifndef INPUT_WORK_OUTPUT_H_
#define INPUT_WORK_OUTPUT_H_

#include <stdexcept>
#include <complex>


class InputWorkOutput
{
public:
    InputWorkOutput(void)
    {
    }

    virtual ~InputWorkOutput()
    {
    }

#define WORK(Ti, To)\
    virtual size_t work(Ti *in, Ti *inEnd, To *out) {return defaultMethod();}\
    virtual size_t work(To *in, To *inEnd, Ti *out) {return defaultMethod();}\
    virtual size_t work(Ti *in, Ti *inEnd, std::complex<To> *out) {return defaultMethod();}\
    virtual size_t work(std::complex<To> *in, std::complex<To> *inEnd, Ti *out) {return defaultMethod();}

    WORK(short,  char);
    WORK(int,    char);
    WORK(float,  char);
    WORK(double, char);

    WORK(int,    short);
    WORK(float,  short);
    WORK(double, short);

    WORK(float,  int);
    WORK(double, int);

    WORK(double, float);
#undef WORK
#define WORK(T)\
    virtual size_t work(T *in, T *inEnd, T *out) {return defaultMethod();}\
    virtual size_t work(T *in, T *inEnd, std::complex<T> *out) {return defaultMethod();}\
    virtual size_t work(std::complex<T> *in, std::complex<T> *inEnd, T *out) {return defaultMethod();}\
    virtual size_t work(std::complex<T> *in, std::complex<T> *inEnd, std::complex<T> *out) {return defaultMethod();}
    WORK(char);
    WORK(short);
    WORK(int);
    WORK(float);
    WORK(double);
#undef WORK

    // Leftover...
    //template<class Ti, class To>
    //size_t work(Ti in, Ti inEnd, To out) {return defaultMethod();}

private:
    size_t defaultMethod(void)
    {
        throw(std::domain_error("Error: InputWorkOutput::work() not overloaded"));
        return 0;
    }
};

#endif
