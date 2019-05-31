#include "channelselector.h"

#include <QDebug>
#include <QGroupBox>
#include <QMouseEvent>
#include <QGraphicsView>
#include <QVBoxLayout>
#include <QHBoxLayout>

ChannelSelector::ChannelSelector(std::vector<ChannelLink> channels, QWidget *parent) :
    QWidget(parent)
{
    for (auto i : channels)
        _inputs.push_back(i);

    for (auto i : _inputs)
        _inputLbls.push_back(new QLabel(i.Name));

    QVBoxLayout* InputsLayout = new QVBoxLayout;
    for (auto i : _inputLbls)
        InputsLayout->addWidget(i, 0, Qt::AlignLeft);

    QVBoxLayout* OutputsLayout = new QVBoxLayout;
    _outR = new QLabel("R");
    _outG = new QLabel("G");
    _outB = new QLabel("B");
    _outA = new QLabel("A");

    OutputsLayout->addWidget(_outR, 0, Qt::AlignRight);
    OutputsLayout->addWidget(_outG, 0, Qt::AlignRight);
    OutputsLayout->addWidget(_outB, 0, Qt::AlignRight);
    OutputsLayout->addWidget(_outA, 0, Qt::AlignRight);

    QHBoxLayout* MainLayout = new QHBoxLayout(this);
    MainLayout->addLayout(InputsLayout);
    MainLayout->addLayout(OutputsLayout);

    _bIsDrawing = false;

    this->setMouseTracking(true);
    this->installEventFilter(this);
}

ChannelSelector::~ChannelSelector()
{

}

QSize ChannelSelector::sizeHint() const
{
    return QSize(200, 120);
}

bool ChannelSelector::PointContained(QPoint p, QPoint c, int size)
{
    QRect e;
    e.setCoords(c.x() - size, c.y() - size, c.x() + size, c.y() + size);

    bool bX = (p.x() > e.left()) && (p.x() < e.right());
    bool bY = (p.y() > e.top()) && (p.y() < e.bottom());
    return bX && bY;
}

void ChannelSelector::SendValues()
{
    emit ValueChanged(this->_inputs);
}

