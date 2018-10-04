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
#include <vector>
#include <stdio.h>

using namespace std;
using namespace cv;

bool operator < ( const pair<REAL, Rect> &a, const pair<REAL, Rect> &b )
{
  return a.first < b.first;
}


///////////////////////////////////////////////////////////

void stretch_contrast (IplImage *m, int channel, int *h, int max, int f);
void enhance_contrast (IplImage *img,  float t);
void enhance_saturation (IplImage *img, float t);
void enhance_light (IplImage *img, int r, int c, float f);
void contraer (IplImage *mat, int channel, int orig, int dest, int f);
bool check_background (IplImage *img, int r, int c, int t);
void ExtractObjectImage ( IplImage *src, const Rect &r, float ratio, IplImage *dst );

///////////////////////////////////////////////////////////

int main (int argc, char **argv)
{

  char file_name[512];
  
  if (argc < 4)
  {
     cout << "fotomaton <classifier> <input_file> <width>" << endl;
     return 2;      
  }
  
  // Obtener la anchura de la imagen.
  int width = atoi ( argv[3] );
  
  if ( width <= 0)
  {
    cout << "Error: Width must be positive!" << endl;
    return 2;
  }
 
  // Cargar el clasificador en cascada proporcionado como argumento.
  CascadeClassifier cascade;
  string ruta_classifier(argv[1]); // Convertir en string la ruta donde se encuentra el clasificador.

  if (!cascade.load (ruta_classifier)){

    cout << "Error: Classifier not found!" << endl;
    return 2;
  }
      
  // Cargar imagen.
  IplImage *img = cvLoadImage(argv[2]);
  if (!img)
  {
    cout << "Error: Image cannot be read!" << endl;
    return 2;
  }
  
  string str_name;
  PartPath ( argv[2], 0, &str_name, 0 );
        
  for (int i = strlen(argv[2])-1; i>= 0; i--)
    if (argv[2][i] == '.') 
    {
      argv[2][i] = '\0';
      break;
    }
  
  // Detectar objetos.

  Mat gray_img; // Crear una matriz donde almacenar en escala de grises la imagen leída.
  Mat m_img = cvarrToMat(img);
  cvtColor (m_img, gray_img, CV_BGR2GRAY); // Almacenar en la matriz anterior la imagen en escala de grises.
  vector <Rect> objects; // Crear un vector en el cual almacenar los objetos detectados.

  cascade.detectMultiScale(gray_img, objects, 1.09, 1);

  // Si no se han detectado caras (vector de rectángulos vacío) se genera el mapa de la imagen
  // sin enmascarar.
  if (objects.empty())
  {
    IplImage *img_map = cvCreateImage (cvSize(width, (width*img->height)/img->width), 8, 3);  
    cvResize (img, img_map);
    sprintf (file_name, "%s_map.jpg", argv[2]);    
    cvSaveImage (file_name, img_map);    
    return 1;
  }

  // Enmascarar la imagen y generar mapa.    
  if(img->width < width)
    width = img->width;  

  IplImage *img_map = cvCreateImage (cvSize(width, (width*img->height)/img->width), 8, 3);
  cvResize (img, img_map);
  CvMat *mask = cvCreateMat (img_map->height, img_map->width, CV_8UC1);
  cvSet (mask, cvRealScalar(1));
  
  sprintf (file_name, "%s_map.txt", argv[2]);
  ofstream map_file ( file_name );
  
  
  // Extaer cada una de las imágenes y aplicar los diferentes filtros.
  IplImage *img_object = cvCreateImage ( cvSize(150, 200), 8, 3 );       
  for (int i = objects.size()-1; i >= 0; i--)
  {
    int res = 1; // fondo blanco?
    
    Rect r;
    r.x = (objects[i].x * width) / img->width;
    r.y = (objects[i].y * width) / img->width;
    r.width = (objects[i].width  * width) / img->width;
    r.height = (objects[i].height * width) / img->width;                        
    cvCircle ( mask, cvPoint(r.x + r.width/2, r.y + r.height/2), r.width*0.75+1, CV_RGB(0,0,0), -1, CV_AA );    

    // Extraer y mejorar imagen.
    ////////////////////////////////////////////////////////        
    ExtractObjectImage ( img, objects[i], 0.75, img_object );    
    sprintf (file_name, "%s_%03d_paso1.jpg", argv[2], i);
    cvSaveImage (file_name, img_object);    
       
    enhance_contrast (img_object,  0.0009);
    enhance_saturation ( img_object, 0.0001);
    
    if (!check_background( img_object, int(0.07*img_object->height), int(0.1*img_object->width), 150 ) )
    { 
      cvCircle ( img_map, cvPoint(r.x + r.width/2, r.y + r.height/2), r.width*0.75+1, CV_RGB(255,0,0), 2, CV_AA );    
      res = 0; 
    }
    else
    {
      cvCircle ( img_map, cvPoint(r.x + r.width/2, r.y + r.height/2), r.width*0.75+1, CV_RGB(0,255,0), 2, CV_AA );    
      sprintf (file_name, "%s_%03d_paso2.jpg", argv[2], i);
      cvSaveImage (file_name, img_object);    
      enhance_light (img_object, int(0.07*img_object->height),
                    int(0.1*img_object->width), 20.0);
      sprintf (file_name, "%s_%03d_paso3.jpg", argv[2], i);
      cvSaveImage (file_name, img_object);
    }  
    ////////////////////////////////////////////////////////

    sprintf (file_name, "%s_%03d", str_name.c_str(), i);                
    map_file << int(r.x + r.width/2) << " " << int(r.y + r.height/2) << " " << int(r.width*0.75+1) << " " << res << " " << file_name << '\n';
  }  
  map_file.close();       
  cvSubS ( img_map, cvScalar(80,120,120,0), img_map, mask );
  sprintf (file_name, "%s_map.jpg", argv[2]);
  cvSaveImage (file_name, img_map);
  
  
  cvReleaseImage (&img_map);  
  cvReleaseImage (&img_object);  
  cvReleaseImage (&img);  
  
  return 0;  
}


