/*
 * Copyright (c) 2012, Frederic Dubouchet
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the CERN nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Frederic Dubouchet ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Frederic DUBOUCHET BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef fftw_fft_HEADER_DEFINED
#define fftw_fft_HEADER_DEFINED

#include <complex>
#include <boost/date_time/posix_time/posix_time.hpp>
#ifdef WITH_FFTW
#include <fftw3.h>
#endif

using namespace boost::posix_time;

template <class T>
class fftw_fft {
   private :
      std::vector<std::complex<T> > fftw_buffer_in_;
      std::vector<std::complex<T> > fftw_buffer_out_;
      size_t data_size_;
   public :
      void prepare(const std::vector<std::complex<T> >& in);
      boost::posix_time::time_duration run(std::vector<std::complex<T> >& out);
};

template <>
void fftw_fft<float>::prepare(const std::vector<std::complex<float> >& in) {
   fftw_buffer_in_ = in;
   fftw_buffer_out_.resize(in.size());
}

template <>
boost::posix_time::time_duration fftw_fft<float>::run(
      std::vector<std::complex<float> >& out) 
{
   ptime before;
   ptime after;
   fftwf_plan plan;
   before = microsec_clock::universal_time();
   plan = fftwf_plan_dft_1d(
         fftw_buffer_in_.size(),
         (fftwf_complex*)&fftw_buffer_in_[0],
         (fftwf_complex*)&fftw_buffer_out_[0],
         FFTW_FORWARD,
         FFTW_ESTIMATE);
   fftwf_execute(plan);
   after = microsec_clock::universal_time();
   fftwf_destroy_plan(plan);
   out = fftw_buffer_out_;
   return after - before;
}

#endif // fftw_fft_HEADER_DEFINED

