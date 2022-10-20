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
  unsigned long long nPoints;
  unsigned int nIter;

  Fractal () : ox(0.0),oy(0.0),
      data(NULL),nPoints(0),nIter(0) {}
  ~Fractal () {}

  virtual void draw(Display *dis, Window win, int solid=0) = 0;
  virtual void incIter() = 0;
  virtual void decIter() = 0;
};

class Dragon : public Fractal {
public:
  unsigned char *pattern;

  Dragon (int OPnIter = 0, double OPox = 0.0, double OPoy=0.0) {
    nIter = OPnIter;
    ox = OPox;
    oy = OPoy;

    
    nPoints = 1<<(nIter+1); //ipow(2,nIter+1);
    pattern = 
        (unsigned char *) malloc (nPoints*sizeof(unsigned char));
    if (nIter == 0){
      pattern[0] = 0;
    } else {
      Dragon prev(nIter-1,ox,oy);
      for(int i=0; i<(1<<nIter); i=i+1){ // counter for prev fractal
        pattern[2*i] = i%2;
        pattern[2*i+1] = prev.pattern[i];
      }
    }
    return;
  }
  ~Dragon () {
    free(data);
  }

  void draw (Display *dis, Window win, int solid = 0) {
    XClearWindow(dis,win);
    std::cout << data[0] << "," << data[1] << std::endl;
    int s=XDefaultScreen(dis);
    XSetForeground(dis, XDefaultGC(dis,s), _RGB(0,0,0x8B));


    if(solid) {
    } else {
      XPoint *a = (XPoint*) malloc((this->nPoints+1)*sizeof(XPoint));
      for(int i=0;i<nPoints;++i) {
        XYToPix(data[2*i],data[2*i+1],a[i].x,a[i].y);
      }
      a[this->nPoints].x=a[0].x;a[this->nPoints].y=a[0].y;
      for(int i=0;i<nPoints;++i)
        XDrawLine(
          dis,
          win,
          XDefaultGC(dis,s),
          a[i].x,a[i].y,
          a[(i+1)%nPoints].x,a[(i+1)%nPoints].y);
      // XDrawLines(
      //   dis,
      //   win,
      //   XDefaultGC(dis,s),
      //   a,
      //   nPoints+1,
      //   CoordModeOrigin);
      free(a);
    }
  }

  void incIter () {
    unsigned char *oldpattern = pattern;

    nPoints = nPoints<<1;
    std::cout << "npoints = " << nPoints << std::endl;
    nIter++;
    pattern = 
        (unsigned char *) malloc (nPoints*sizeof(unsigned char));

    for(int i=0; i<(1<<nIter); i=i+1){ // counter for prev fractal
      pattern[2*i] = i%2;
      pattern[2*i+1] = oldpattern[i];
    }
    free(oldpattern);
    return;
  }

  void decIter () {
    if (nIter == 0) return;
    unsigned char *oldpattern = pattern;
    nPoints = nPoints>>1;
    std::cout << "npoints = " << nPoints << std::endl;

    nIter--;
    pattern = 
        (unsigned char *) malloc (nPoints*sizeof(unsigned char));

    for(int i=0; i<nPoints; i=i+1){
      pattern[i]=oldpattern[2*i+1]
    }
    free(oldpattern);
    return;
  }
};



std::ostream & operator<<(std::ostream &out, const Dragon &op) {
  for(int i=0;i<op.nPoints;++i)
    out << "["<<op.data[2*i]<<","<<op.data[2*i+1]<<"],"<<std::endl;
  return out;
}

int main(void) {
  std::cout << std::fixed << std::setprecision(3);
  Dragon kock;
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
      kock.draw(dis,win,solid);
    }
    if (e.type == ButtonPress) {
      if(e.xbutton.button == 1) {
        pixToXY(e.xbutton.x,e.xbutton.y,CENTERX,CENTERY);
      } 
      if(e.xbutton.button == 4) {
        XWIDTH/=1.2;
        XHEIGHT=(XWIDTH*winHeight)/winWidth;
      } 
      if(e.xbutton.button == 5) {
        XWIDTH*=1.2;
        XHEIGHT=(XWIDTH*winHeight)/winWidth;
      }
      std::cout << "draw!"<<std::endl;
      kock.draw(dis,win,solid);
      XFlush(dis);
    }
    if (e.type == KeyPress){
      std::cout << e.xkey.keycode << std::endl;
      unsigned int key = e.xkey.keycode;
      if(key == 38){
        kock.incIter();
      } else if (key == 52){
        kock.decIter();
      } else {
        break;
      }
      kock.draw(dis,win,solid);
      XFlush(dis);
    }
  }

  XDestroyWindow(dis,win);
  XCloseDisplay(dis);




  return 0;
}