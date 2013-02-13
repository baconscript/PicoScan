#include "projectionscreen.h"

#include <QApplication>
#include <QDesktopWidget>

ProjectionScreen::ProjectionScreen(QWidget *parent) :
    ImageViewWidget(parent)
{
    setWindowTitle("Projection screen");
    setCursor(QCursor(Qt::BlankCursor));

    QPalette pal(palette());
    pal.setColor(QPalette::Background, Qt::black);
    setAutoFillBackground(true);
    setPalette(pal);

    setContentsMargins(0,0,0,0);
}

/*
        The purpose of this function is to convert an unsigned
        binary number to reflected binary Gray code.

        The operator >> is shift right. The operator ^ is exclusive or.
*/
unsigned int ProjectionScreen::binaryToGray(unsigned int num)
{
        return (num >> 1) ^ num;
}

/*
        The purpose of this function is to convert a reflected binary
        Gray code number to a binary number.
*/
unsigned int ProjectionScreen::grayToBinary(unsigned int num)
{
    unsigned int numBits = 8 * sizeof(num);
    unsigned int shift;
    for (shift = 1; shift < numBits; shift = 2 * shift)
    {
        num = num ^ (num >> shift);
    }
    return num;
}

QImage createWhite(unsigned int width,
                  unsigned int height,
                   unsigned int maxBright){
    QImage out(width,height,QImage::Format_ARGB32);
    unsigned int x,y;

    for (y = 0; y < height; y++) {
            QRgb *destrow = (QRgb*)out.scanLine(y);
            for (x = 0; x < width; ++x) {
                destrow[x] = qRgba(maxBright,maxBright,maxBright,255);
            }
    }

    return out;
}

QImage createGray(unsigned int width,
                  unsigned int height,
                  unsigned int n,
                  bool invert,
                  unsigned int maxBright){
    QImage out(width,height,QImage::Format_ARGB32);
    unsigned int values[width];
    unsigned int x,y,gray;
    unsigned int mask = 1<<n;

    for (x = 0; x < width; x++) {
        gray = ProjectionScreen::binaryToGray(x);
        if(!invert)
            values[x] = (gray & mask) ? 0:maxBright;
        else
            values[x] = (gray & mask) ? maxBright:0;
    }

    for (y = 0; y < height; y++) {
            QRgb *destrow = (QRgb*)out.scanLine(y);
            for (x = 0; x < width; ++x) {
                destrow[x] = qRgba(values[x],values[x],values[x],255);
            }
    }

    return out;
}

QImage createSinusoid(unsigned int width,
                  unsigned int height,
                  int period, double shift,
                  unsigned int maxBright){
    QImage out(width,height,QImage::Format_ARGB32);
    unsigned int values[width];
    unsigned int x,y;
    double d;

    for (x = 0; x < width; x++) {
        d = (1+cos((x-shift)*M_PI*2/period))*maxBright*0.5;
        values[x] = (unsigned int) d;
    }

    for (y = 0; y < height; y++) {
            QRgb *destrow = (QRgb*)out.scanLine(y);
            for (x = 0; x < width; ++x) {
                destrow[x] = qRgba(values[x],values[x],values[x],255);
            }
    }

    return out;
}


void ProjectionScreen::projectOnDisplay(int n)
{
    if(n>=getDisplayCount()) return;
    QRect screenres = QApplication::desktop()->screenGeometry(n);
    move(QPoint(screenres.x(),screenres.y()));
    resize(screenres.width(),screenres.height());
    showFullScreen();
}

int ProjectionScreen::getDisplayCount()
{
    return QApplication::desktop()->screenCount();
}

void ProjectionScreen::projectBinary(int bit, bool inverted, int maxBright)
{
    QImage im = createGray(640,480,bit,inverted,maxBright);
    displayImage(im);
}

void ProjectionScreen::projectSinusoid(int period, double shift, int maxBright)
{
    QImage im = createSinusoid(640,480,period,shift,maxBright);
    displayImage(im);
}

void ProjectionScreen::projectWhite()
{
    QImage im = createWhite(640,480,150);
    displayImage(im);
}


