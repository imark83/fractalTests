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

  virtual void draw(Display *dis, Window win, int solid=0) = 0;
  virtual void incIter() = 0;
  virtual void decIter() = 0;
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
        //std::cout << a[0]<<","<<a[1]<<"|"<<b[0]<<","<<b[1]<<std::endl<<std::endl;
        data[8*i] = a[0];
        data[8*i+1] = a[1];
        data[8*i+2] = a[0]+(b[0]-a[0])*3.333333333333333e-01;
        data[8*i+3] = a[1]+(b[1]-a[1])*3.333333333333333e-01;
        data[8*i+6] = a[0]+(b[0]-a[0])*6.666666666666666e-01;
        data[8*i+7] = a[1]+(b[1]-a[1])*6.666666666666666e-01;
        data[8*i+4] = a[0]+0.5*(b[0]-a[0])+(b[1]-a[1])*2.886751345948129e-01;
        data[8*i+5] = a[1]+0.5*(b[1]-a[1])+(a[0]-b[0])*2.886751345948129e-01;
        
      }
    }
    return;
  }
  ~Kock () {
    free(data);
  }

  void draw (Display *dis, Window win, int solid = 0) {
    XClearWindow(dis,win);
    std::cout << data[0] << "," << data[1] << std::endl;
    int s=XDefaultScreen(dis);
    XSetForeground(dis, XDefaultGC(dis,s), _RGB(0,0,0x8B));


    if(solid) {
      XPoint *a = (XPoint*) malloc((this->nPoints)*sizeof(XPoint));
      for(int i=0;i<nPoints;++i) {
        XYToPix(data[2*i],data[2*i+1],a[i].x,a[i].y);
      }
      XFillPolygon(
        dis,
        win,
        XDefaultGC(dis,s),
        a,
        nPoints,
        Nonconvex,
        CoordModeOrigin);
      free(a);
    } else {
      XPoint *a = (XPoint*) malloc((this->nPoints+1)*sizeof(XPoint));
      for(int i=0;i<nPoints;++i) {
        XYToPix(data[2*i],data[2*i+1],a[i].x,a[i].y);
      }
      a[this->nPoints].x=a[0].x;a[this->nPoints].y=a[0].y;
      XDrawLines(
        dis,
        win,
        XDefaultGC(dis,s),
        a,
        nPoints+1,
        CoordModeOrigin);
      free(a);
    }
  }

  void incIter () {
    double *olddata = data;

    nPoints = nPoints*4;
    std::cout << "npoints = " << nPoints << std::endl;
    nIter++;
    data = (double *) malloc (2*nPoints*sizeof(double));

    for(int i=0; i<nPoints/4; i=i+1){ // counter for prev fractal
      double a[2] = {olddata[2*i],olddata[2*i+1]};
      double b[2] = {olddata[2*((i+1)%(nPoints/4))],
                      olddata[2*((i+1)%(nPoints/4))+1]};
      //std::cout << a[0]<<","<<a[1]<<"|"<<b[0]<<","<<b[1]<<std::endl<<std::endl;
      data[8*i] = a[0];
      data[8*i+1] = a[1];
      data[8*i+2] = a[0]+(b[0]-a[0])*3.333333333333333e-01;
      data[8*i+3] = a[1]+(b[1]-a[1])*3.333333333333333e-01;
      data[8*i+6] = a[0]+(b[0]-a[0])*6.666666666666666e-01;
      data[8*i+7] = a[1]+(b[1]-a[1])*6.666666666666666e-01;
      data[8*i+4] = a[0]+0.5*(b[0]-a[0])+(b[1]-a[1])*2.886751345948129e-01;
      data[8*i+5] = a[1]+0.5*(b[1]-a[1])+(a[0]-b[0])*2.886751345948129e-01;
    }

    free(olddata);
    return;
  }

  void decIter () {
    if (nIter == 0) return;
    double *olddata = data;
    nPoints = nPoints/4;
    std::cout << "npoints = " << nPoints << std::endl;

    nIter--;
    data = (double *) malloc (2*nPoints*sizeof(double));
    for(int i=0; i<nPoints; i=i+1){
      data[2*i] = olddata[8*i];
      data[2*i+1] = olddata[8*i+1];
    }
    free(olddata);
    return;
  }
};



std::ostream & operator<<(std::ostream &out, const Kock &op) {
  for(int i=0;i<op.nPoints;++i)
    out << "["<<op.data[2*i]<<","<<op.data[2*i+1]<<"],"<<std::endl;
  return out;
}

int main(void) {
  std::cout << std::fixed << std::setprecision(3);
  Kock kock;

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
      kock.draw(dis,win,1);
    }
    if (e.type == ButtonPress) {
      if(e.xbutton.button == 4) {
        XWIDTH = XWIDTH/2.0;
        XHEIGHT=(XWIDTH*winHeight)/winWidth;
      } 
      if(e.xbutton.button == 5) {
        XWIDTH = XWIDTH*2.0;
        XHEIGHT=(XWIDTH*winHeight)/winWidth;
      }
      if(e.xbutton.button == 1) {
        pixToXY(e.xbutton.x,e.xbutton.y,CENTERX,CENTERY);
      }
      std::cout << "draw!"<<std::endl;
      kock.draw(dis,win,1);
      XFlush(dis);
    }
    if (e.type == KeyPress){

      std::cout << e.xkey.keycode << std::endl;
      break;
    }
  }

  XDestroyWindow(dis,win);
  XCloseDisplay(dis);




  return 0;
}