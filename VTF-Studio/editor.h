#ifndef EDITOR_H
#define EDITOR_H

#include "xVTF/xVTF.h"

#include "channelselector.h"

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QSlider>
#include <QVBoxLayout>

#include <vector>

namespace Ui {
class Editor;
}

class Editor : public QMainWindow
{
    Q_OBJECT

public:
    explicit Editor(QWidget *parent = nullptr);
    ~Editor() override;

private:
    void OpenImage();
    void CreateImageFromVecChannels(uint * const & error = nullptr);
    void SetupSliders();

    void MipChanged(int value, QWidget* lbl);
    void FaceChanged(int value, QWidget* lbl);
    void FrameChanged(int value, QWidget* lbl);
    void DepthChanged(int value, QWidget* lbl);

    uint UpdateImage();

private:
    void ReceiveChannels(const std::vector<ChannelLink>& channels);

    Ui::Editor *ui;

    uint _CurrentMip;
    uint _CurrentFrame;
    uint _CurrentFace;
    uint _CurrentSlice;

    QVBoxLayout* _SliderLayout;

    std::vector<QWidget*> _Sliders;
    ChannelSelector* _channelSelector;
    std::vector<QColor> _rawData;
    std::vector<ChannelLink> _channels;
    QGraphicsScene* _imageScene;
    QImage* _image;
    xvtf::Bitmap::VTFFile* _vtfImage;
    QGraphicsView* ImageViewer;
};

#endif // EDITOR_H
