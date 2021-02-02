#include "coronavirus.h"
#include "ui_coronavirus.h"

void dumpErrLogs(cv::Exception &err0) {
    FILE* fp0 = fopen("ErrCrash.log", "w+");
    if (fp0) {
        fprintf(fp0, "%s", err0.msg.c_str());
        fclose(fp0);
    }
}

CoronaVirus::CoronaVirus(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CoronaVirus) {
    losbarrios = 20;
    mouseStatus = 0;
    ui->setupUi(this);
    connect(ui->toolButton, &QAbstractButton::clicked, [ = ]() {
        imgpath = QFileDialog::getOpenFileName(this, "Choose Image File", ".",
        "Picture files (*.png);;All files (*)");
        ui->gnome->setText(imgpath);
    });
    connect(ui->gnome, &QTextEdit::textChanged, [ = ]() {
        imgpath = ui->gnome->toPlainText();
    });
    connect(ui->mosaic, &QAbstractButton::clicked, this, &CoronaVirus::startMosaic);
    connect(ui->guardar, &QAbstractButton::clicked, [ = ]() {
        if (inputImage.empty()) {
            QMessageBox::warning(this, "Warning", "Please read in a picture first!");
        }
        else {
            QString nueva = QFileDialog::getSaveFileName(this, "Save Image File", ".",
                "Picture files (*.png);;All files (*)");
            if (!nueva.isEmpty()) {
                try {
                    std::vector<int> compression_params;
                    compression_params.push_back(IMWRITE_PNG_COMPRESSION);
                    compression_params.push_back(1);
                    imwrite(nueva.toStdString(), inputImage_clone, compression_params);
                } catch (cv::Exception &err0) {
                    dumpErrLogs(err0);
                    QMessageBox::warning(this, "Warning", "Failed to save the picture!");
                }
            }
        }
    });
    this->setAcceptDrops(true);
}

void CoronaVirus::dragEnterEvent(QDragEnterEvent* e) {
    e->setAccepted(e->mimeData()->hasFormat("text/uri-list"));
}

void CoronaVirus::dropEvent(QDropEvent* e) {
    QList<QUrl> urls = e->mimeData()->urls();
    if (urls.isEmpty() || urls.size() > 1) return;
    imgpath = urls.first().toLocalFile();
    ui->gnome->setText(imgpath);
}

void CoronaVirus::startMosaic() {
    if(!validPath()) {
        QMessageBox::critical(this, "Error", "File path is invalid!");
        return;
    }
    try {
        inputImage = imread(imgpath.toStdString());
        if (inputImage.empty()) {
            QMessageBox::critical(this, "Error", "Cannot read file!");
            return;
        }
        inputImage_clone = inputImage.clone();
        createMosaicImage(inputImage, inputImage_mosaic, losbarrios);
        namedWindow("showImage", 0);
        setMouseCallback("showImage", onMouse);
        waitKey();
    }
    catch (cv::Exception& err0) {
        dumpErrLogs(err0);
    }
}

bool CoronaVirus::validPath() {
    if(imgpath.isEmpty()) return false;
    QFile aFile(imgpath);
    if (!aFile.exists()) return false;
    if (!aFile.open(QIODevice::ReadOnly)) {
        aFile.close();
        return false;
    }
    aFile.close();
    return true;
}

void CoronaVirus::createMosaicImage(Mat inputMat, Mat &outputMat, int size) {
    RNG rng;
    int height = inputMat.rows;
    int width = inputMat.cols;
    Mat padding;
    Mat tempMat;
    copyMakeBorder(inputMat, padding, 0, size - inputMat.rows % size, 0, size - inputMat.cols % size, BORDER_REPLICATE);
    tempMat = padding.clone();
    for (int row = 0; row < padding.rows; row += size) {
        for (int col = 0; col < padding.cols; col += size) {
            int rand_x = rng.uniform(0, size);
            int rand_y = rng.uniform(0, size);
            Rect rect = Rect(col, row, size, size);
            Mat roi = tempMat(rect);
            Scalar color = Scalar(padding.at<Vec3b>(row + rand_y, col + rand_x)[0], \
                                  padding.at<Vec3b>(row + rand_y, col + rand_x)[1], \
                                  padding.at<Vec3b>(row + rand_y, col + rand_x)[2]);
            roi.setTo(color);
        }
    }
    outputMat = tempMat(Rect(0, 0, width, height)).clone();
}

void CoronaVirus::setMosaic(Mat &inputMat, Rect rect) {
    Mat roi = inputMat(rect);
    Mat tempRoi = inputImage_mosaic(rect);
    tempRoi.copyTo(roi);
}

void CoronaVirus::onMouse(int events, int x, int y, int flag, void *ustg) {
    if (x < 0 || x > inputImage.cols || y < 0 || y > inputImage.rows) {
        return;
    }
    int x_left, x_right, y_top, y_bottom;
    x - losbarrios <= 0 ? x_left = 0 : x_left = x - losbarrios;
    x + losbarrios > inputImage.cols ? x_right = inputImage.cols : x_right = x + losbarrios;
    y - losbarrios <= 0 ? y_top = 0 : y_top = y - losbarrios;
    y + losbarrios > inputImage.rows ? y_bottom = inputImage.rows : y_bottom = y + losbarrios;
    if (events == EVENT_LBUTTONDOWN) {
        mouseStatus = 1;
        setMosaic(inputImage_clone, Rect(x_left, y_top, x_right - x_left, y_bottom - y_top));
    } else if (events == EVENT_MOUSEMOVE) {
        if (mouseStatus == 1) {
            setMosaic(inputImage_clone, Rect(x_left, y_top, x_right - x_left, y_bottom - y_top));
        } else {
        }
    } else if (events == EVENT_LBUTTONUP) {
        mouseStatus = 0;
    } else {
    }
    imshow("showImage", inputImage_clone);
}

CoronaVirus::~CoronaVirus() {
    delete ui;
}

