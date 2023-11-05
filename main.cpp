#include <QCoreApplication>
#include <QTextStream>
#include <QDebug>
#include <QTcpSocket>
#include <QHostAddress>

// 连接服务端
void ConnectServer(const QHostAddress& IPValue, int PortValue) {
    // 创建Socket对象
    QTcpSocket tcpSocket;
    // 连接服务端
    tcpSocket.connectToHost(QHostAddress(IPValue), PortValue);
    // 等待连接成功
    if (!tcpSocket.waitForConnected()) {
        qDebug("\033[31m[ERROR] Failed to Connect Server!\033[0m");
        return;
    }
    // 连接成功
    qDebug("\033[32m[OK] Connected to [%s:%d]!\033[0m", qPrintable(IPValue.toString()), PortValue);
    // 等待服务端发送欢迎信息
    if (!tcpSocket.waitForReadyRead()) {
        qDebug("\033[31m[ERROR] Failed to Receive Message from Server!\033[0m");
        return;
    }
    // 接收欢迎信息
    qDebug(qPrintable(tcpSocket.readAll()));
    // 服务端-客户端通信
    while (true) {
        qDebug("[INPUT] Domain-Specific Command:");
        // 获取用户指令
        QTextStream cin(stdin, QIODevice::ReadOnly);
        QString command = cin.readLine();
        // 是否结束指令
        if (command == "exit") {
            // 发送结束指令
            tcpSocket.write(command.toUtf8());
            // 结束指令
            qDebug("\033[33m[FINISH] Client Exited!\033[0m");
            // 结束通信 关闭Socket
            tcpSocket.disconnected();
            tcpSocket.close();
            return;
        }
        // 发送指令
        tcpSocket.write(command.toUtf8());
        // 等待服务端响应
        if (!tcpSocket.waitForReadyRead()) {
            qDebug("\033[31m[ERROR] Failed to Receive Message from Server!\033[0m");
            return;
        }
        // 接收响应
        qDebug();
        qDebug("[SERVER] @%d: %s", PortValue, qPrintable(tcpSocket.readAll()));
    }
}

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    // 检查命令行参数
    if (argc > 1) {
        // 获取IP地址
        QHostAddress IPValue = QHostAddress(QString(argv[1]));
        // 获取端口号
        int PortValue = QString(argv[2]).toInt();
        // 检查端口号是否合法
        if (PortValue < 0 || PortValue > 65535) {
            // 端口号不合法
            qDebug("\033[31mRed [ERROR] The Port[%d] is Invalid!\033[0m", PortValue);
            return 0;
        }
        // 连接服务端
        ConnectServer(IPValue, PortValue);
        return 0;
    }
    // 使用默认IP地址和端口号连接服务端
    ConnectServer(QHostAddress("127.0.0.1"), 3000);
    return 0;
}
