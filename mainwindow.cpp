#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "vtkSmartPointer.h"
#include "vtkNIFTIImageReader.h"
#include "vtkImageData.h"
#include <vtkAutoInit.h>
#include <vtkSmartPointer.h>

#include <QDir>
#include <QFileDialog>
#include<vtkImageResample.h>
#include<qdebug.h>
#include<vtkMetaImageWriter.h>
#include<QMessageBox>
#include<vtkNIFTIImageWriter.h>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_pushButton_clicked()
{
    vtkSmartPointer <vtkNIFTIImageReader> reader = vtkSmartPointer<vtkNIFTIImageReader>::New();
    QString filter;
    filter = "NIFTII image file (*.nii)";
    QDir dir;
    QString fileName = QFileDialog::getOpenFileName( this, QString(tr("Open NIFTII Image")), dir.currentPath() , filter );
    if ( fileName.isEmpty() == true )
        return;
    // 支持带中文路径的读取
    QByteArray ba = fileName.toLocal8Bit();
    const char *fileName_str = ba.data();
    reader->SetFileName(fileName_str);
    reader->Update();
    //        printf("%c\n",fileName_str);


    int imageDims[3];
    reader->GetOutput()->GetDimensions(imageDims);
    printf("before_dims:%d %d %d\n",imageDims[0],imageDims[1],imageDims[2]);
    double spacing[3];
    reader->GetOutput()->GetSpacing(spacing);
    printf("before_spacing:%f %f %f\n",spacing[0],spacing[1],spacing[2]);
    double origin[3];
    reader->GetOutput()->GetOrigin(origin);
    printf("before_origin:%f %f %f\n",origin[0],origin[1],origin[2]);

    printf("_____________________________________________\n");
    vtkSmartPointer<vtkImageResample> imageResample = vtkSmartPointer<vtkImageResample>::New();
    imageResample->SetInputConnection(reader->GetOutputPort());
    imageResample->Update();
    //imageResample->SetOutputDimensionality(3);
    imageResample->SetAxisMagnificationFactor(0, spacing[0]);
    imageResample->SetAxisMagnificationFactor(1, spacing[1]);
    imageResample->SetAxisMagnificationFactor(2, spacing[2]);
    //imageResample->SetInterpolationModeToLinear();
    imageResample->SetInterpolationModeToCubic();//重采样的方式
    imageResample->Update();
    //imageResample->SetOutputSpacing(spaceing2);
    int dim3[3];
    imageResample->GetOutput()->GetDimensions(dim3);
    printf("after_dims:%d %d %d\n",dim3[0],dim3[1],dim3[2]);
    double origin3[3];
    imageResample->GetOutput()->GetOrigin(origin3);
    printf("after_origin:%f %f %f\n",origin3[0],origin3[1],origin3[2]);
    double spacing3[3];
    imageResample->GetOutput()->GetSpacing(spacing3);
    printf("after_spacing:%f %f %f\n",spacing3[0],spacing3[1],spacing3[2]);

    //save
    if (imageResample->GetOutput())
    {
        QDir *temp = new QDir;
        bool exist = temp->exists("/home/wangxw/res");
        if (exist)
        {
        }
        else
        {
            temp->mkdir("/home/wangxw/res");
        }
        vtkSmartPointer<vtkNIFTIImageWriter> envImageWriter = vtkSmartPointer<vtkNIFTIImageWriter>::New();
        envImageWriter->SetInputData(imageResample->GetOutput());
        envImageWriter->SetFileName("/home/wangxw/res/res.nii");
        envImageWriter->Write();
        printf("done\n");
    }
    else
    {
        QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("！！！"), QMessageBox::Ok);
    }

}

