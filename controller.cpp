#include "controller.hpp"

Controller::Controller(QObject *parent, QString initialIp, quint16 initalPort)
	: QObject(parent)
{
	m_socket = new QUdpSocket(this);
	m_ip = QHostAddress(initialIp);
	m_port = initalPort;
	m_sequenceNumber = 0;
}

void Controller::setIp(QString givenIp)
{
	m_ip = QHostAddress(givenIp);
}

void Controller::setPort(int givenPort)
{
	m_port = (quint16)givenPort;
}

unsigned long Controller::nextSequenceNumber()
{
	return m_sequenceNumber++;
}

void Controller::sendCommand(QByteArray body)
{
	int bodyLength = body.size();
	unsigned long seqNum = nextSequenceNumber();

	QByteArray command;
	// Command type
	command.append("\x01\x00", 2);
	// Body length (one byte at a time)
	command.append((bodyLength >> 8) & 0xFF);
	command.append(bodyLength);
	// Sequence number (one byte at a time)
	command.append((seqNum >> 24) & 0xFF);
	command.append((seqNum >> 16) & 0xFF);
	command.append((seqNum >> 8) & 0xFF);
	command.append(seqNum);
	command.append(body);

	m_socket->writeDatagram(command, m_ip, m_port);
}

void Controller::powerOn()
{
	sendCommand(QByteArray("\x81\x01\x04\x00\x02\xFF", 6));
	qDebug() << "Powered on";
}

void Controller::powerOff()
{
	sendCommand(QByteArray("\x81\x01\x04\x00\x03\xFF", 6));
	qDebug() << "Powered off";
}

void Controller::moveStop()
{
	sendCommand(QByteArray("\x81\x01\x06\x01\x00\x00\x03\x03\xFF", 9));
}

void Controller::moveUp(unsigned short speed)
{
	QByteArray command("\x81\x01\x06\x01");
	command.append(speed);
	command.append(speed);
	command.append("\x03\x01\xFF");
	sendCommand(command);
}

void Controller::moveUpRight(unsigned short speed)
{
	QByteArray command("\x81\x01\x06\x01");
	command.append(speed);
	command.append(speed);
	command.append("\x02\x01\xFF");
	sendCommand(command);
}

void Controller::moveRight(unsigned short speed)
{
	QByteArray command("\x81\x01\x06\x01");
	command.append(speed);
	command.append(speed);
	command.append("\x02\x03\xFF");
	sendCommand(command);
}

void Controller::moveDownRight(unsigned short speed)
{
	QByteArray command("\x81\x01\x06\x01");
	command.append(speed);
	command.append(speed);
	command.append("\x02\x02\xFF");
	sendCommand(command);
}

void Controller::moveDown(unsigned short speed)
{
	QByteArray command("\x81\x01\x06\x01");
	command.append(speed);
	command.append(speed);
	command.append("\x03\x02\xFF");
	sendCommand(command);
}

void Controller::moveDownLeft(unsigned short speed)
{
	QByteArray command("\x81\x01\x06\x01");
	command.append(speed);
	command.append(speed);
	command.append("\x01\x02\xFF");
	sendCommand(command);
}

void Controller::moveLeft(unsigned short speed)
{
	QByteArray command("\x81\x01\x06\x01");
	command.append(speed);
	command.append(speed);
	command.append("\x01\x03\xFF");
	sendCommand(command);
}

void Controller::moveUpLeft(unsigned short speed)
{
	QByteArray command("\x81\x01\x06\x01");
	command.append(speed);
	command.append(speed);
	command.append("\x01\x01\xFF");
	sendCommand(command);
}

void Controller::zoomStop()
{
	sendCommand(QByteArray("\x81\x01\x04\x07\x00\xFF", 6));
}

void Controller::zoomIn(unsigned short speed)
{
	QByteArray command("\x81\x01\x04\x07");
	speed += 32; // Zoom in speed is formatted in hex as "2[speed]"
	command.append(speed);
	command.append("\xFF");
	sendCommand(command);
}

void Controller::zoomOut(unsigned short speed)
{
	QByteArray command("\x81\x01\x04\x07");
	speed += 48; // Zoom out speed is formatted in hex as "3[speed]"
	command.append(speed);
	command.append("\xFF");
	sendCommand(command);
}

void Controller::presetSet(unsigned short preset)
{
	QByteArray command("\x81\x01\x04\x3F\x01");
	command.append(preset);
	command.append("\xFF");
	sendCommand(command);
}

void Controller::presetRecall(unsigned short preset)
{
	QByteArray command("\x81\x01\x04\x3F\x02");
	command.append(preset);
	command.append("\xFF");
	sendCommand(command);
}
