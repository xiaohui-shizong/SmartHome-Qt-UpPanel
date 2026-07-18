#include "widget.h"
#include "ui_widget.h"
#include <cmath>
#include <QStack>
#include <QStringList>
#include <QRegularExpression>

widget::widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::widget)
{
    ui->setupUi(this);

    // 渐变背景代码
        this->setStyleSheet("QWidget{"
            "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1,"
            "stop:0 #87CEEB, stop:1 #E6E6FA);"
        "}");


}

widget::~widget()
{
    delete ui;
}

// ================= 基础UI交互逻辑 =================
void widget::on_pushButton_0_clicked()  { ui->lineEdit->setText(ui->lineEdit->text() + "0"); }
void widget::on_pushButton_1_clicked()  { ui->lineEdit->setText(ui->lineEdit->text() + "1"); }
void widget::on_pushButton_2_clicked()  { ui->lineEdit->setText(ui->lineEdit->text() + "2"); }
void widget::on_pushButton_3_clicked()  { ui->lineEdit->setText(ui->lineEdit->text() + "3"); }
void widget::on_pushButton_4_clicked()  { ui->lineEdit->setText(ui->lineEdit->text() + "4"); }
void widget::on_pushButton_5_clicked()  { ui->lineEdit->setText(ui->lineEdit->text() + "5"); }
void widget::on_pushButton_6_clicked()  { ui->lineEdit->setText(ui->lineEdit->text() + "6"); }
void widget::on_pushButton_7_clicked()  { ui->lineEdit->setText(ui->lineEdit->text() + "7"); }
void widget::on_pushButton_8_clicked()  { ui->lineEdit->setText(ui->lineEdit->text() + "8"); }
void widget::on_pushButton_9_clicked()  { ui->lineEdit->setText(ui->lineEdit->text() + "9"); }
void widget::on_pushButton_10_clicked() { ui->lineEdit->setText(ui->lineEdit->text() + "."); }
void widget::on_pushButton_12_clicked() { ui->lineEdit->setText(ui->lineEdit->text() + "+"); }
void widget::on_pushButton_13_clicked() { ui->lineEdit->setText(ui->lineEdit->text() + "-"); }
void widget::on_pushButton_14_clicked() { ui->lineEdit->setText(ui->lineEdit->text() + "/"); }
void widget::on_pushButton_18_clicked() { ui->lineEdit->setText(ui->lineEdit->text() + "*"); }
void widget::on_pushButton_19_clicked() { ui->lineEdit->setText(ui->lineEdit->text() + "%"); }
void widget::on_pushButton_20_clicked() { ui->lineEdit->setText(ui->lineEdit->text() + "√"); }
void widget::on_pushButton_21_clicked() { ui->lineEdit->setText(ui->lineEdit->text() + "²"); }
void widget::on_pushButton_22_clicked() { ui->lineEdit->setText(ui->lineEdit->text() + "⁻¹"); }

void widget::on_pushButton_15_clicked()
{
    ui->lineEdit->setText("");
}

void widget::on_pushButton_17_clicked()
{
    QString text = ui->lineEdit->text();
    if (!text.isEmpty()) {
        text.chop(1); // chop直接修改原字符串
        ui->lineEdit->setText(text);
    }
}

void widget::on_pushButton_11_clicked()
{
    QString exp = ui->lineEdit->text();
    if (exp.isEmpty()) return;
    QString result = calculate(exp);
    ui->lineEdit->setText(result);
}

// ================= 核心计算引擎 =================

// 获取运算符优先级
int widget::getPriority(const QString &op) {
    if (op == "+" || op == "-") return 1;
    if (op == "*" || op == "/" || op == "%") return 2;
    if (op == "√" || op == "²" || op == "⁻¹") return 3; // 单目运算符优先级最高
    return 0;
}

// 执行二元运算
double widget::applyOp(double a, double b, const QString &op) {
    if (op == "+") return a + b;
    if (op == "-") return a - b;
    if (op == "*") return a * b;
    if (op == "/") {
        if (b == 0) throw QString("除数不能为0");
        return a / b;
    }
    if (op == "%") {
        if (b == 0) throw QString("除数不能为0");
        return fmod(a, b);
    }
    return 0;
}

