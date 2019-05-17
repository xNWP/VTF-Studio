#ifndef EDITOR_H
#define EDITOR_H

#include "xVTF/xVTF.h"

#include <QMainWindow>
#include <QGraphicsScene>

namespace Ui {
class Editor;
}

class Editor : public QMainWindow
{
    Q_OBJECT

public:
    explicit Editor(QWidget *parent = nullptr);
    ~Editor();

private:
    void OpenImage();

private:
    Ui::Editor *ui;

    QGraphicsScene* _imageScene;
    QImage* _image;
    xvtf::Bitmap::VTFFile* _vtfImage;
};

#endif // EDITOR_H
