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
//   This file contains the R2C class declaration.
//
// Note:
//   The modification history of this file is maintained in SVN.
//
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

#ifndef _R2C_H_
#define _R2C_H_

#include <iterator>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include "fftw3.h"

//
// Debug: write filter taps to file
//
#undef WRITE_TAPS_TO_FILE
#ifdef WRITE_TAPS_TO_FILE
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <time.h>
#endif
#include "Filter.h"

#define MY_FFTW_FLAGS (FFTW_MEASURE)

class R2C : public Filter
{
public:
    //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
    //
    // Description:
    //   R2C's constructor. It computes the coefficients for the quadrature
    //   filters described in article "An Efficient Analytic Signal Generator,"
    //   by C. S. Turner, IEEE Signal Processing Magazine "Tips and Tricks"
    //   column, July 2009.
    //
    // Parameters:
    //   wLo - beginning frequency of filters' passband (-3dB point). It must be
    //         in the range [0,0.5].
    //   wHi - end frequency of filters' passband (-3dB point). It must be in
    //         the range [0,0.5], and greater than wLo.
    //   dw - half the filters' desired transition-region width. It must be in
    //        the range [0,0.5].
    //   n - number of filter taps
    //
    // Return Value:
    //   None.
    //
    //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

    R2C(const Real &wLo = 0.05,
        const Real &wHi = 0.45,
        const Real &dw  = 0.05,
        const size_t &n = 50,
        const size_t &fftSize = 2048) :
        _f(n),
        _h(n),
        _z(n),
        _freqFilter(NULL),
        _input(NULL),
        _fftPlan(NULL)
    {
    	if((dw <= wLo) && (wLo<= 0.5-dw))
        {
            if(wLo<wHi)
            {
                //
                // Time vector
                //
                RealArray t(n);
                for (size_t k = 0; k < n; ++k)
                    t[k] = M_PI*(2.*k + 1. - n);

                //
                // In-phase impulse response
                //
                //RealArray A(t.apply(FilterTap(wLo, wHi, dw)));
                RealArray A(n);
                std::transform(&t[0], &t[n], &A[0], FilterTap(wLo, wHi, dw));

                //
                // Quadrature-phase impulse response (flip "A", left to right)
                //
	            RealArray B(n);
	            std::reverse_copy(&A[0], &A[n], &B[0]);

                //
                // Filter
                //
                std::transform(&A[0], &A[n], &B[0], &_h[0], complexify);
		
                //
                // Create the Frequency Domain Filter
                //
                _h = _h.apply(std::conj<Real>);

                _freqFilter = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex)*fftSize);
                _input = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex)*fftSize);
                _fftPlan = fftwf_plan_dft_1d(fftSize, _input, _freqFilter, FFTW_FORWARD, MY_FFTW_FLAGS);

                for(size_t ii=0;ii<n;++ii){
                	_input[ii][0] = _h[ii].real();
                	_input[ii][1] = _h[ii].imag();
                }
                fftwf_execute_dft(_fftPlan, (fftwf_complex*)&_input[0], _freqFilter);
                for(size_t ii=0;ii<fftSize;++ii){
                	_freqFilter[ii][0] /= fftSize;
                	_freqFilter[ii][1] /= fftSize;
                }


#ifdef WRITE_TAPS_TO_FILE
                char fname[256];
                sprintf(fname, "R2C%lu.%lutaps.txt", time(NULL), _h.size());
                std::ofstream f(fname);
                std::copy(&_h[0], &_h[n], std::ostream_iterator<Complex>(f, "\n"));
                f.close();
#endif


            }
            else
                throw(std::domain_error("wHi must be greater than wLo."));
        }
        else
            throw(std::domain_error("wLo must be in the range: "
                                    "dw <= wLo <= 0.5-dw."));
    }

    //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
    //
    // Description:
    //   R2C's destructor.
    //
    // Parameters:
    //   None.
    //
    // Return Value:
    //   None.
    //
    //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

    virtual ~R2C()
    {
    	if(_freqFilter)
    		fftwf_free(_freqFilter);
    	if(_input)
    		fftwf_free(_input);
    	if(_fftPlan)
   			fftwf_destroy_plan(_fftPlan);
    }

    //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
    //
    // Description:
    //   R2C's main work method.
    //
    // Parameters:
    //   in/inEnd - pointers to the first and (last+1) samples
    //   out - pointer to the output array
    //
    // Return Value:
    //   n - the number of samples processed
    //
    //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

    virtual size_t work(Real *in, Real *inEnd, Complex *out)
    {
        const Complex *h0(&_h[0]);
        size_t n(std::distance(in, inEnd));

        // Start at beginning of history
        for(Real *z(&_z[0]), *zz(&_z[0]); in != inEnd; zz = z = &_z[0])
        {
            // Point to last coefficient
            const Complex *h(&_h[_h.size()-1]);

            Complex sum(*z++ * *h--);
            while(h != h0)
            {
                // Shift history array
                *zz++ = *z;
                // Accumulate
                sum += *z++ * *h--;
            }

            // Add input sample to history, accumulate, and write to output
            *out++ = sum + (*zz = *in++) * *h;
        }
        
        return n;
    }


    void workFreq(fftwf_complex *in, fftwf_complex *inEnd, fftwf_complex *out){
    	//this function will take each bin and multiply it by the filter.... it will also work with the complex conjugate of the negative freqs
    	size_t n(std::distance(in, inEnd)); // n will go from 0 -> fftSize/2 and contain fftSize/2+1 elements
    	//std::cout << "n is " << n << std::endl;
    	for (size_t ii=0, jj=(n-1)*2-1;ii<n; ++ii, --jj){
    		if(ii == 0 || ii == n){
    			out[ii][0]=in[ii][0] * _freqFilter[ii][0]; //should be purely real
    			out[ii][1] = 0; //should be purely real
    		}else{
    			out[ii][0] = in[ii][0] * _freqFilter[ii][0] - in[ii][1] * _freqFilter[ii][1];
    			out[ii][1] = in[ii][0] * _freqFilter[ii][1] + in[ii][1] * _freqFilter[ii][0];
    			out[jj][0] = in[ii][0] * _freqFilter[jj][0] + in[ii][1] * _freqFilter[jj][1];
    			out[jj][1] = in[ii][0] * _freqFilter[jj][1] - in[ii][1] * _freqFilter[jj][0];
    		}
    	}
    }



    //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
    //
    // Description:
    //   Return R2C's filter taps.
    //
    //   (ToDo: make Demod class a templated class to handle complex taps)
    //
    // Parameters:
    //   None
    //
    // Return Value:
    //   filter taps
    //
    //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

    inline ComplexArray get(void) const {return _h;}


