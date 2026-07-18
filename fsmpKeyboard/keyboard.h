#ifndef KEYBOARD_H
#define KEYBOARD_H

/*******************************************************************
 *
 * 软键盘类： KeyBoard
 * 注意事项： 仅支持QLineEdit
 * 使用方法：
 *      以设置键盘对象为行编辑框的过滤器即可
 *      eg：
 *          //构造键盘对象
 *          KeyBoard *keyboard = new KeyBoard();
 *
 *          //将键盘设置为某行编辑框的过滤器（每个行编辑框需要单独设置）
 *          ui->lineEdit->installEventFilter(keyboard);
 *          ui->lineEdit1->installEventFilter(keyboard);
 *
 * *****************************************************************/

#include <QDialog>

#include <QButtonGroup>
#include <QList>
#include <QLineEdit>
#include <QDebug>
#include <QEvent>

namespace Ui {
class KeyBoard;
}

class KeyBoard : public QDialog
{
    Q_OBJECT

public:
    explicit KeyBoard(QWidget *parent = 0);
    ~KeyBoard();
    QString outputStr;

protected:
    QString findChinese(QString PinYin);
    void setHanziBt();

private slots:
    void on_caps_Bt_clicked();
    void slot_numClicked(QAbstractButton *bt);
    void slot_carClicked(QAbstractButton *bt);
    void slot_hanziBtClicked(QAbstractButton *bt);

    void on_zh_us_Bt_clicked();
    void on_delBt_clicked();
    void on_nextBt_clicked();
    void on_prevBt_clicked();
    void on_closeBt_clicked();

    void on_spaceBt_clicked();

private:
    bool eventFilter(QObject *watched, QEvent *event);
    Ui::KeyBoard *ui;

    bool isCap;                         //大小写控制
    QButtonGroup *carGroup;
    QList<QAbstractButton *> carList;
    QList<QAbstractButton *> zhList;

    QStringList zikuList;               // 字库一行一行分割

    int pageAll;                        // 总页数
    int curPage;                        // 当前页
    QStringList pageList;               // 查找出的字符串10个10个分割

    QLineEdit *lineEdit;
    QString data;

    void loadziku();                    // 加载中文字库
};

#endif // KEYBOARD_H
