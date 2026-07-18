#include "keyboard.h"
#include "ui_keyboard.h"

#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QByteArray>
#include <QTextCodec>

KeyBoard::KeyBoard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KeyBoard)
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF8"));
    ui->setupUi(this);
    data = "";
    //this->setStyleSheet("QDialog{background-color: lightblue;}");

    isCap = false;
    outputStr = "";
    pageAll = 0;
    curPage = 0;

    ui->nextBt->setEnabled(false);
    ui->prevBt->setEnabled(false);

    QButtonGroup *numGroup = new QButtonGroup();
    numGroup->addButton(ui->num_0_Bt, 0);
    numGroup->addButton(ui->num_1_Bt, 1);
    numGroup->addButton(ui->num_2_Bt, 2);
    numGroup->addButton(ui->num_3_Bt, 3);
    numGroup->addButton(ui->num_4_Bt, 4);
    numGroup->addButton(ui->num_5_Bt, 5);
    numGroup->addButton(ui->num_6_Bt, 6);
    numGroup->addButton(ui->num_7_Bt, 7);
    numGroup->addButton(ui->num_8_Bt, 8);
    numGroup->addButton(ui->num_9_Bt, 9);
    connect( numGroup, SIGNAL(buttonClicked(QAbstractButton*)),
            this, SLOT(slot_numClicked(QAbstractButton*)) );

    QButtonGroup *carGroup = new QButtonGroup();
    carGroup->addButton(ui->q_Bt, 1);
    carGroup->addButton(ui->w_Bt, 2);
    carGroup->addButton(ui->e_Bt, 3);
    carGroup->addButton(ui->r_Bt, 4);
    carGroup->addButton(ui->t_Bt, 5);
    carGroup->addButton(ui->y_Bt, 6);
    carGroup->addButton(ui->u_Bt, 7);
    carGroup->addButton(ui->i_Bt, 8);
    carGroup->addButton(ui->o_Bt, 9);
    carGroup->addButton(ui->p_Bt, 10);
    carGroup->addButton(ui->a_Bt, 11);
    carGroup->addButton(ui->s_Bt, 12);
    carGroup->addButton(ui->d_Bt, 13);
    carGroup->addButton(ui->f_Bt, 14);
    carGroup->addButton(ui->g_Bt, 15);
    carGroup->addButton(ui->h_Bt, 16);
    carGroup->addButton(ui->j_Bt, 17);
    carGroup->addButton(ui->k_Bt, 18);
    carGroup->addButton(ui->l_Bt, 19);
    carGroup->addButton(ui->z_Bt, 20);
    carGroup->addButton(ui->x_Bt, 21);
    carGroup->addButton(ui->c_Bt, 22);
    carGroup->addButton(ui->v_Bt, 23);
    carGroup->addButton(ui->b_Bt, 24);
    carGroup->addButton(ui->n_Bt, 25);
    carGroup->addButton(ui->m_Bt, 26);
    connect( carGroup, SIGNAL(buttonClicked(QAbstractButton*)),
            this, SLOT(slot_carClicked(QAbstractButton*)) );
    carList = carGroup->buttons();

    QButtonGroup *hanziGroup = new QButtonGroup();
    hanziGroup->addButton(ui->zh_1_Bt, 1);
    hanziGroup->addButton(ui->zh_2_Bt, 2);
    hanziGroup->addButton(ui->zh_3_Bt, 3);
    hanziGroup->addButton(ui->zh_4_Bt, 4);
    hanziGroup->addButton(ui->zh_5_Bt, 5);
    hanziGroup->addButton(ui->zh_6_Bt, 6);
    hanziGroup->addButton(ui->zh_7_Bt, 7);
    hanziGroup->addButton(ui->zh_8_Bt, 8);
    hanziGroup->addButton(ui->zh_9_Bt, 9);
    hanziGroup->addButton(ui->zh_10_Bt, 10);
    connect( hanziGroup, SIGNAL(buttonClicked(QAbstractButton*)),
             this, SLOT(slot_hanziBtClicked(QAbstractButton*)));
    zhList = hanziGroup->buttons();

    // 加载字库
    loadziku();
    setGeometry(QRect(QPoint(0, 250), QPoint(850, 470)));

}

