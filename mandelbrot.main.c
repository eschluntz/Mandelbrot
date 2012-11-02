#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include "graphics.h"

#define THREADS 9
#define sizeX 900
#define sizeY 600


//globals
float zoom = 1;
double Xcenter = -.5;
double Ycenter = 0;

int t = 0;

HANDLE handles[THREADS]; //handles for threads

//////////////////////////////////////////////// functions 

void display(int nt) { 
     //defines
     int n = nt; //saving nt
     
     int x;
     int y;
     int i;
     int in;
     int count;
     
     double  cr;
     double  ci;
     double  zr;
     double  zi;
     double  zrTemp;
     double  ziTemp;
     
     //calculating
     for (y = n*sizeY/THREADS; y <(n+1)*sizeY/THREADS ; y++) {
		  if (n==0) {
			  if ( y%(sizeY/(THREADS*20)) ==0 ) {
			  	  printf (">");
			  }
		  }
          for (x = 0; x < sizeX; x++) {
              //setting C and Z based on pixel
              // C and Z are complex numbers
              // C = cr + ci*i 
              // Z = zr + zi*i
              cr = 3/zoom*x/(1.00*sizeX) - 1.5/zoom + Xcenter;
              ci = 2/zoom*y/(1.00*sizeY) - 1/zoom + Ycenter;
              zr = cr;
              zi = ci;
              
              //determining color based on escape speed
              count = 0;
              while (((zr*zr+zi*zi) < 4) && (count <= 255)) {
                    // z = z^2 + c
                    zrTemp = zr*zr - zi*zi + cr;
                    ziTemp = 2*zr*zi + ci;
                    zr = zrTemp;
                    zi = ziTemp;
                    count ++; 
              }

              pixels[3*(x+y*sizeX) + 0] = count;
              pixels[3*(x+y*sizeX) + 1] = 0;
              pixels[3*(x+y*sizeX) + 2] = 0;
              
         }
     }
}

void onclick(int x, int y) {
     int a;
     int b;
     int i;
     
     clock_t time;
     
     Xcenter = 3/zoom*x/(1.00*sizeX) - 1.5/zoom + Xcenter;
     Ycenter = 1/zoom + Ycenter - 2/zoom*y/(1.00*sizeY);
     zoom *= 4;
     printf ("Zoom: %g \n", zoom);
     printf ("X coord: %g \n", Xcenter);
     printf ("Y coord: %g \n", Ycenter);
     
     time = clock();
     
     //starting threads 
     for (i = 0; i < THREADS; i++) {
			handles[i] = CreateThread(NULL,0,display, i, 0, 0);
	 }
	 
	 //waiting for threads
	 WaitForMultipleObjects(THREADS, handles, TRUE, INFINITE);
	 
	 printf ("\n");
	 printf ("Clocks: %d\n", clock() - time);
	 
     //marking center of the screen
     for (x = sizeX/2 -1; x < sizeX/2 +2; x++) {
         for (y = sizeY/2 -1; y < sizeY/2 +2; y++) {
             pixels[3*(x+y*sizeX) + 0] = 255;
             pixels[3*(x+y*sizeX) + 1] = 0;
             pixels[3*(x+y*sizeX) + 2] = 0;
         }
     }
}

void draw()
{
	if (t == 0) {
		
		//starting threads 
     	for (t = 0; t < THREADS; t++) {
			handles[t] = CreateThread(NULL,0,display, t, 0, 0);
	 	}
	 	//waiting for threads
		 WaitForMultipleObjects(THREADS, handles, TRUE, INFINITE);
	}
}

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow) {
        
    int i;
    
    return spawnWindow(hThisInstance, nCmdShow, sizeX, sizeY, 0, "Mandelbrot set", draw, onclick);
    
    for (i = 0; i < THREADS; i++) {
		CloseHandle(handles[i]);
	}
}
