#include "editor.h"

#include "channelselector.h"
#include "globals.h"
#include "imagetools.h"
#include "error.h"

#include "ui_editor.h"

#include <QCheckBox>
#include <QDebug>
#include <QFileDialog>
#include <QGridLayout>
#include <QSlider>
#include <QVBoxLayout>
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

    this->_CurrentMip = 0;
    this->_CurrentFace = 0;
    this->_CurrentFrame = 0;
    this->_CurrentSlice = 0;

    /* Init */
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
    auto File = QFileDialog::getOpenFileName(this, "Open File", QString(), "Valve Texture Format (*.vtf)");
    if (File.isNull())
        return;

    if (_vtfImage != nullptr)
    {
        xvtf::Bitmap::VTFFile::Free(_vtfImage);
    }

    uint err;
    _vtfImage = xvtf::Bitmap::VTFFile::Alloc(File.toUtf8().constData(), false, &err);

    if (err != static_cast<uint>(xvtf::ERRORCODE::NONE))
    {
        xvtf::Bitmap::VTFFile::Free(_vtfImage);
        QString serr = "xVTF Error: " + QString("0x%1").arg(err, 0, 16);
        VTFStudio::Error::Display(serr);
        return;
    }

    _rawData.clear();
    _channels.clear();
    _rawData = VTFStudio::ImageTools::CreateImageFromVTF(_vtfImage, _channels, &err);

    if (err != static_cast<uint>(xvtf::ERRORCODE::NONE))
    {
        xvtf::Bitmap::VTFFile::Free(_vtfImage);
        _rawData.clear();
        QString serr = "xVTF Error: " + QString("0x%1").arg(err, 0, 16);
        VTFStudio::Error::Display(serr);
        return;
    }

    CreateImageFromVecChannels(&err);

    if (err != static_cast<uint>(xvtf::ERRORCODE::NONE))
    {
        xvtf::Bitmap::VTFFile::Free(_vtfImage);
        _rawData.clear();
        QString serr = "xVTF Error: " + QString("0x%1").arg(err, 0, 16);
        VTFStudio::Error::Display(serr);
        return;
    }

    if (_channelSelector != nullptr)
    {
        delete _channelSelector;
        _channelSelector = nullptr;
    }

    delete _SliderLayout;
    _SliderLayout = new QVBoxLayout();
    ui->MainGrid->addLayout(_SliderLayout, 0, 1, Qt::AlignTop);

    _channelSelector = new ChannelSelector(_channels);
    _channelSelector->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    _SliderLayout->addWidget(_channelSelector, 0, Qt::AlignTop);

    connect(_channelSelector, &ChannelSelector::ValueChanged,this, &Editor::ReceiveChannels);

    SetupSliders();
}

void Editor::CreateImageFromVecChannels(uint * const & error)
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

    xvtf::Bitmap::Resolution res;
    unsigned int err;
    _vtfImage->GetResolution(&res, this->_CurrentMip, &err);

    if (err != static_cast<uint>(xvtf::ERRORCODE::NONE))
    {
        delete[] out;
        if (error != nullptr)
            *error = err;

        return;
    }

    delete _image;
    _image = new QImage(out, static_cast<int>(res.Width), static_cast<int>(res.Height), QImage::Format_RGBA8888);

    _imageScene->clear();
    _imageScene->addPixmap(QPixmap::fromImage(*_image));
    _imageScene->setSceneRect(_image->rect());

    ImageViewer->setScene(_imageScene);
}

