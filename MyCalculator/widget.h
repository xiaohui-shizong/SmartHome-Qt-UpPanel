#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QString> // 确保包含 QString 头文件

QT_BEGIN_NAMESPACE
namespace Ui { class widget; }
QT_END_NAMESPACE

class widget : public QWidget
{
    Q_OBJECT

public:
    widget(QWidget *parent = nullptr);
    ~widget();

    // ==================== 【重点】在这里添加缺失的声明 ====================
    // 这里的参数类型和名称必须与 .cpp 文件中完全一致
    int getPriority(const QString &op);
    double applyOp(double a, double b, const QString &op);
    double applyUnaryOp(double num, const QString &symbol);
    QString calculate(QString expr);
    // =====================================================================

private slots:
    void on_pushButton_0_clicked();

    void on_pushButton_1_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_10_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_13_clicked();

    void on_pushButton_14_clicked();

    void on_pushButton_17_clicked();

    void on_pushButton_15_clicked();

    void on_pushButton_19_clicked();

    void on_pushButton_20_clicked();

    void on_pushButton_21_clicked();

    void on_pushButton_22_clicked();

    void on_pushButton_18_clicked();

private:
    Ui::widget *ui;
};
#endif // WIDGET_H