bool KeyBoard::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        lineEdit = static_cast<QLineEdit*>(watched);
        Qt::WindowFlags m_flags = windowFlags();
        setWindowFlags(m_flags | Qt::WindowStaysOnTopHint);
  //      this->setWindowOpacity(0.5);

        setWindowFlag(Qt::FramelessWindowHint);
        setAutoFillBackground(true);
        setAttribute(Qt::WA_TranslucentBackground, true);//设置窗口背景透明
        QPalette pal = palette();
        pal.setColor(QPalette::Background, QColor(0x00,0xff,0x00,0x00));
        setPalette(pal);
        //this->showFullScreen();
        ui->widget->setStyleSheet("QWidget{background-color: lightblue;}");
        this->show();

    }

    return QWidget::eventFilter(watched, event);
}

KeyBoard::~KeyBoard()
{
    delete ui;
}

/* 加载字库 */
void KeyBoard::loadziku()
{
    // 加载字库文件
    QFile zikuFile(":/fsmpKeyboard/ziku/ziku.dat");
    if ( !zikuFile.open(QIODevice::ReadOnly | QIODevice::Text) )
    {
        QMessageBox::information(this, "键盘字库",
                                 "加载字库文件错误，中文输入不可用！", QMessageBox::Ok);
        ui->zh_us_Bt->setEnabled(false);
        return;
    }

    QString zikuAll;

    while (true)
    {
        char buf[1024];
        qint64 len = zikuFile.readLine(buf, sizeof(buf));

        if (len <= 0)
        {
            break;
        }

        QString str = QString::fromLocal8Bit(buf);
        zikuAll += str;

    }

    zikuList = zikuAll.split("\n");      //用QString的split函数将字库文件划分成一段一段，每个拼音对应一段字库

    zikuFile.close();
}

/* 查找中文 */
QString KeyBoard::findChinese(QString PinYin)
{
    QStringList lst;
    QString Line;
    QString returnChinese;

    for(int i = 0; i < zikuList.count(); i++)//m_srf.count()QStringList有多少段
    {
        Line = zikuList.at(i);
        if (Line.isEmpty() || PinYin.isEmpty()) continue;

        lst = Line.split(",");
        if (QString::compare(lst.at(0), PinYin, Qt::CaseInsensitive) == 0)
        {
            returnChinese =  lst.at(1);
        }
    }

    // 字符串分割成10个10个一组
    pageList.clear();
    int len = returnChinese.length();
    if (len > 0)
    {
        pageAll = len % 10 == 0 ? len/10 : (len/10) + 1;
        QString str;
        for (int i = 1; i <= len; i++)
        {
            str += returnChinese.at(i-1);
            if (i % 10 == 0 && i != 0)
            {
                pageList << str;
                str = "";
            }
            QByteArray a = str.toLatin1();
            char* p = a.data();
            //qDebug()<<"str"<<p[0]+p[1]+p[2];
        }

        if (len % 10 != 0)
        {
            pageList << returnChinese.right(len % 10);
        }
    }//if

    return returnChinese;
}

/* 汉字按钮的设置 */
void KeyBoard::setHanziBt()
{

    for (int i = 0; i < 10; i++)
    {
        zhList.at(i)->setText("");
    }

    QString PinYin = ui->showLb->text();
    QString ChineseStr = findChinese(PinYin);

    ui->nextBt->setEnabled(false);
    ui->prevBt->setEnabled(false);

    if (ChineseStr != "")
    {
        int ChineseStrCount = ChineseStr.length();
        // 10字以内汉字个数
        if (ChineseStrCount <= 10)
        {
            for (int i = 0; i < ChineseStrCount; i++)
            {
                ((QPushButton *)zhList.at(i))->setText(ChineseStr.at(i));
            }
            return;
        }

        // 大于十字
        for (int i = 0; i < 10; i++)
        {
            ((QPushButton *)zhList.at(i))->setText(ChineseStr.at(i));
        }

        ui->nextBt->setEnabled(true);
        ui->prevBt->setEnabled(true);

    }//if
}

