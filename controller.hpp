#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QUdpSocket>

class Controller : public QObject {
	Q_OBJECT
public:
	Controller(QObject *parent = nullptr, QString ip = "127.0.0.1",
		   quint16 port = 52381);

	QString getIp() { return m_ip.toString(); };
	QString getPort() { return QString::number(m_port); };

public slots:
	void setIp(QString ip);
	void setPort(int port);

	void powerOn();
	void powerOff();

	void moveStop();
	void moveUp(unsigned short speed);
	void moveUpRight(unsigned short speed);
	void moveRight(unsigned short speed);
	void moveDownRight(unsigned short speed);
	void moveDown(unsigned short speed);
	void moveDownLeft(unsigned short speed);
	void moveLeft(unsigned short speed);
	void moveUpLeft(unsigned short speed);

	void zoomStop();
	void zoomIn(unsigned short speed);
	void zoomOut(unsigned short speed);

	void presetSet(unsigned short preset);
	void presetRecall(unsigned short preset);

private:
	void sendCommand(QByteArray body);
	unsigned long nextSequenceNumber();

	QHostAddress m_ip;
	quint16 m_port;
	QUdpSocket *m_socket;
	unsigned long m_sequenceNumber;
};

#endif // CONTROLLER_H
