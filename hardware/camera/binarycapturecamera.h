#ifndef BINARYCAPTURECAMERA_H
#define BINARYCAPTURECAMERA_H

#include "qcamera.h"
#include "../projector/qprojector.h"

/// A "binary camera", which is capable of capturing
/// a binary frame all by itself.
/// Just pass in a QCamera * and a QProjector * to use
/// for capture and projection, respectively. If
/// there is another BinaryCaptureCamera working,
/// then this one will hook into the same projector
/// pattern queue.
/// One thing to note: while it is possible to stream
/// from a BinaryCaptureCamera, you're not really
/// going to have a whole lot of fun doing it. You'll
/// probably max out around 1 frame every 10 to 30
/// seconds or so. Don't say I didn't warn you...

class BinaryCaptureCamera : public QCamera
{
    Q_OBJECT
public:

    /// Create a new BinaryCaptureCamera.
    explicit BinaryCaptureCamera(QObject *parent = 0);

    /// Set high and low bits simultaneously.
    /// \returns \b true if the bits were set,
    /// \b false if not. A failure might indicate
    /// that \b loBit &gt;= \b hiBit.
    bool setBitRange(uint lo, uint hi);

    /// Set the camera with which to capture the frames.
    void setCapturingCamera(QCamera *cam);
    /// Set the projector with which to project the binary patterns.
    void setProjector(QProjector *proj);

    /// Convert Gray code to binary.
    static int grayToBinary(int num);
    
signals:
    /// We're still working on the full binary,
    /// but in the meantime, here's something pretty
    /// to look at.
    void intermediateFrame(cv::Mat frame);
    
public slots:

    /// Capture a frame and emit when ready.
    /// BinaryCaptureCamera will only emit grayscale
    /// binary-encoded frames.
    /// \returns \b true if the camera will emit the
    /// frame, \b false if the camera will not. Likely
    /// reasons for a false return value: color frame
    /// request, or capture already in progress.
    bool requestFrame(FrameType type);

    /// A particular pattern has been projected.
    void patternProjected(QProjector::Pattern *pattern);

private:
    /// do we have a frame for the selected bit and
    /// inversion?
    bool hasCapturedRawFrame(uint bit, bool inv);

    /// get raw frame for desired bit and inversion
    cv::Mat getRawFrame(uint bit, bool inv);

    /// compile the frames
    cv::Mat compileFrames();

    /// the camera we will capture with
    QCamera *camera;

    /// the projector we will be using for binary patterns
    QProjector *projector;

    /// the lowest bit we will capture
    uint loBit;

    /// the highest bit we will capture
    uint hiBit;

    /// the frame index we're waiting to fill
    uint currentFrameIndex;

    /// the frames we capture, before processing
    std::vector<cv::Mat> frames;

    /// the type of frame we've been tasked with getting
    FrameType type;

    /// is there a capture in progress?
    bool inProgress;

    
};

#endif // BINARYCAPTURECAMERA_H