private:
    ComplexArray _f;
    ComplexArray _h;    // Filter taps
    RealArray    _z;    // Filter memory
    fftwf_complex* _freqFilter;
    fftwf_complex* _input;
    fftwf_plan _fftPlan;

    //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
    //
    // Description:
    //   This functor class computes the in-phase impulse response.
    //
    //_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

    class FilterTap
    {
    public:
        FilterTap(const Real &wLo, const Real &wHi, const Real &dw) :
            _wLo(wLo), _wHi(wHi), _dw(dw), tol(1.e-20)
        {
        }

        virtual ~FilterTap()
        {
        }

        Real operator()(Real &t)
        {
            Real A(0);

            // Prevent dividing "by zero"
            Real den((4*_dw*_dw*t*t - M_PI*M_PI)*t);
            if (std::abs(den) < tol)
            {
                //
                // Compute the proper coefficient, depending on where it
                // happened (i.e., t).
                //
                if(t < -M_PI_2/_dw + 0.1)                    // At t=-pi/(2a)?
                {
			        Real phi1(M_PI_4*(_dw - 2*_wLo)/_dw);
			        Real phi2(M_PI_4*(_dw - 2*_wHi)/_dw);
                    A = _dw*(sin(phi1) - sin(phi2));
                }
                else if(t >  M_PI_2/_dw - 0.1)               // At t=pi/(2a)?
                {
			        Real phi1(M_PI_4*(_dw + 2*_wHi)/_dw);
			        Real phi2(M_PI_4*(_dw + 2*_wLo)/_dw);
                    A = _dw*(sin(phi1) - sin(phi2));
                }
                else if(std::abs(t) < 0.0001)               // At t=0?
                    A = sqrt(2.)*(_wHi-_wLo);
            }
            else
            {
                Real num(2*M_PI*M_PI*cos(_dw*t));
                A = (sin(_wLo*t + M_PI_4) - sin(_wHi*t + M_PI_4)) * num / den;
            }

            return A;
        }

    private:
        const Real &_wLo, &_wHi, &_dw;
        const Real tol;
    };

    //
    // Utilities
    //
    static Complex complexify(const Real &im, const Real &re) {return Complex(re,im);}
};

#endif // _R2C_H_



#ifdef R2C_UNIT_TEST
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
//
// Description:
//   Unit test
//
// Parameters:
//   argc - number of arguments passed to the function
//   argv - array of pointers to arguments passed to the function
//
// Return Value:
//   retval - Success or Failure
//
//_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/

#include <iostream>
#include <fstream>
#include <algorithm>
#include <arpa/inet.h>
#include "utils.h"
using namespace std;


int main(int argc, char *argv[])
{
    int retval(0);

    if(argc == 2)
    {
        string filename(argv[1]);

        typedef valarray<short> SampleArray;    // 16t input file
        SampleArray s;
        if(readFile(filename, s))
        {
            const Real wLo(0.05), wHi(0.45), dw(0.05);
            const size_t nTaps(50);

            // Work...
            RealArray x(s.size());
#if 1
            copy(&s[0], &s[s.size()], &x[0]);
#else
            transform(&s[0], &s[s.size()], &x[0], ntohs); // BugAlert:
                                                          // assumes short!
#endif
            ComplexArray y(x.size());
            R2C r2c(wLo, wHi, dw, nTaps);
            r2c.work(&x[0], &x[x.size()], &y[0]);

            ComplexArray h(r2c.get());
            ofstream ofile("r2c_taps.txt");
            copy(&h[0], &h[h.size()], ostream_iterator<Complex>(ofile, "\n"));
            ofile.close();

            // Write results to file...
            s.resize(2*y.size());
            Complex *yptr(&y[0]);
            for(SampleArray::value_type *p(&s[0]), *pEnd(&s[s.size()]);
                p!=pEnd; ++yptr)
            {
                *p++ = static_cast<SampleArray::value_type>(yptr->real());
                *p++ = static_cast<SampleArray::value_type>(yptr->imag());
            }
            filename += "_r2c";
            ofile.open(filename.c_str(), ios_base::out|ios_base::binary);
            ofile.write((char *)&s[0], s.size()*sizeof(s[0]));
            ofile.close();
        }
        else
            cerr << "Error opening " << filename << endl;
    }
    else
        cerr << "Usage: " << argv[0] << " <infile>" << endl;

    return retval;
}
#endif
