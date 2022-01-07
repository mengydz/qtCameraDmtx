#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <stdint.h>
#include <QDebug>
#include <QTimer>
#include <QFileDialog>
#include <QClipboard>
#include "opencv2/opencv.hpp"
#include "dmtx.h"
#include <QMessageBox>
#include <iostream>
#include <QFile>
#include <QImage>
#include <QTime>
#include <QColor>
#include <QPainter>
#include <assert.h>

using namespace std;

void dataMatrixDecode();//

#ifdef _WIN32
#ifdef QT_NO_DEBUG
#pragma comment(lib, "opencv_core2413.lib")
#pragma comment(lib, "opencv_highgui2413.lib")
#else
#pragma comment(lib, "opencv_core2413d.lib")
#pragma comment(lib, "opencv_highgui2413d.lib")
#endif
#endif

// solution 1: use cv::VideoCapture
class Capture1 {
private:
	cv::VideoCapture cap;
public:
	void open()
	{
		close();
		cap.open(0);
	}
	void close()
	{
		cap.release();
	}
	bool isOpened() const
	{
		return cap.isOpened();
	}
	QImage capture()
	{
		if (isOpened()) {
			cv::Mat frame;
			cap.read(frame);
			if (frame.dims == 2 && frame.channels() == 3) {
				int w = frame.cols;
				int h = frame.rows;
				QImage image(w, h, QImage::Format_RGB32);
				for (int i = 0; i < h; i++) {
					uint8_t const *src = frame.ptr(i);
					QRgb *dst = (QRgb *)image.scanLine(i);
					for (int j = 0; j < w; j++) {
						dst[j] = qRgb(src[2], src[1], src[0]);
						src += 3;
					}
				}
				return image;
			}
		}
		return QImage();
	}
};

// solution 2: use CvCapture
class Capture2 {
private:
	CvCapture *cap = nullptr;
public:
	void open()
	{
		close();
		cap = cvCreateCameraCapture(0);
	}
	void close()
	{
		if (isOpened()) {
			cvReleaseCapture(&cap);
			cap = nullptr;
		}
	}
	bool isOpened() const
	{
		return cap != nullptr;
	}
	QImage capture()
	{
		if (isOpened()) {
			cvQueryFrame(cap);
			IplImage *iplimage = cvQueryFrame(cap);
			int w = iplimage->width;
			int h = iplimage->height;
			QImage image(w, h, QImage::Format_RGB32);
			for (int i = 0; i < h; i++) {
				uint8_t const *src = (uint8_t const *)iplimage->imageData + i * w * 3;
				QRgb *dst = (QRgb *)image.scanLine(i);
				for (int j = 0; j < w; j++) {
					dst[j] = qRgb(src[2], src[1], src[0]);
					src += 3;
				}
			}
			return image;
		}
		return QImage();
	}
};

struct MainWindow::Private {
    Capture1 cap;
//    Capture2 cap;
	QTimer timer;
};

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	pv = new Private();
	ui->setupUi(this);

	pv->cap.open();

	connect(&pv->timer, SIGNAL(timeout()), this, SLOT(doCapture()));
	pv->timer.setInterval(0);
	pv->timer.start();
}

MainWindow::~MainWindow()
{
	pv->cap.close();
	delete pv;
	delete ui;
}

void MainWindow::doCapture()
{
	QImage image = pv->cap.capture();
	int w = image.width();
	int h = image.height();
	if (w > 0 && h > 0) {
		setFixedSize(w, h);
		ui->widget->setFixedSize(w, h);
		ui->widget->setImage(image);
	}
#if 1
    //简化版本
    QTime time_dmtx;
    QImage src = image.copy();
    DmtxMessage *msg;
    DmtxRegion *reg;
    DmtxImage *imgdtx;
    QString outstr;

    time_dmtx.start();//qt开始计时

    qDebug()<<"src.format() "<<src.format();//注意图片格式要对应 dmtxImageCreate的 不然无法解码

    // 增加超时时间。
    DmtxTime beginTime = dmtxTimeNow();	// 根据系统设置情况，获得当前时间
    long timeout_ms = 10;
    DmtxTime stopTime = dmtxTimeAdd(beginTime, timeout_ms);	// 增加xx ms

    //创建dmtxImage，将qt读取到的图片存储到dmtxImage
    imgdtx = dmtxImageCreate(src.bits(),src.width(),src.height(),DmtxPack32bppXRGB);
    //assert(imgdtx != NULL);
    DmtxDecode *dec = dmtxDecodeCreate(imgdtx, 1);//解码
    //assert(dec != NULL);
    reg = dmtxRegionFindNext(dec,&stopTime);//查找下一个，如果超时则认为没有找到
    //assert(reg != NULL);

    if(dmtxTimeExceeded(stopTime))
    {
        qDebug()<<"超时";
    }
    else
    {
        if (reg != NULL)
        {
            msg = dmtxDecodeMatrixRegion(dec, reg, DmtxUndefined);//获取解码信息
            if (msg != NULL)
            {
                cout << msg->output << endl;//输出解码信息
                cout << msg->outputIdx << endl;

                //qt输出解码信息string->QString
                string strout = (char*)msg->output;//解码信息
                outstr=QString::fromStdString(strout);
                qDebug()<<"解码信息:"<<outstr;//解码信息
                qDebug()<<"解码信息xx:"<<msg->output;//解码信息

                //二维码坐标信息
                qDebug()<<"reg->leftLine.locPos.X "<<reg->leftLine.locPos.X;
                qDebug()<<"reg->leftLine.locPos.Y "<<reg->leftLine.locPos.Y;
                qDebug()<<"reg->leftLine.locNeg.X "<<reg->leftLine.locNeg.X;
                qDebug()<<"reg->leftLine.locNeg.Y "<<reg->leftLine.locNeg.Y;
                qDebug()<<"bottomLine.locPos.X "<<reg->bottomLine.locPos.X;
                qDebug()<<"bottomLine.locPos.Y "<<reg->bottomLine.locPos.Y;
                qDebug()<<"bottomLine.locNeg.X "<<reg->bottomLine.locNeg.X;
                qDebug()<<"bottomLine.locNeg.y "<<reg->bottomLine.locNeg.Y;

                dmtxMessageDestroy(&msg);
            }
            else{qDebug()<<"无法检测到2";}

            dmtxRegionDestroy(&reg);
        }
        else{qDebug()<<"无法检测到1";}
    }

    dmtxDecodeDestroy(&dec);
    dmtxImageDestroy(&imgdtx);
//    qDebug()<<time.elapsed()/1000.0<<"s";

#endif
}

void MainWindow::on_action_file_save_as_triggered()
{
	QString path = QFileDialog::getSaveFileName(this, tr("Save as"), QString(), "JPEG files (*.jpg);;PNG files (*.png)");
	if (path.isEmpty()) return;

	QImage image = ui->widget->getImage();
	image.save(path);
}

void MainWindow::on_action_edit_copy_triggered()
{
	QImage image = ui->widget->getImage();
	qApp->clipboard()->setImage(image);
}

