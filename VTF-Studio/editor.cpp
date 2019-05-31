#include "editor.h"

#include "channelselector.h"
#include "imagetools.h"
#include "error.h"

#include "ui_editor.h"

#include <QCheckBox>
#include <QDebug>
#include <QFileDialog>
#include <QGridLayout>
#include <QWidgetAction>

Editor::Editor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Editor)
{
    /* Setup */
    ui->setupUi(this);
    this->_imageScene = new QGraphicsScene(this);
    this->ImageViewer = new QGraphicsView();

    /* Sanity */
    _image = nullptr;
    _vtfImage = nullptr;
    _channelSelector = nullptr;

    /* Test */
    ui->MainGrid->addWidget(ImageViewer);

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

    // TODO: Currently exiting the open dialog returns a null character file which causes an erorr.
    unsigned int err;
    _vtfImage = xvtf::Bitmap::VTFFile::Alloc(File.toUtf8().constData(), false, &err);

    if (err != static_cast<unsigned int>(xvtf::ERRORCODE::NONE))
    {
        xvtf::Bitmap::VTFFile::Free(_vtfImage);
        QString serr = "xVTF Error: " + QString("0x%1").arg(err, 0, 16);
        VTFStudio::Error::Display(serr);
        return;
    }

    _rawData = VTFStudio::ImageTools::CreateImageFromVTF(_vtfImage, _channels, &err);

    CreateImageFromVecChannels();

    delete _channelSelector;
    _channelSelector = new ChannelSelector(_channels);
    _channelSelector->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
    ui->MainGrid->addWidget(_channelSelector, 0, 1, Qt::AlignTop);

    connect(_channelSelector, &ChannelSelector::ValueChanged,this, &Editor::ReceiveChannels);
}

void Editor::CreateImageFromVecChannels()
{
    uchar* out = new uchar[_rawData.size() * 4];
    for (uint i = 0; i < _rawData.size(); ++i)
    {
        // Default Values
        out[i * 4] = 0;
        out[i * 4 + 1] = 0;
        out[i * 4 + 2] = 0;
        out[i * 4 + 3] = 0xFF;

        int r, g, b, a;
        _rawData[i].getRgb(&r, &g, &b, &a);

        for (uint j = 0; j < _channels.size(); ++j)
        {
            for (uint k = 0; k < _channels[j].LinkedTo.size(); ++k)
            {
                if (_channels[j].LinkedTo[k] == Channel::R)
                {
                    if (j == 0)
                        out[i * 4] = static_cast<uchar>(r);
                    else if (j == 1)
                        out[i * 4] = static_cast<uchar>(g);
                    else if (j == 2)
                        out[i * 4] = static_cast<uchar>(b);
                    else
                        out[i * 4] = static_cast<uchar>(a);
                }
                else if (_channels[j].LinkedTo[k] == Channel::G)
                {
                    if (j == 0)
                        out[i * 4 + 1] = static_cast<uchar>(r);
                    else if (j == 1)
                        out[i * 4 + 1] = static_cast<uchar>(g);
                    else if (j == 2)
                        out[i * 4 + 1] = static_cast<uchar>(b);
                    else
                        out[i * 4 + 1] = static_cast<uchar>(a);
                }
                else if (_channels[j].LinkedTo[k] == Channel::B)
                {
                    if (j == 0)
                        out[i * 4 + 2] = static_cast<uchar>(r);
                    else if (j == 1)
                        out[i * 4 + 2] = static_cast<uchar>(g);
                    else if (j == 2)
                        out[i * 4 + 2] = static_cast<uchar>(b);
                    else
                        out[i * 4 + 2] = static_cast<uchar>(a);
                }
                else if (_channels[j].LinkedTo[k] == Channel::A)
                {
                    if (j == 0)
                        out[i * 4 + 3] = static_cast<uchar>(r);
                    else if (j == 1)
                        out[i * 4 + 3] = static_cast<uchar>(g);
                    else if (j == 2)
                        out[i * 4 + 3] = static_cast<uchar>(b);
                    else
                        out[i * 4 + 3] = static_cast<uchar>(a);
                }
            }
        }
    }

    // TODO: error checking + make this modular for different mips/faces/etc
    xvtf::Bitmap::Resolution res;
    _vtfImage->GetResolution(&res);

    delete _image;
    _image = new QImage(out, static_cast<int>(res.Width), static_cast<int>(res.Height), QImage::Format_RGBA8888);

    _imageScene->clear();
    _imageScene->addPixmap(QPixmap::fromImage(*_image));
    _imageScene->setSceneRect(_image->rect());

    ImageViewer->setScene(_imageScene);
}

void Editor::ReceiveChannels(const std::vector<ChannelLink> &channels)
{
    this->_channels = channels;
    CreateImageFromVecChannels();
}
