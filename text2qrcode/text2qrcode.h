#ifndef TEXT2QRCODE_H
#define TEXT2QRCODE_H
/***************************************************************************
 *
 * 类名：text2Qrcode
 * 功能说明：生成二维码类
 * 公有函数：
 *      text2Qrcode(QWidget *parent = nullptr);
 *
 * 信号：
 *      无
 * 槽：
 *      QPixmap toPixmap(QString strUrl)；       //文字转二维码
 *
 * *************************************************************************/
#include <QObject>
#include <QPixmap>
#include "qrencode.h"
#include <QDebug>
#include <QPainter>

class text2Qrcode:public QObject{
    Q_OBJECT

public:
    text2Qrcode(QObject *parent=nullptr)
        :QObject(parent)
    {

    }

    QPixmap toPixmap(QString strUrl)
    {
        if(strUrl.isEmpty())
            return QPixmap();
        QRcode* qrcode = QRcode_encodeString(strUrl.toStdString().c_str(), 2, QR_ECLEVEL_Q, QR_MODE_8, 1);

        qint32 temp_width = 500;
        qint32 temp_height = 500;
        //qDebug() << "temp_width=" << temp_width << ";temp_height=" << temp_height;

        qint32 qrcode_width = qrcode->width > 0 ? qrcode->width : 1;
        double scale_x = (double)temp_width / (double)qrcode_width;
        double scale_y = (double)temp_height / (double)qrcode_width;

        int offset = 14;
        QImage mainimg = QImage(temp_width + offset * 2, temp_height + offset * 2, QImage::Format_ARGB32);
        QPainter painter(&mainimg);
        QColor background(Qt::white);
        painter.setBrush(background);
        painter.setPen(Qt::NoPen);
        painter.drawRect(offset, offset, temp_width, temp_height);
        QColor foreground(Qt::black);
        painter.setBrush(foreground);

        for (qint32 y = 0; y < qrcode_width; y++)
        {
            for (qint32 x = 0; x < qrcode_width; x++)
            {
                unsigned char b = qrcode->data[y * qrcode_width + x];
                if (b & 0x01)
                {
                    QRectF r(offset + x * scale_x, offset + y * scale_y, scale_x, scale_y);
                    painter.drawRects(&r, 1);
                }
            }
        }

        painter.setPen(QColor(0, 0, 255));
        //painter.drawText(temp_width / 2, temp_height + offset * 2 - 2, strUrl);
        QPixmap mainmap = QPixmap::fromImage(mainimg);

        return mainmap;
    }

};

#endif // TEXT2QRCODE_H
