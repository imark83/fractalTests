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




void pixToXY(short px, short py, double &x, double &y) {
  x=CENTERX-0.5*XWIDTH + XWIDTH*(px/(winWidth-1.0));
  y=CENTERY-0.5*XHEIGHT + XWIDTH*(py/(winWidth-1.0));
  return;
}

void XYToPix(double x, double y, short &px, short &py) {
  px=(short) floor((x-CENTERX+0.5*XWIDTH)/XWIDTH*(winWidth-1.0));
  py=(short) floor((y-CENTERY+0.5*XHEIGHT)/XWIDTH*(winWidth-1.0));
  
  return;
}

unsigned long _RGB(unsigned char r,unsigned char g, unsigned char b){
    return b + (g<<8) + (r<<16);
}


unsigned long long ipow(unsigned long long op1, unsigned int op2) {
  if(op2==0) return 1;
  unsigned long long aux = ipow(op1,op2/2);
  if(op2%2==0) return aux*aux;
  return aux*aux*op1;
}

class Fractal {
public:
  double ox;
  double oy;
  double *data;
  unsigned int nPoints;
  unsigned int nIter;

  Fractal () : ox(0.0),oy(0.0),
      data(NULL),nPoints(0),nIter(0) {}
  ~Fractal () {}

  virtual void draw(Display *dis, Window win, int solid=0) = 0;
  virtual void incIter() = 0;
  virtual void decIter() = 0;
};

class Sierpinski : public Fractal {
public:
  double scale;
  Sierpinski (int OPnIter = 0, double OPox = 0.0, double OPoy=0.0) {
    nIter = OPnIter;
    ox = OPox;
    oy = OPoy;
    scale=1.0/(1<<nIter);

    
    nPoints = ipow(3,nIter);
    data = (double *) malloc (2*nPoints*sizeof(double));
    if (nIter == 0){
      data[0] = 0.0+ox;
      data[1] = -1.0+oy;
    } else {
      Sierpinski prev(nIter-1,ox,oy);
      for(int i=0; i<prev.nPoints; i=i+1){ // counter for prev fractal
        double a[2] = {prev.data[2*i],prev.data[2*i+1]};
        //std::cout << a[0]<<","<<a[1]<<"|"<<b[0]<<","<<b[1]<<std::endl<<std::endl;
        data[6*i] = a[0];
        data[6*i+1] = a[1];
        data[6*i+2] = a[0]-0.5*scale;
        data[6*i+3] = a[1]+8.660254037844386e-01*scale;
        data[6*i+4] = a[0]+0.5*scale;
        data[6*i+5] = data[6*i+3];        
      }
    }
    return;
  }
  ~Sierpinski () {
    free(data);
  }

  void draw (Display *dis, Window win, int solid = 0) {
    XClearWindow(dis,win);
    std::cout << data[0] << "," << data[1] << std::endl;
    int s=XDefaultScreen(dis);
    XSetForeground(dis, XDefaultGC(dis,s), _RGB(0,0,0x8B));


    if(solid) {
      XPoint *a = (XPoint*) malloc(3*sizeof(XPoint));
      for(int i=0;i<nPoints;++i) {
        XYToPix(data[2*i],data[2*i+1],a[0].x,a[0].y);
        XYToPix(data[2*i]-0.5*scale,data[2*i+1]+8.660254037844386e-01*scale,a[1].x,a[1].y);
        XYToPix(data[2*i]+0.5*scale,data[2*i+1]+8.660254037844386e-01*scale,a[2].x,a[2].y);

        XFillPolygon(
          dis,
          win,
          XDefaultGC(dis,s),
          a,
          3,
          Nonconvex,
          CoordModeOrigin);
      }
      free(a);
    } else {
      XPoint *a = (XPoint*) malloc(4*sizeof(XPoint));
      for(int i=0;i<nPoints;++i) {
        XYToPix(data[2*i],data[2*i+1],a[0].x,a[0].y);
        XYToPix(data[2*i]-0.5*scale,data[2*i+1]+8.660254037844386e-01*scale,a[1].x,a[1].y);
        XYToPix(data[2*i]+0.5*scale,data[2*i+1]+8.660254037844386e-01*scale,a[2].x,a[2].y);
        XYToPix(data[2*i],data[2*i+1],a[3].x,a[3].y);

        XDrawLines(
          dis,
          win,
          XDefaultGC(dis,s),
          a,
          4,
          CoordModeOrigin);
      }
      free(a);
    }
  }

