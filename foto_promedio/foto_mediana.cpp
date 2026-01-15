/*
 *  Calcula la foto mediana de un conjunto dado
 *
 *  Copyright (C) 2010-1012  Daniel J. Calandria Hernández &
 *                           Antonio Cañas Vargas
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


#include <fstream>
#include <iostream>
#include <cstdlib>

#include <opencv/cv.h>
#include <opencv/highgui.h>

using namespace std;


void AddHist (IplImage *input, int **hist);
void Mediana (IplImage *output, int **hist);


int main (int argc, char **argv)
{
	ifstream fs;
	char file_name[256];
	IplImage *output, *input;
	int total = 0;
	int **hist;

	//Opciones: <lista> <salida>
	if (argc < 3)
		return -1;
	

	fs.open (argv[1]);
	if (fs.fail())
		return -1;
	
	fs.getline(file_name, 256);

	input = cvLoadImage (file_name);
	output = cvCreateImage (cvGetSize(input), 8, 3);

	//Crear histogramas
	hist = new int*[input->width * input->height * input->nChannels];
	for (int i = 0; i < input->height; i++)
		for (int j = 0; j <input->width; j++)
			for (int k = 0; k < input->nChannels; k++)
			{
				hist[i * input->width * input->nChannels + j * input->nChannels + k] = new int[256];
				memset (hist[i * input->width * input->nChannels + j * input->nChannels + k], 0, sizeof(int)*256);
			}

	cvZero (output);
  cvReleaseImage (&input);
	//Sumar histogramas
	while (!fs.eof())
	{
		input = cvLoadImage (file_name);		
		AddHist (input, hist);
		cvReleaseImage (&input);			
		total++;
		fs.getline(file_name, 256);	
	}	

	//Obtener mediana
	Mediana (output, hist);

	cvSaveImage (argv[2], output);

	for (int i = 0; i < output->height; i++)
		for (int j = 0; j <output->width; j++)
			for (int k = 0; k < output->nChannels; k++)
				delete [] hist[i * output->width * output->nChannels + j * output->nChannels + k];
	delete [] hist;

	cvReleaseImage (&output);

	return 0;

}

void AddHist (IplImage *input, int **hist)
{
	for (int i = 0; i < input->height; i++)
		for (int j = 0; j < input->width; j++)
			for (int k = 0; k < input->nChannels; k++)						
				hist[i * input->width * input->nChannels  + j * input->nChannels + k][
				((uchar*) (input->imageData + i * input->widthStep))[j * input->nChannels + k]]++;	
}

void Mediana (IplImage *output, int **hist)
{
	int sum = 0, val = 0;
	//Calcular sumas
	for (int i = 0; i < output->height; i++)
		for (int j = 0; j < output->width; j++)		
			for (int k = 0; k < output->nChannels; k++)	
			{

				sum = 0;
				for (int p = 0; p < 256; p++)						
					sum += hist[i * output->width * output->nChannels + j * output->nChannels + k][p];

				sum /= 2;
				val = 0;
				for (int p = 0; p < 256; p++)
				{
					val += hist[i * output->width * output->nChannels  + j * output->nChannels + k][p];
					if (val > sum)
					{	
						((uchar*) (output->imageData + i * output->widthStep))[j * output->nChannels + k] = p;	
						break;
					}
				}
			}																	
}