void ExtractObjectImage ( IplImage *src, const Rect &r, float ratio, IplImage *dst )
{
  Rect obj_r;
  
  obj_r = r;
  obj_r.y -= r.height / 2;
  obj_r.height += 1.5*r.height;
  obj_r.x -= r.width / 2;
  obj_r.width += r.width;    
  if (obj_r.y < 0)
    obj_r.y = 0;
  if (obj_r.x < 0)
    obj_r.x = 0;
  if (obj_r.y + obj_r.height >= src->height)
    obj_r.height = src->height - obj_r.y;
  if (obj_r.x + obj_r.width >= src->width)
    obj_r.width = src->width - obj_r.x;

  if (obj_r.width / obj_r.height > ratio)
  {    
    //Añadir filas por abajo, si se puede. Si no, quitar columnas  
    if (obj_r.y + obj_r.width / ratio > src->height)
    {      
      float f = obj_r.height * ratio;      
      obj_r.x = obj_r.x + (obj_r.width - f)/2;
      obj_r.width = f;
    }
    else
      obj_r.height = obj_r.width / ratio;
  }
  else
  {
    //Añadir columnas si se puede    
    float f = obj_r.height * ratio;
    if (obj_r.x + (obj_r.width - f)/2 < 0 ||
        obj_r.x + (obj_r.width + f)/2 > src->width )
      obj_r.height = obj_r.width / ratio;     
    else
    {
      obj_r.x = obj_r.x + (obj_r.width - f)/2;
      obj_r.width = f;
    }
  }

  CvMat mat_aux;
  cvGetSubRect (src, &mat_aux, obj_r);
  cvResize (&mat_aux, dst); 
}