  void incIter () {
    double *olddata = data;

    nPoints = nPoints*3;
    std::cout << "npoints = " << nPoints << std::endl;
    nIter++;
    data = (double *) malloc (2*nPoints*sizeof(double));
    scale/=2.0;

    
    for(int i=0; i<nPoints/3; i=i+1){ // counter for prev fractal
      double a[2] = {olddata[2*i],olddata[2*i+1]};
      //std::cout << a[0]<<","<<a[1]<<"|"<<b[0]<<","<<b[1]<<std::endl<<std::endl;
      data[6*i] = a[0];
      data[6*i+1] = a[1];
      data[6*i+2] = a[0]-0.5*scale;
      data[6*i+3] = a[1]+8.660254037844386e-01*scale;
      data[6*i+4] = a[0]+0.5*scale;
      data[6*i+5] = data[6*i+3];        
    }

    free(olddata);
    return;
  }

  void decIter () {
    if (nIter == 0) return;
    double *olddata = data;
    nPoints = nPoints/3;
    std::cout << "npoints = " << nPoints << std::endl;

    nIter--;
    data = (double *) malloc (2*nPoints*sizeof(double));
    scale*=2.0;

    for(int i=0; i<nPoints; i=i+1){
      data[2*i] = olddata[6*i];
      data[2*i+1] = olddata[6*i+1];
    }
    free(olddata);
    return;
  }
};



std::ostream & operator<<(std::ostream &out, const Sierpinski &op) {
  for(int i=0;i<op.nPoints;++i)
    out << "["<<op.data[2*i]<<","<<op.data[2*i+1]<<"],"<<std::endl;
  return out;
}

int main(void) {
  std::cout << std::fixed << std::setprecision(3);
  Sierpinski sierpinski;
  int solid=0;

  Display *dis;
  Window win;
  XEvent e;
  int s;

  dis = XOpenDisplay(NULL);
  if (dis == NULL) {
    fprintf(stderr, "Cannot open display\n");
    exit(1);
  }

  s = DefaultScreen(dis);
  win = XCreateSimpleWindow(
        dis, 
        RootWindow(dis, s), 
        10, 10, 
        800, 
        600, 
        1,
        BlackPixel(dis, s), 
        WhitePixel(dis, s));

  XSelectInput(dis, win, ExposureMask | KeyPressMask | ButtonPressMask);
  XMapWindow(dis, win);

  while (1) {
    XNextEvent(dis, &e);
    if (e.type == Expose) {
      sierpinski.draw(dis,win,solid);
    }
    if (e.type == ButtonPress) {
      if(e.xbutton.button == 1) {
        pixToXY(e.xbutton.x,e.xbutton.y,CENTERX,CENTERY);
      } 
      if(e.xbutton.button == 4) {
        XWIDTH/=1.2;
        XHEIGHT=(XWIDTH*winHeight)/winWidth;
        std::cout << "Width = " << XWIDTH << std::endl;
      } 
      if(e.xbutton.button == 5) {
        XWIDTH*=1.2;
        XHEIGHT=(XWIDTH*winHeight)/winWidth;
        std::cout << "Width = " << XWIDTH << std::endl;
      }
      std::cout << "draw!"<<std::endl;
      sierpinski.draw(dis,win,solid);
      XFlush(dis);
    }
    if (e.type == KeyPress){
      std::cout << e.xkey.keycode << std::endl;
      unsigned int key = e.xkey.keycode;
      if(key == 38){
        sierpinski.incIter();
      } else if (key == 52){
        sierpinski.decIter();
      } else {
        break;
      }
      sierpinski.draw(dis,win,solid);
      XFlush(dis);
    }
  }

  XDestroyWindow(dis,win);
  XCloseDisplay(dis);




  return 0;
}