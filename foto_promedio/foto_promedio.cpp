/*
 *  Calcula la foto promedio de un conjunto dado
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

#include <opencv/cv.h>
#include <opencv/highgui.h>

using namespace std;

void Add (IplImage *input, IplImage *output);

int main (int argc, char **argv)
{
	ifstream fs;
	char file_name[256];
	IplImage *output, *output8, *input;
	int total = 0;

	//Opciones: <lista> <salida>
	if (argc < 3)
		return -1;
	

	fs.open (argv[1]);
	if (fs.fail())
		return -1;

	
	fs.getline(file_name, 256);

	input = cvLoadImage (file_name);
	output = cvCreateImage (cvGetSize(input), IPL_DEPTH_32S, 3);
	cvZero (output); cvReleaseImage (&input);
	//Recorrer el fichero sumando las imágenes
	while (!fs.eof())
	{
		input = cvLoadImage (file_name);		
		Add (input, output);
		cvReleaseImage (&input);			
		total++;
		fs.getline(file_name, 256);	
	}	

	output8 = cvCreateImage (cvGetSize(output), 8, 3);
	cvConvertScale(output, output8, 1.0/total);

	cvSaveImage (argv[2], output8);
	cvReleaseImage (&output); cvReleaseImage (&output8);

	return 0;

}

void Add (IplImage *input, IplImage *output)
{
	for (int i = 0; i < input->height; i++)
		for (int j = 0; j < input->width; j++)
			for (int k = 0; k < input->nChannels; k++)			
			{
				((int*) ((char*) (output->imageData + i * output->widthStep)) )[j * output->nChannels + k] +=
				((uchar*) (input->imageData + i * input->widthStep))[j * input->nChannels + k];	
			}

}
