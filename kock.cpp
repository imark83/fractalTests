#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex>

#include <iostream>
#include <ios>
#include <iomanip>



int winWidth = 800;
int winHeight = 600;
double CENTERX=0.0;
double CENTERY=0.0;
double XWIDTH=3.0;
double XHEIGHT=(XWIDTH*winHeight)/winWidth;


class Fractal {
public:
  double ox;
  double oy;
  double xwidth;
  double *data;
  unsigned int nPoints;
  unsigned int nIter;

  Fractal () : ox(0.0),oy(0.0),xwidth(0.0),
      data(NULL),nPoints(0),nIter(0) {}
  ~Fractal () {}

  virtual void draw(Display *dis, Window win) = 0;
};

class Kock : public Fractal {
public:
  Kock (int OPnIter = 0, double OPox = 0.0, double OPoy=0.0,
        double OPxwidth = 0.0) {
    nIter = OPnIter;
    ox = OPox;
    oy = OPoy;
    xwidth = OPxwidth;

    
    nPoints = pow(4,nIter)*3;
    data = (double *) malloc (2*nPoints*sizeof(double));
    if (nIter == 0){
      data[0] = -8.660254037844386e-01+ox;
      data[1] = -0.5+oy;
      data[2] = 8.660254037844386e-01+ox;
      data[3] = -0.5+oy;
      data[4] = 0.0+ox;
      data[5] = 1.0+oy;
    } else {
      Kock prev(nIter-1,ox,oy,xwidth);
      for(int i=0; i<prev.nPoints; i=i+1){ // counter for prev fractal
        double a[2] = {prev.data[2*i],prev.data[2*i+1]};
        double b[2] = {prev.data[2*((i+1)%prev.nPoints)],
                        prev.data[2*((i+1)%prev.nPoints)+1]};
        std::cout << a[0]<<","<<a[1]<<"|"<<b[0]<<","<<b[1]<<std::endl;
        data[8*i] = a[0];
        data[8*i+1] = a[1];
        data[8*i+2] = (a[0]+b[0])*3.333333333333333e-01;
        data[8*i+3] = (a[1]+b[1])*3.333333333333333e-01;
        //data[8*i+4] = (b[1]-a[1])*8.660254037844386e-01;
        //data[8*i+5] = (a[0]-b[0])*8.660254037844386e-01;
        data[8*i+6] = (a[0]+b[0])*6.666666666666666e-01;
        data[8*i+7] = (a[1]+b[1])*6.666666666666666e-01;
      }
    }
    return;
  }
  ~Kock () {
    free(data);
  }

  void draw (Display *dis, Window win) {

  }
};

std::ostream & operator<<(std::ostream &out, const Kock &op) {
  for(int i=0;i<op.nPoints;++i)
    out << "["<<op.data[2*i]<<","<<op.data[2*i+1]<<"],"<<std::endl;
  return out;
}

void pixToXY(int px, int py, double &x, double &y) {
  x=CENTERX-0.5*XWIDTH + XWIDTH*(px/(winWidth-1.0));
  y=CENTERY-0.5*XHEIGHT + XWIDTH*(py/(winWidth-1.0));
  return;
}

int main(void) {
  //std::cout << std::fixed << std::setprecision(3);
  // Display *dis;
  // Window win;
  // XEvent e;
  // int s;



  // dis = XOpenDisplay(NULL);
  // if (dis == NULL) {
  //   fprintf(stderr, "Cannot open display\n");
  //   exit(1);
  // }

  // s = DefaultScreen(dis);
  // win = XCreateSimpleWindow(
  //       dis, 
  //       RootWindow(dis, s), 
  //       10, 10, 
  //       800, 
  //       600, 
  //       1,
  //       BlackPixel(dis, s), 
  //       WhitePixel(dis, s));

  // XSelectInput(dis, win, ExposureMask | KeyPressMask | ButtonPressMask);
  // XMapWindow(dis, win);

  // while (1) {
  //   XNextEvent(dis, &e);
  //   if (e.type == Expose) {
        
  //     }
  //   if (e.type == ButtonPress) {
  //     pixToXY(e.xbutton.x,e.xbutton.y,CENTERX,CENTERY);
  //   }
  //   if (e.type == KeyPress){
  //     break;
  //   }
  // }

  // XDestroyWindow(dis,win);
  // XCloseDisplay(dis);



  Kock kock(1);
  std::cout << kock << std::endl;
  return 0;
}