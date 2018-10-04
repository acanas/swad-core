/*
 *  FOTOMATON. Detector de rostros de la plataforma SWAD
 *
 *  Copyright (C) 2018  Daniel J. Calandria Hernández,
 *                      Antonio Cañas Vargas &
 *			Jesús Mesa González.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef util_h
#define util_h

// Algunas funciones utiles

#include "common.h"

#include <string>
#include <vector>
#include <cstdlib>

std::vector<std::string> ReadDir (const char *dir, const char *ext);

void FullPath (const std::string &path, const std::string &name, const std::string &ext, std::string &full);
void PartPath (const std::string &full, std::string *path, std::string *name, std::string *ext);

inline double randu ()
{  return std::rand() / (RAND_MAX + 1.0);
}

inline double randu (double a, double b)
{
  return a + (b - a) * randu();
}

inline int rand (int a, int b)
{
  return a + (b - a) * randu();
}

inline CvMat* ExtractPatch ( const CvMat *img, const CvRect& r, const CvSize& size = cvSize(0,0) )
{
  if (r.x + r.width  > img->width) std::cout << "ERROR\n";
  if (r.y + r.height > img->height) std::cout << "ERROR\n";  
  CvMat orig_patch;
  cvGetSubRect (img, &orig_patch, r);
  
  /*if (size.width == 0 || size.height == 0)
    return orig_patch;*/
        
  CvMat *patch = cvCreateMat (size.width, size.height, img->type);
  cvResize ( &orig_patch, patch );
  
  //cvReleaseMat (&orig_patch);
  return patch;
}


#endif

