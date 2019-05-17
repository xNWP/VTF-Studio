#include "editor.h"

#include "imagetools.h"
#include "error.h"

#include "ui_editor.h"
#include "xVTF/xVTFError.h"

#include <QDebug>
#include <QFileDialog>

Editor::Editor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Editor)
{
    ui->setupUi(this);
    this->_imageScene = new QGraphicsScene(this);

    /* Event Handlers */
    connect(ui->actionOpen, &QAction::triggered, this, &Editor::OpenImage);
}

Editor::~Editor()
{
    delete ui;
}

void Editor::OpenImage()
{
    auto File = QFileDialog::getOpenFileName(this);

    if (_vtfImage != nullptr)
    {
        xvtf::Bitmap::VTFFile::Free(_vtfImage);
    }

    unsigned int err;
    _vtfImage = xvtf::Bitmap::VTFFile::Alloc(File.toUtf8().constData(), false, &err);

    if (err != static_cast<unsigned int>(xvtf::ERRORCODE::NONE))
    {
        xvtf::Bitmap::VTFFile::Free(_vtfImage);
        QString serr = "xVTF Error: " + QString("0x%1").arg(err, 0, 16);
        VTFStudio::Error::Display(serr);
        return;
    }

    _image = VTFStudio::ImageTools::CreateImageFromVTF(_vtfImage);

    if (_image == nullptr)
    {
        QString serr = "xVTF Error: " + QString("0x%1").arg(err, 0, 16);
        VTFStudio::Error::Display(serr);
        return;
    }

    _imageScene->addPixmap(QPixmap::fromImage(*_image));
    _imageScene->setSceneRect(_image->rect());

    ui->ImageViewer->setScene(_imageScene);
}
