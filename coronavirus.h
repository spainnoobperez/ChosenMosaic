#ifndef CORONAVIRUS_H
#define CORONAVIRUS_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cstdio>

using namespace cv;

QT_BEGIN_NAMESPACE
namespace Ui { class CoronaVirus; }
QT_END_NAMESPACE

class CoronaVirus : public QMainWindow
{
    Q_OBJECT

public:
    CoronaVirus(QWidget *parent = nullptr);
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    bool validPath();
    static void onMouse(int events, int x, int y, int flag, void* ustg);
    static void createMosaicImage(Mat inputMat, Mat& outputMat, int size);
    static void setMosaic(Mat& inputMat, Rect rect);
    void startMosaic();
    ~CoronaVirus();

private:
    Ui::CoronaVirus *ui;
    QString imgpath;
    inline static Mat inputImage;
    inline static Mat inputImage_mosaic;
    inline static Mat inputImage_clone;
    inline static int losbarrios;
    inline static int mouseStatus; //Record the state of the mouse, 0 means the left mouse button is not pressed or up, 1 means the left mouse button is pressed
};

#endif // CORONAVIRUS_H