bool ChannelSelector::eventFilter(QObject *obj, QEvent *event)
{
    static uint inputN;

    if (event->type() == QEvent::MouseMove)
    {
        QMouseEvent* mEvent = static_cast<QMouseEvent*>(event);
        auto loc = mapFromGlobal(mEvent->globalPos());

        for (uint i = 0; i < _inputs.size(); ++i)
        {
            if (PointContained(loc, _inputLbls[i]->geometry().center()))
            {
                _inputLbls[i]->setStyleSheet("font-weight: bold");
                goto done_bold;
            }
            else
            {
                _inputLbls[i]->setStyleSheet("font-weight: normal");
            }
        }

        if (PointContained(loc, _outR->geometry().center()))
        {
            _outR->setStyleSheet("font-weight: bold");
            goto done_bold;
        }
        else
        {
            _outR->setStyleSheet("font-weight: normal");
        }

        if (PointContained(loc, _outG->geometry().center()))
        {
            _outG->setStyleSheet("font-weight: bold");
            goto done_bold;
        }
        else
        {
            _outG->setStyleSheet("font-weight: normal");
        }

        if (PointContained(loc, _outB->geometry().center()))
        {
            _outB->setStyleSheet("font-weight: bold");
            goto done_bold;
        }
        else
        {
            _outB->setStyleSheet("font-weight: normal");
        }

        if (PointContained(loc, _outA->geometry().center()))
        {
            _outA->setStyleSheet("font-weight: bold");
            goto done_bold;
        }
        else
        {
            _outA->setStyleSheet("font-weight: normal");
        }
    }
done_bold:

    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* mEvent = static_cast<QMouseEvent*>(event);
        if (mEvent->button() == Qt::LeftButton)
        {
            QLabel* lblObj = nullptr;

            for (uint i = 0; i < _inputs.size(); ++i)
            {
                if (PointContained(_inputLbls[i]->geometry().center(), mapFromGlobal(mEvent->globalPos())))
                {
                    lblObj = _inputLbls[i];
                    inputN = i;
                    break;
                }
            }

            _activeLbl = lblObj;

            if(lblObj != nullptr)
            {
                _bIsDrawing = true;
            }
        }

        return true;
    }
    else if (event->type() == QEvent::MouseButtonRelease && _bIsDrawing)
    {
        QMouseEvent* mEvent = static_cast<QMouseEvent*>(event);
        if (mEvent->button() == Qt::LeftButton)
        {
            // Check if we released in an output
            Channel ch = Channel::NONE;

            if (PointContained(mapFromGlobal(mEvent->globalPos()), _outR->geometry().center()))
            {
                ch = Channel::R;
            }
            else if (PointContained(mapFromGlobal(mEvent->globalPos()), _outG->geometry().center()))
            {
                ch = Channel::G;
            }
            else if (PointContained(mapFromGlobal(mEvent->globalPos()), _outB->geometry().center()))
            {
                ch = Channel::B;
            }
            else if (PointContained(mapFromGlobal(mEvent->globalPos()), _outA->geometry().center()))
            {
                ch = Channel::A;
            }

            // Connection To Be Made
            if (ch != Channel::NONE)
            {
                // Check if this is a disconnect
                bool bDisc = false;
                for (uint i = 0; i < _inputs[inputN].LinkedTo.size(); ++i)
                {
                    if (_inputs[inputN].LinkedTo[i] == ch)
                    {
                        _inputs[inputN].LinkedTo.erase(_inputs[inputN].LinkedTo.begin() + i);
                        bDisc = true;
                    }
                }

                if (!bDisc)
                {
                    // Clear any other channel linked there
                    for (uint i = 0; i < _inputs.size(); ++i)
                    {
                        for (uint j = 0; j < _inputs[i].LinkedTo.size(); ++j)
                        {
                            if (_inputs[i].LinkedTo[j] == ch)
                            {
                                _inputs[i].LinkedTo.erase(_inputs[i].LinkedTo.begin() + j);
                                break;
                            }
                        }
                    }

                _inputs[inputN].LinkedTo.push_back(ch);
                }
            }

            _activeLbl = nullptr;
            _bIsDrawing = false;

            update();
            SendValues();
        }

        return true;
    }
    else if (event->type() == QEvent::MouseButtonRelease && !_bIsDrawing)
    {
        QMouseEvent* mEvent = static_cast<QMouseEvent*>(event);
        if (mEvent->button() == Qt::LeftButton)
        {
            // Check if we're in an output
            bool bOutput = false;
            Channel ch = Channel::NONE;

            if (PointContained(mapFromGlobal(mEvent->globalPos()), _outR->geometry().center()))
            {
                bOutput = true;
                ch = Channel::R;
            }
            else if (PointContained(mapFromGlobal(mEvent->globalPos()), _outG->geometry().center()))
            {
                bOutput = true;
                ch = Channel::G;
            }
            else if (PointContained(mapFromGlobal(mEvent->globalPos()), _outB->geometry().center()))
            {
                bOutput = true;
                ch = Channel::B;
            }
            else if (PointContained(mapFromGlobal(mEvent->globalPos()), _outA->geometry().center()))
            {
                bOutput = true;
                ch = Channel::A;
            }

            if (bOutput)
            {
                // Disconnect whatever is currently connected
                for (uint i = 0; i < _inputs.size(); ++i)
                {
                    for (uint j = 0; j < _inputs[i].LinkedTo.size(); ++j)
                    {
                        if (_inputs[i].LinkedTo[j] == ch)
                        {
                            _inputs[i].LinkedTo.erase(_inputs[i].LinkedTo.begin() + j);
                        }
                    }
                }

                update();
                SendValues();
                return true;
            }
        }
    }
    else if (event->type() == QEvent::MouseMove && _bIsDrawing)
    {
        QMouseEvent* mEvent = static_cast<QMouseEvent*>(event);
        _MouseLoc = mEvent->globalPos();
        update();

        return true;
    }

    return QObject::eventFilter(obj, event);
}

void ChannelSelector::paintEvent(QPaintEvent *event)
{
    // Paint Connection
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw Connections
    // For each input
    for (uint i = 0; i < _inputs.size(); ++i)
    {
        auto inLoc = _inputLbls[i]->geometry().center();
        inLoc.setX(inLoc.x() + 8);
        inLoc.setY(inLoc.y() + 1);

        // For Each Channel
        for (auto ch : _inputs[i].LinkedTo)
        {
            QPoint outLoc;

            if (ch == Channel::R)
            {
                painter.setPen(Qt::red);
                outLoc = _outR->geometry().center();
                outLoc.setX(outLoc.x() - 8);
                outLoc.setY(outLoc.y() + 1);
            }
            else if (ch == Channel::G)
            {
                painter.setPen(Qt::green);
                outLoc = _outG->geometry().center();
                outLoc.setX(outLoc.x() - 8);
                outLoc.setY(outLoc.y() + 1);
            }
            else if (ch == Channel::B)
            {
                painter.setPen(Qt::blue);
                outLoc = _outB->geometry().center();
                outLoc.setX(outLoc.x() - 8);
                outLoc.setY(outLoc.y() + 1);
            }
            else if (ch == Channel::A)
            {
                painter.setPen(Qt::black);
                outLoc = _outA->geometry().center();
                outLoc.setX(outLoc.x() - 8);
                outLoc.setY(outLoc.y() + 1);
            }
            else if (ch == Channel::NONE)
            {
                continue;
            }

            painter.drawLine(inLoc, outLoc);
        }
    }

    if (_bIsDrawing)
    {
        auto inLoc = _activeLbl->geometry().center();
        inLoc.setX(inLoc.x() + 8);
        inLoc.setY(inLoc.y() + 1);
        auto outLoc = mapFromGlobal(_MouseLoc);

        painter.setPen(QPen(Qt::black, 1, Qt::PenStyle::DashLine));
        painter.drawLine(inLoc, outLoc);
    }
}
