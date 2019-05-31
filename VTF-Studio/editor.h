#ifndef EDITOR_H
#define EDITOR_H

#include "xVTF/xVTF.h"

#include "channelselector.h"

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>

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
    void CreateImageFromVecChannels();

private:
    void ReceiveChannels(const std::vector<ChannelLink>& channels);

    Ui::Editor *ui;

    ChannelSelector* _channelSelector;
    std::vector<QColor> _rawData;
    std::vector<ChannelLink> _channels;
    QGraphicsScene* _imageScene;
    QImage* _image;
    xvtf::Bitmap::VTFFile* _vtfImage;
    QGraphicsView* ImageViewer;
};

#endif // EDITOR_H
