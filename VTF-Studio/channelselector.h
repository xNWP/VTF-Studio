#ifndef CHANNELSELECTOR_H
#define CHANNELSELECTOR_H

#include <QLabel>
#include <QWidget>

#include <vector>

enum Channel
{
    NONE, R, G, B, A
};

struct ChannelLink
{
    ChannelLink(QString name, Channel link) : Name(name) { if (link != Channel::NONE) LinkedTo.push_back(link); }
    ChannelLink(QString name, const std::vector<Channel>& links) : Name(name), LinkedTo(links) { }
    QString Name; // The name of the input channel.
    std::vector<Channel> LinkedTo; // The channel(s) the input is linked to.
};


class ChannelSelector : public QWidget
{
    Q_OBJECT

public:
    explicit ChannelSelector(std::vector<ChannelLink> channels, QWidget *parent = nullptr);
    ~ChannelSelector() override;
    QSize sizeHint() const override;

signals:
    void ValueChanged(const std::vector<ChannelLink>& values);


private:
    bool PointContained(QPoint p, QPoint c, int size = 12);
    void SendValues();

    bool eventFilter(QObject *obj, QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

    QLabel* _outR;
    QLabel* _outG;
    QLabel* _outB;
    QLabel* _outA;

    std::vector<ChannelLink> _inputs;
    std::vector<QLabel*> _inputLbls;
    bool _bIsDrawing;

    QPoint _MouseLoc;
    QLabel* _activeLbl;
};

#endif // CHANNELSELECTOR_H