// 执行单目运算
double widget::applyUnaryOp(double a, const QString &op) {
    if (op == "√") {
        if (a < 0) throw QString("负数不能开根");
        return sqrt(a);
    }
    if (op == "²") return pow(a, 2);
    if (op == "⁻¹") {
        if (a == 0) throw QString("分母不能为0");
        return 1.0 / a;
    }
    return 0;
}

// 主计算函数：中缀表达式转后缀并求值
QString widget::calculate(QString expr)
{
    if (expr.isEmpty()) return "";

    // 1. 预处理：将表达式分割为 Token (数字、运算符、括号)
    QStringList tokens;
    QString tempNum;
    for (int i = 0; i < expr.length(); ++i) {
        QChar c = expr[i];

        if (c.isDigit() || c == '.') {
            tempNum += c;
        } else if(i + 1 < expr.length() && expr.mid(i, 2) == "⁻¹"){
            // 先把之前拼好的数字存起来
            if (!tempNum.isEmpty()) {
                tokens.append(tempNum);
                tempNum.clear();
            }
            // 把 "⁻¹" 作为一个完整的运算符 Token 加入
            tokens.append("⁻¹");
            i++; // 跳过下一个字符 "¹"，因为已经处理过了
        }
        else{
                // 遇到其他非数字字符，先把之前拼好的数字存起来
                if (!tempNum.isEmpty()) {
                    tokens.append(tempNum);
                    tempNum.clear();
                }

                // 判断这个 '-' 到底是减号，还是负号？
                if (c == '-' && (tokens.isEmpty() ||
                                 tokens.last() == "+" ||
                                 tokens.last() == "-" ||
                                 tokens.last() == "*" ||
                                 tokens.last() == "/" ||
                                 tokens.last() == "(")) {
                    // 如果是负号，把它和下一个数字绑定
                    tempNum += c;
                 } else {
                    // 否则，它就是一个普通的运算符
                    tokens.append(c);
                 }
        }

}
    if (!tempNum.isEmpty()) tokens.append(tempNum);

    // 2. 中缀转后缀 (Shunting-yard 算法)
    QStringList output;
    QStack<QString> opStack;

    for (int i = 0; i < tokens.size(); ++i) {
        const QString &token = tokens[i];

        // 如果是数字
        bool isNumber;
        token.toDouble(&isNumber);
        if (isNumber) {
            output.append(token);
        }
        // 如果是单目运算符 (√, ², ⁻¹)
        else if (token == "√" || token == "²" || token == "⁻¹") {
            while (!opStack.isEmpty() && getPriority(opStack.top()) >= getPriority(token)) {
                output.append(opStack.pop());
            }
            opStack.push(token);
        }
        // 如果是二元运算符
        else if (token == "+" || token == "-" || token == "*" || token == "/" || token == "%") {
            while (!opStack.isEmpty() && opStack.top() != "(" &&
                   getPriority(opStack.top()) >= getPriority(token)) {
                output.append(opStack.pop());
            }
            opStack.push(token);
        }
        // 左括号
        else if (token == "(") {
            opStack.push(token);
        }
        // 右括号
        else if (token == ")") {
            while (!opStack.isEmpty() && opStack.top() != "(") {
                output.append(opStack.pop());
            }
            if (!opStack.isEmpty() && opStack.top() == "(") {
                opStack.pop(); // 弹出左括号
            }
        }
    }
    while (!opStack.isEmpty()) {
        output.append(opStack.pop());
    }

    // 3. 后缀表达式求值
    QStack<double> valStack;
    try {
        for (const QString &token : output) {
            bool isNumber;
            double val = token.toDouble(&isNumber);
            if (isNumber) {
                valStack.push(val);
            } else if (token == "√" || token == "²" || token == "⁻¹") {
                if (valStack.isEmpty()) throw QString("表达式错误");
                double a = valStack.pop();
                valStack.push(applyUnaryOp(a, token));
            } else {
                if (valStack.size() < 2) throw QString("表达式错误");
                double b = valStack.pop();
                double a = valStack.pop();
                valStack.push(applyOp(a, b, token));
            }
        }
    } catch (const QString &errMsg) {
        return errMsg;
    }

    if (valStack.size() != 1) return "表达式错误";

    double result = valStack.pop();
    // 格式化输出，去除多余的0
    return QString::number(result, 'g', 15);
}