/* 大小写转换 */
void KeyBoard::on_caps_Bt_clicked()
{
    if (isCap == false) // 小写状态
    {
        for (int i = 0; i < carList.size(); i++)
        {
            QString upper = carList.at(i)->text().toUpper();
            carList.at(i)->setText(upper);
        }
        isCap = true;
        return;
    }
    else // 大写状态
    {
        for (int i = 0; i < carList.size(); i++)
        {
            QString lower = carList.at(i)->text().toLower();
            carList.at(i)->setText(lower);
        }
        isCap = false;
        return;
    }
}


/* 数字键盘点击 */
void KeyBoard::slot_numClicked(QAbstractButton *bt)
{

    QPushButton *tmpBt = (QPushButton *)bt;
    /*
    qDebug()<<"-------------" << tmpBt->text();
    QString str = data;
    str.append(tmpBt->text());
    data = str;
*/
    QString str_1 = ui->showLb->text();
    str_1.append(tmpBt->text());
    ui->showLb->setText(str_1);

    QString str_2 = lineEdit->text();
    str_2.append(tmpBt->text());
    lineEdit->setText(str_2);
}

/* 字母键点击 */
void KeyBoard::slot_carClicked(QAbstractButton *bt)
{
    QPushButton *tmp = (QPushButton *)bt;
    QString str_1 = ui->showLb->text();
    str_1.append(tmp->text());
    ui->showLb->setText(str_1);
    //qDebug()<<"zimu"<<str_1;
    if (ui->langLb->text() == "英文")
    {
        QString str_2 = lineEdit->text();
        str_2.append(tmp->text());
        lineEdit->setText(str_2);
    }
    else
    {
        setHanziBt();
    }

}

/* 十个中文按钮点击 */
void KeyBoard::slot_hanziBtClicked(QAbstractButton *bt)
{
    QPushButton *tmp = (QPushButton *)bt;
    QString str = lineEdit->text();
    str.append(tmp->text());
    lineEdit->setText(str);
    ui->showLb->setText("");
    for (int i = 0; i < 10; i++)
    {
        zhList.at(i)->setText("");
    }

    ui->nextBt->setEnabled(false);
    ui->prevBt->setEnabled(false);
}

/* 中英切换按钮 */
void KeyBoard::on_zh_us_Bt_clicked()
{
    if (ui->langLb->text() == "英文")
    {
        ui->langLb->setText("中文");
        ui->showLb->setText("");
    }
    else
    {
        ui->langLb->setText("英文");
        ui->showLb->setText("");
    }
}

/* 退格键 */
void KeyBoard::on_delBt_clicked()
{
    QString tmp = ui->showLb->text();
    ui->showLb->setText(tmp.left(tmp.length() - 1));

    if (ui->langLb->text() == "英文")
    {
        QString str = lineEdit->text();
        lineEdit->setText(str.left(str.length() - 1));
    }
    else
    {
        for (int i = 0; i < 10; i++)
        {
            zhList.at(i)->setText("");
        }
        setHanziBt();
        if (ui->showLb->text() == "") // 中文模式下，showLb中删除完毕才去删除LineEdit里的文字
        {
            QString str = lineEdit->text();
            lineEdit->setText(str.left(str.length() - 1));
        }
    }
}

/* 翻页按键 */
void KeyBoard::on_nextBt_clicked()
{
    curPage++;

    if (curPage > pageAll)
    {
        curPage = 1;
    }

    for (int i = 0; i < 10; i++)
    {
        zhList.at(i)->setText("");
    }

    QString str = pageList.at(curPage-1);
    qDebug() << str;
    for (int i = 0; i < str.length(); i++)
    {
        zhList.at(i)->setText(str.at(i));
    }
}

void KeyBoard::on_prevBt_clicked()
{
    curPage--;
    if (curPage == 0)
    {
        curPage = pageAll;
    }

    for (int i = 0; i < 10; i++)
    {
        zhList.at(i)->setText("");
    }

    QString str = pageList.at(curPage-1);
    for (int i = 0; i < str.length(); i++)
    {
        zhList.at(i)->setText(str.at(i));
    }
}

void KeyBoard::on_closeBt_clicked()
{
    this->close();
}


void KeyBoard::on_spaceBt_clicked()
{
    QString str_1 = ui->showLb->text();
    str_1.append(' ');
    ui->showLb->setText(str_1);

    QString str_2 = lineEdit->text();
    str_2.append(' ');
    lineEdit->setText(str_2);
}