/*************************************************************/
/* FILTROS DE REALCE                                         */
/*************************************************************/

//Mejora de contraste.

void enhance_contrast (IplImage *img,  float t)
{
  int *hist[3];
  for (int i = 0; i < 3; ++i)
  {
    hist[i] = new int[256];
    memset (hist[i], 0, sizeof(int)*256);
  }
  int count = 0;
  
  //Calcular histogramas
  for (int i = 0; i < img->height; ++i)
    for (int j = 0; j < img->width; ++j)      
      for (int k = 0; k < 3; ++k)
      {
        ++hist[k][((uchar*) (img->imageData + i * img->widthStep))[j * img->nChannels + k]];
      }
     
  count = img->height * img->width;

  //Estirar el contraste teniendo en cuenta los pixels de piel.  
  for (int i = 0; i < 3; ++i)
    stretch_contrast (img, i, hist[i], int(t * count), 255 );
    
}
// Mejora de saturacion.
void enhance_saturation (IplImage *img, float t)
{
  cvCvtColor(img,img, CV_RGB2HSV);
  
  int hist[256];
  int count = 0;
    
  // Calcular histogramas.
  for (int i = 0; i < img->height; ++i)
    for (int j = 0; j < img->width; ++j)      
      {     
        ++hist[((uchar*) (img->imageData + i*img->widthStep))[j * img->nChannels + 2]];
        ++count;
      }                           

  count = img->height *img->width;
  
  // Estirar el contraste teniendo en cuenta los pixels de piel  
  stretch_contrast (img, 2, hist, int(t * count), 255 );

  cvCvtColor(img,img, CV_HSV2RGB);
}

// Realce de blancos.
void enhance_light (IplImage *img, int r, int c, float f)
{
  int avg[3] = {0,0,0};
  int nuevo_valor, savg, total = 0;

  // Obtener el color medio de la imagen, tomando las esquinas como referencia
    
  // Esquina izquierda.
  for (int i = 0; i <= r; ++i)  
    for (int j = c - (c * i) / r; j >= 0; --j)      
      { 
        ++total;
        for (int n = 0; n < 3; ++n)           
          avg[n] += ((uchar*)(img->imageData + i * img->widthStep))[j * img->nChannels + n];
      }         
  // Esquina derecha.
  for (int i = 0; i <= r; ++i)
    for (int j = (c * i) / r; j <= c; ++j)      
      {
        ++total;
        for (int n = 0; n < 3; ++n)
          avg[n] += ((uchar*)(img->imageData + i * img->widthStep))[img->width -1 - c + j];          
      }
  
  for (int i = 0; i < 3; ++i)
    avg[i] /= total;
      
  // Obtener el nuevo valor de blanco.
  savg = (avg[0] + avg[1] + avg[2])/3;
  nuevo_valor = int(savg + (255 - savg) / f);
  
  //Desplazar hacia el nuevo blanco
  for (int i = 0; i < 3; ++i)
    contraer (img, i, avg[i], nuevo_valor, 255);
}

// Estira el histograma h asociado a m.
void stretch_contrast (IplImage *m, int channel, int *h, int max, int f)
{
  int /*max = int(t * m.rows() * m.cols()),*/ sum = 0; 
  int inf = 0, sup = 0, w;

 // Limite inferior.
 for (unsigned i = 0; i < 256; ++i)
  {
    sum += h[i];
    
    if (sum > max)
    {     inf = i;      break;    }
  }

  // Limite superior.
  sum = 0;
  for (int i = 256-1; i >= 0; --i)
  {
    sum += h[i];
    
    if (sum > max)
    {     sup = i;      break;    }
  }  
  
  
  // Estirar histograma entre [inf,sup].
  w = sup - inf;
  
  //cout << "inf = " << inf << ", sup = " << sup << ", w = " << w << endl;
  
  for (int i = 0; i < m->height; ++i)
    for (int j = 0; j < m->width; ++j)
    {
      if (((uchar*) (m->imageData + i * m->widthStep))[j * m->nChannels +  channel] < inf)
        ((uchar*) (m->imageData + i * m->widthStep))[j * m->nChannels +  channel]  = 0; 
      else if (((uchar*) (m->imageData + i * m->widthStep))[j * m->nChannels +  channel] > sup)
        ((uchar*) (m->imageData + i * m->widthStep))[j * m->nChannels +  channel] = f;
      else ((uchar*) (m->imageData + i * m->widthStep))[j * m->nChannels +  channel] = int(rint( (((uchar*) (m->imageData + i * m->widthStep))[j * m->nChannels +  channel] - inf) * f /  w));
    }
}

