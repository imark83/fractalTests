#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex>

#include <iostream>


typedef std::complex<double> Complex_t;


int winWidth = 800;
int winHeight = 600;
double CENTERX=-0.5;
double CENTERY=0.0;
double XWIDTH=3.0;
double XHEIGHT=(XWIDTH*winHeight)/winWidth;





int newton (double x, double y) {
  Complex_t x1(x,y), x0;
  do {
    x0 = x1;
    x1 = x0 - (x0*x0*x0 - 1.0)/(3.0*x0*x0);
  } while (abs(x1-x0) > 0.1);
  if (x1.real() > 0)
    return 0;
  if (x1.imag() > 0)
    return 1;
  return 2;
}



double mandelbrot (double x, double y) {
  Complex_t c(x,y), z;
  int maxIter=500;
  do {
    z=z*z+c;
  } while (abs(z) < 4  && maxIter-- !=0);
  return (abs(z));
}

void pixToXY(int px, int py, double &x, double &y) {
  x=CENTERX-0.5*XWIDTH + XWIDTH*(px/(winWidth-1.0));
  y=CENTERY-0.5*XHEIGHT + XWIDTH*(py/(winWidth-1.0));
  return;
}

void drawNewton (Display *dis, int s,Window &win, GC gc){

  XColor Red, Green, Blue;
  XAllocNamedColor(dis, DefaultColormap(dis,s), "red", &Red, &Red);
  XAllocNamedColor(dis, DefaultColormap(dis,s), "green", &Green, &Green);
  XAllocNamedColor(dis, DefaultColormap(dis,s), "blue", &Blue, &Blue);
  //const char *msg = "Hello, World!";

  //XFillRectangle(dis, win, DefaultGC(dis, s), 1, 1, 10, 10);
  //XDrawString(dis, win, DefaultGC(dis, s), 10, 50, msg, strlen(msg));
  double x,y;
  for(int px=0;px<winWidth;++px) for(int py=0;py<winHeight;++py) {
    pixToXY(px,py,x,y);
    int c=newton(x,y);
    if(c==0) {
      XSetForeground(dis, gc, Green.pixel);
      XDrawPoint(dis,win,gc,px,py);
    } else if (c==1) {
      XSetForeground(dis, gc, Blue.pixel);
      XDrawPoint(dis,win,gc,px,py);
    } else {
      XSetForeground(dis, gc, Red.pixel);
      XDrawPoint(dis,win,gc,px,py);        
    }
  }

  return;
}


void drawMandelbrot (Display *dis, int s,Window &win, GC gc){

  XColor Red, Green, Blue;
  XAllocNamedColor(dis, DefaultColormap(dis,s), "red", &Red, &Red);
  XAllocNamedColor(dis, DefaultColormap(dis,s), "green", &Green, &Green);
  XAllocNamedColor(dis, DefaultColormap(dis,s), "blue", &Blue, &Blue);
  //const char *msg = "Hello, World!";

  //XFillRectangle(dis, win, DefaultGC(dis, s), 1, 1, 10, 10);
  //XDrawString(dis, win, DefaultGC(dis, s), 10, 50, msg, strlen(msg));
  double x,y;
  for(int px=0;px<winWidth;++px) for(int py=0;py<winHeight;++py) {
    pixToXY(px,py,x,y);
    double c=mandelbrot(x,y);
    unsigned long cc = c*(1<<12);
    if(c>4) {
      //std::cout << c << std::endl;
      XSetForeground(dis, gc, cc);
      XDrawPoint(dis,win,gc,px,py);
    } else {
      XSetForeground(dis, gc, BlackPixel(dis, s));
      XDrawPoint(dis,win,gc,px,py);
    } 
  }

  return;
}


int main(void) {
   Display *dis;
   Window win;
   GC gc;
   XEvent e;
   int s;

   dis = XOpenDisplay(NULL);
   if (dis == NULL) {
      fprintf(stderr, "Cannot open display\n");
      exit(1);
   }

   s = DefaultScreen(dis);
   win = XCreateSimpleWindow(dis, RootWindow(dis, s), 10, 10, 800, 600, 1,
                           BlackPixel(dis, s), WhitePixel(dis, s));
   XSelectInput(dis, win, ExposureMask | KeyPressMask | ButtonPressMask);
   XMapWindow(dis, win);
   gc=XCreateGC(dis, win, 0,0); 

   int once=1;
   while (1) {
      XNextEvent(dis, &e);
      if (once && e.type == Expose) {
        once = 0;
        drawMandelbrot(dis,s,win,gc);
      }
      if (e.type == ButtonPress) {
        pixToXY(e.xbutton.x,e.xbutton.y,CENTERX,CENTERY);
        XWIDTH /= 2.0;
        XHEIGHT /= 2.0;
        drawMandelbrot(dis,s,win,gc);
      }
      if (e.type == KeyPress)
         break;
   }
   //XFreeGC(dis, gc)
   XDestroyWindow(dis,win);
   XCloseDisplay(dis);
   return 0;
}