void Editor::SetupSliders()
{
    for (auto i : _Sliders)
        delete i;

    _Sliders.clear();

    /* MIPS */
    int mipCount = static_cast<int>(_vtfImage->GetMipCount());
    if (mipCount > 1)
    {
        QSlider* mipSlider = new QSlider(Qt::Horizontal);
        mipSlider->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
        mipSlider->setMaximum(mipCount - 1);
        mipSlider->setMinimum(0);
        mipSlider->setTickInterval(1);
        mipSlider->setTickPosition(QSlider::TicksBelow);
        mipSlider->setMaximumWidth(SLIDERS_MAX_WIDTH);
        mipSlider->setPageStep(1);
        QLabel* mipLbl = new QLabel(QString("MIP Level : 0 / %1").arg(mipCount - 1));

        _SliderLayout->addWidget(mipLbl, 0, Qt::AlignTop);
        _SliderLayout->addWidget(mipSlider, 0, Qt::AlignTop);
        _Sliders.push_back(mipLbl);
        _Sliders.push_back(mipSlider);

        connect(mipSlider, &QSlider::valueChanged, [=]()
        {
            Editor::MipChanged(mipSlider->value(), mipLbl);
        });
    }

    /* FACES */
    if (_vtfImage->GetFlags() & static_cast<unsigned int>(xvtf::Bitmap::VTF::ImageFlags::ENVIRONMENTMAP))
    {
        QSlider* faceSlider = new QSlider(Qt::Horizontal);
        faceSlider->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
        faceSlider->setMaximum(5);
        faceSlider->setMinimum(0);
        faceSlider->setTickInterval(1);
        faceSlider->setTickPosition(QSlider::TicksBelow);
        faceSlider->setMaximumWidth(SLIDERS_MAX_WIDTH);
        faceSlider->setPageStep(1);
        QLabel* faceLbl = new QLabel("Face : 1 / 6");

        _SliderLayout->addWidget(faceLbl, 0, Qt::AlignTop);
        _SliderLayout->addWidget(faceSlider, 0, Qt::AlignTop);
        _Sliders.push_back(faceLbl);
        _Sliders.push_back(faceSlider);

        connect(faceSlider, &QSlider::valueChanged, [=]()
        {
            Editor::FaceChanged(faceSlider->value(), faceLbl);
        });
    }

    /* FRAME */
    int frameCount = static_cast<int>(_vtfImage->GetFrameCount());
    if (frameCount > 1)
    {
        QSlider* frameSlider = new QSlider(Qt::Horizontal);
        frameSlider->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
        frameSlider->setMaximum(frameCount - 1);
        frameSlider->setMinimum(0);
        frameSlider->setTickInterval(1);
        frameSlider->setTickPosition(QSlider::TicksBelow);
        frameSlider->setMaximumWidth(SLIDERS_MAX_WIDTH);
        frameSlider->setPageStep(1);
        QLabel* frameLbl = new QLabel(QString("Frame : 0 / %1").arg(frameCount - 1));

        _SliderLayout->addWidget(frameLbl, 0, Qt::AlignTop);
        _SliderLayout->addWidget(frameSlider, 0, Qt::AlignTop);
        _Sliders.push_back(frameLbl);
        _Sliders.push_back(frameSlider);

        connect(frameSlider, &QSlider::valueChanged, [=]()
        {
            Editor::FrameChanged(frameSlider->value(), frameLbl);
        });
    }

    /* SLICE */
    int depthCount = static_cast<int>(_vtfImage->GetDepth());
    if (depthCount > 1)
    {
        QSlider* depthSlider = new QSlider(Qt::Horizontal);
        depthSlider->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
        depthSlider->setMaximum(depthCount - 1);
        depthSlider->setMinimum(0);
        depthSlider->setTickInterval(1);
        depthSlider->setTickPosition(QSlider::TicksBelow);
        depthSlider->setMaximumWidth(SLIDERS_MAX_WIDTH);
        depthSlider->setPageStep(1);
        QLabel* depthLbl = new QLabel(QString("Slice : 0 / %1").arg(depthCount - 1));

        _SliderLayout->addWidget(depthLbl, 0, Qt::AlignTop);
        _SliderLayout->addWidget(depthSlider, 0, Qt::AlignTop);
        _Sliders.push_back(depthLbl);
        _Sliders.push_back(depthSlider);

        connect(depthSlider, &QSlider::valueChanged, [=]()
        {
            Editor::DepthChanged(depthSlider->value(), depthLbl);
        });
    }
}

void Editor::FaceChanged(int value, QWidget* lbl)
{
    _CurrentFace = static_cast<uint>(value);
    static_cast<QLabel*>(lbl)->setText(QString("Face : %1 / 6").arg(value + 1));

    auto err = UpdateImage();
    if (err != 0)
    {
        QString serr = "xVTF Error: " + QString("0x%1").arg(err, 0, 16);
        VTFStudio::Error::Display(serr);
    }
}

void Editor::FrameChanged(int value, QWidget* lbl)
{
    _CurrentFrame = static_cast<uint>(value);
    static_cast<QLabel*>(lbl)->setText(QString("Frame : %1 / %2").arg(value).arg(_vtfImage->GetFrameCount() - 1));

    auto err = UpdateImage();
    if (err != 0)
    {
        QString serr = "xVTF Error: " + QString("0x%1").arg(err, 0, 16);
        VTFStudio::Error::Display(serr);
    }
}

void Editor::DepthChanged(int value, QWidget* lbl)
{
    _CurrentSlice = static_cast<uint>(value);
    static_cast<QLabel*>(lbl)->setText(QString("Slice : %1 / %2").arg(value).arg(_vtfImage->GetDepth() - 1));

    auto err = UpdateImage();
    if (err != 0)
    {
        QString serr = "xVTF Error: " + QString("0x%1").arg(err, 0, 16);
        VTFStudio::Error::Display(serr);
    }
}

void Editor::MipChanged(int value, QWidget* lbl)
{
    _CurrentMip = static_cast<uint>(value);
    static_cast<QLabel*>(lbl)->setText(QString("MIP Level : %1 / %2").arg(value).arg(_vtfImage->GetMipCount() - 1));

    auto err = UpdateImage();
    if (err != 0)
    {
        QString serr = "xVTF Error: " + QString("0x%1").arg(err, 0, 16);
        VTFStudio::Error::Display(serr);
    }
}

uint Editor::UpdateImage()
{
    using namespace xvtf::Bitmap;

    unsigned int err;
    _rawData.clear();
    std::vector<ChannelLink> Fake; // Want to keep the channels the same
    _rawData = VTFStudio::ImageTools::CreateImageFromVTF(_vtfImage, Fake, &err, _CurrentMip, _CurrentFrame, _CurrentFace, _CurrentSlice);

    if (err != 0)
        return err;

    CreateImageFromVecChannels(&err);

    return err;
}

void Editor::ReceiveChannels(const std::vector<ChannelLink> &channels)
{
    this->_channels = channels;
    CreateImageFromVecChannels();
}