/*
* Reimplementación de la rutina contraer del proyecto de Alvarez y Rodrigo.
*/
void contraer (IplImage *mat, int channel, int orig, int dest, int f)
{
  float k = dest / float(orig);
  
  if (f > orig)
  {
    for (int i = 0; i < mat->height; ++i)
      for (int j = 0; j < mat->width; ++j)
      {
        float val;
        
        if (((uchar*) (mat->imageData + i * mat->widthStep))[j * mat->nChannels +  channel] < orig)
          val = ((uchar*) (mat->imageData + i * mat->widthStep))[j * mat->nChannels +  channel]  * k;
        else
          val = ((((uchar*) (mat->imageData + i * mat->widthStep))[j * mat->nChannels +  channel] - orig) * (f - dest) / float(f - orig)) + dest;
        
        // Se comprueba que este dentro del rango [0,f]
        if (val < 0)
          ((uchar*) (mat->imageData + i * mat->widthStep))[j * mat->nChannels +  channel] = 0;
        else if (val > f)
          ((uchar*) (mat->imageData + i * mat->widthStep))[j * mat->nChannels +  channel] = f;
        else
          ((uchar*) (mat->imageData + i * mat->widthStep))[j * mat->nChannels +  channel] = int(rint(val));
      }   
  }
  
}


bool check_background (IplImage *img, int r, int c, int t)
{
  // Comprueba los dos triangulos de las esquinas superiores.
  int avg[3]; // Medias para rojo, verde y azul.
  bool ok = true;
  int total = 0;
  
  
  // Esquina izquierda.
  avg[0] = avg[1] = avg[2] = 0;
  
  // Calcular media.
  for (int i = 0; i <= r; ++i)  
    for (int j = c - (c * i) / r; j >= 0; --j)      
      { 
        ++total;                           
        avg[0] += ((uchar*) (img->imageData + i * img->widthStep))[j * img->nChannels    ];
        avg[1] += ((uchar*) (img->imageData + i * img->widthStep))[j * img->nChannels + 1];
        avg[2] += ((uchar*) (img->imageData + i * img->widthStep))[j * img->nChannels + 2];       
      }         
  avg[0] /= total;
  avg[1] /= total;
  avg[2] /= total; 
  ok = (avg[0] + avg[1] + avg[2]) > 3*t;        
  if (!ok) //el fondo es oscuro
    return false;   
  
  // Esquina derecha.
  avg[0]= avg[1] = avg[2] = total = 0;
  for (int i = 0; i <= r; ++i)
    for (int j = (c * i) / r; j <= c; ++j)      
      {
        ++total;        
        avg[0] += ((uchar*) (img->imageData + i * img->widthStep))[(img->width - 1 - c + j) * img->nChannels    ];
        avg[1] += ((uchar*) (img->imageData + i * img->widthStep))[(img->width - 1 - c + j) * img->nChannels + 1];
        avg[2] += ((uchar*) (img->imageData + i * img->widthStep))[(img->width - 1 - c + j) * img->nChannels + 2];        
      }

  avg[0] /= total;
  avg[1] /= total;
  avg[2] /= total;   
  ok = (avg[0] + avg[1] + avg[2]) > 3*t;  
  
  return ok;  
}
