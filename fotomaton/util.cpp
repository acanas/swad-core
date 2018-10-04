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

#include "util.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

std::vector<std::string> ReadDir (const char *dir_name, const char *ext)
{
  std::vector<std::string> names;
  //DIR *dir;
  dirent *dir_ent, **ents;
  char *path;
  std::vector<std::string> exts;

  //Ajustar path correctamente
  path = new char[strlen(dir_name)+2];
  strcpy (path,dir_name);
  if (dir_name[strlen(dir_name)-1] != '/')
    strcat(path, "/");

  //Obtener extensiones
  for (unsigned i = 0; i < strlen(ext); )
  {
    if (ext[i] != ' ')
    {
      exts.push_back(std::string(&ext[i], 3));
      i = i+3;
    }
    else i++;
  }
  
  //dir = opendir (dir_name);
  /*if (!dir)
  {
    std::cerr << "(utilReadDir) Error: no se puede abrir el directorio de entrada" << std::endl;
    return std::vector<std::string>();
  }*/


  int count = scandir(dir_name, &ents, 0, alphasort);
  if (count < 0)
  {
    std::cerr << "(ReadDir) Error: no se puede abrir el directorio de entrada" << std::endl;
    return std::vector<std::string>();  
  }
  
  //while ( (dir_ent = readdir(dir)) )  
  for (int i = 0; i < count; i++)
  {
    dir_ent = ents[i];
    bool res = false;
    for (unsigned i = 0; i < exts.size(); i++)
    {
      if (!strncmp (&dir_ent->d_name[strlen(dir_ent->d_name) - 3], exts[i].c_str(), 3))
      {
        res = true;
        break; 
      }             
    }

    if (res)
     names.push_back (std::string(path)+dir_ent->d_name);
  }

  //closedir(dir);
  delete [] path;
  free(ents);
  return names;
}

void PartPath (const std::string& pfull, std::string* path, std::string* name, std::string* ext)
{
  int file_pos = -1;
  int ext_pos  = -1;

  char *full = new char[strlen(pfull.c_str())+1];
  strcpy (full, pfull.c_str());

 
  // Localizar extension y archivo
  for (int i = strlen(full)-1; i >= 0; i--)
  {
    if (full[i] == '.' && ext_pos < 0)
    {
      ext_pos = i+1;
      full[i] = '\0';
    }
    else if (full[i] == '/' && file_pos < 0)
    {     
      file_pos = i+1;
      full[i] = '\0';      
    }

    if (file_pos > -1 && ext_pos > -1)
      break;
  }
  
  if (path)  
    *path = full;
  if (name)
  {
    if (file_pos >= 0)
      *name = full + file_pos;
    else
      *name = full;  
  }
  if (ext)
  {
    if (ext_pos >= 0)
      *ext =  full + ext_pos;
    else
      *ext = "";    
  }
  delete [] full;
}

void FullPath (const std::string &path, const std::string &name, const std::string &ext, std::string &full)
{
  full = path;
  if (path[path.length()-1] != '/')
    full = full + "/";
    
  full = full + name + "." + ext;  
}

