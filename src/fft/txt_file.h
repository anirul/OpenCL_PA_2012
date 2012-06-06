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

#ifndef txt_file_HEADER_DEFINED
#define txt_file_HEADER_DEFINED

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

template <class T = cl_float2>
class txt_file : public std::vector<T> {
   public :

      txt_file& operator=(const std::vector<T>& in) {
         this->clear();
         this->assign(in.begin(), in.end());
         return (*this);
      }

      void import_file(const std::string& name) throw(std::exception) {
         std::ifstream ifs;
         ifs.open(name.c_str());
         if (!ifs.is_open()) 
            throw std::runtime_error("Could not open file : " + name);
         std::string line = "";
         while (std::getline(ifs, line)) {
            if (line.size() == 0) continue;
            std::stringstream ss(line);
            std::string item;
            while (std::getline(ss, item, ' ')) {
               if (item.size() == 0) continue;
               T value;
               value.x = atof(item.c_str());
               value.y = 0.0;
               this->push_back(value);
            }
         } 
         ifs.close();
      }

      void export_file(const std::string& name) throw(std::exception) {
         std::ofstream ofs;
         ofs.open(name.c_str());
         if (!ofs.is_open())
            throw std::runtime_error("Could not open file : " + name);
         ofs << (*this);
         ofs.close();
      }

};

std::ostream& operator<<(std::ostream& os, const txt_file<cl_float2>& in) {
   std::vector<cl_float2>::const_iterator ite;
   for (ite = in.begin(); ite != in.end(); ++ite) {
      os << (*ite).x << std::endl;
   }
   return os;
}

std::ostream& operator<<(std::ostream& os, const txt_file<cl_double2>& in) {
   std::vector<cl_double2>::const_iterator ite;
   for (ite = in.begin(); ite != in.end(); ++ite) {
      os << (*ite).x << std::endl;
   }
   return os;
}

template <class T>
class txt_file<std::complex<T> > : public std::vector<std::complex<T> > {
   public :

      txt_file& operator=(const std::vector<std::complex<T> >& in) {
         this->clear();
         this->assign(in.begin(), in.end());
         return (*this);
      }


      std::ostream& operator<<(std::ostream& os) {
         typename std::vector<std::complex<T> >::const_iterator ite;
         for (ite = this->begin(); ite != this->end(); ++ite) {
            os << (*ite).real() << std::endl;
         }
         return os;
      }


      void import_file(const std::string& name) throw(std::exception) {
         std::ifstream ifs;
         ifs.open(name.c_str());
         if (!ifs.is_open()) 
            throw std::runtime_error("Could not open file : " + name);
         std::string line = "";
         while (std::getline(ifs, line)) {
            if (line.size() == 0) continue;
            std::stringstream ss(line);
            std::string item;
            while (std::getline(ss, item, ' ')) {
               if (item.size() == 0) continue;
               std::complex<T> value;
               value.real() = (T)atof(item.c_str());
               value.imag() = 0.0f;
               this->push_back(value);
            }
         } 
         ifs.close();
      }

      void export_file(const std::string& name) throw(std::exception) {
         std::ofstream ofs;
         ofs.open(name.c_str());
         if (!ofs.is_open())
            throw std::runtime_error("Could not open file : " + name);
         this->operator<<(ofs);
         ofs.close();
      }
};

template <class T>
std::ostream& operator<<(std::ostream& os, const txt_file<std::complex<T> >& in) {
   typename std::vector<std::complex<T> >::const_iterator ite;
   for (ite = in.begin(); ite != in.end(); ++ite) {
      os << (*ite).real() << std::endl;
   }
   return os;
}

#endif

