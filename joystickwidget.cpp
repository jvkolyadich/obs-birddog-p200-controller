#include "joystickwidget.hpp"

JoystickWidget::JoystickWidget(QWidget *parent) : QWidget(parent)
{
	QSizePolicy policy;
	policy.setHorizontalPolicy(QSizePolicy::Policy::Expanding);
	policy.setVerticalPolicy(QSizePolicy::Policy::Expanding);
	setSizePolicy(policy);

	m_state = m_prevState = {Direction::None, 0, 0};

	m_scrollWheelPos = 0;

	m_maxDistFromCenter = 200;

	m_shouldBindToMousePos = false;

	setMaxMoveSpeed(17);
	setMaxZoomSpeed(7);

	m_moveAdjustment = m_zoomAdjustment = 0;
	m_moveAdjustmentWeight = m_zoomAdjustmentWeight = 0;

	m_degPerZoom = 15;
	m_zoomArcSpan = 90;

	updateMoveSpeedSteps();
	updateZoomSpeedSteps();
}

QPointF JoystickWidget::centerPoint()
{
	return QPointF(size().width() / 2, size().height() / 2);
}

QRectF JoystickWidget::centerCircleBounds()
{
	float size = calcMoveSpeedStepAt(m_maxMoveSpeed - 1);
	QRectF bounds(-(size / 2), -(size / 2), size, size);
	if (m_shouldBindToMousePos)
		return bounds.translated(m_mousePos);
	return bounds.translated(centerPoint());
}

float JoystickWidget::calcMoveSpeedStepAt(int index)
{
	float xVal = (float)index / m_maxMoveSpeed;
	float multiplier = ((m_moveAdjustmentWeight *
			     (std::powf(xVal, m_moveAdjustment))) +
			    ((1 - m_moveAdjustmentWeight) * xVal));
	return (m_maxDistFromCenter - ((multiplier * m_maxMoveSpeed) *
				       (m_maxDistFromCenter / m_maxMoveSpeed)));
}

float JoystickWidget::calcZoomSpeedStepAt(int index)
{
	float xVal = (float)(m_maxZoomSpeed - index) / m_maxZoomSpeed;
	float multiplier = 1 - (((m_zoomAdjustmentWeight *
				  (std::powf(xVal, m_zoomAdjustment))) +
				 ((1 - m_zoomAdjustmentWeight) * xVal)));
	return ((m_maxZoomSpeed * m_degPerZoom) -
		((multiplier * m_maxZoomSpeed) * m_degPerZoom));
}

void JoystickWidget::updateMoveSpeedSteps()
{
	m_moveSpeedSteps.clear();

	for (int i = 0; i < m_maxMoveSpeed; i++) {
		MoveSpeedStep step;

		step.speed = m_maxMoveSpeed - i;

		step.radius = calcMoveSpeedStepAt(i);

		QRectF bounds(-(step.radius), -(step.radius), (step.radius * 2),
			      (step.radius * 2));
		step.bounds = bounds.translated(centerPoint());

		QColor background = palette().window().color();
		QColor secondary = palette().window().color();
		secondary = (secondary.lightness() > 128)
				    ? (secondary.darker(115))
				    : (secondary.lighter(140));
		step.color = (m_maxMoveSpeed % 2 == 0)
				     ? (i % 2 == 0 ? secondary : background)
				     : (i % 2 == 0 ? background : secondary);

		m_moveSpeedSteps.push_back(step);
	}

	update();
}

void JoystickWidget::updateZoomSpeedSteps()
{
	m_zoomSpeedSteps.clear();

	for (int i = 0; i < m_maxZoomSpeed; i++) {
		ZoomSpeedStep step;

		step.speed = m_maxZoomSpeed - i;

		step.deg = calcZoomSpeedStepAt(i);

		step.dispDeg = (m_zoomArcSpan * step.deg) /
			       (m_maxZoomSpeed * m_degPerZoom);

		m_zoomSpeedSteps.push_back(step);
	}

	update();
}

void JoystickWidget::updateState()
{
	if (m_shouldBindToMousePos) {
		QLineF normVector(centerPoint(), m_mousePos);
		qreal angle = normVector.angle();
		float length = (float)normVector.length();

		if (length < m_moveSpeedSteps.last().radius)
			m_state.direction = Direction::None;
		else if (292.5 <= angle && angle < 337.5)
			m_state.direction = Direction::DownRight;
		else if (247.5 <= angle && angle < 292.5)
			m_state.direction = Direction::Down;
		else if (202.5 <= angle && angle < 247.5)
			m_state.direction = Direction::DownLeft;
		else if (157.5 <= angle && angle < 202.5)
			m_state.direction = Direction::Left;
		else if (112.5 <= angle && angle < 157.5)
			m_state.direction = Direction::UpLeft;
		else if (67.5 <= angle && angle < 112.5)
			m_state.direction = Direction::Up;
		else if (22.5 <= angle && angle < 67.5)
			m_state.direction = Direction::UpRight;
		else
			m_state.direction = Direction::Right;

		for (int i = (m_moveSpeedSteps.length() - 1); i >= 0; i--) {
			if (m_moveSpeedSteps.at(i).radius < length)
				m_state.speed = m_moveSpeedSteps.at(i).speed;
		}
	} else {
		m_state.direction = Direction::None;
		m_state.speed = 0;
	}

	if (m_zoomSpeedSteps.last().deg < std::abs(m_scrollWheelPos)) {
		for (int i = (m_zoomSpeedSteps.length() - 1); i >= 0; i--) {
			if (m_zoomSpeedSteps.at(i).deg <
			    std::abs(m_scrollWheelPos))
				m_state.zoom = m_zoomSpeedSteps.at(i).speed *
					       m_zoomDirection;
		}
	} else {
		m_state.zoom = 0;
	}
}

void JoystickWidget::emitChanges()
{
	updateState();

	if (m_state.direction != m_prevState.direction ||
	    m_state.speed != m_prevState.speed) {
		switch (m_state.direction) {
		case Direction::Up:
			emit movedUp(m_state.speed);
#if DEBUG_MOVE
			qDebug() << "Moved Up at speed" << m_state.speed;
#endif
			break;
		case Direction::UpRight:
			emit movedUpRight(m_state.speed);
#if DEBUG_MOVE
			qDebug() << "Moved UpRight at speed" << m_state.speed;
#endif
			break;
		case Direction::Right:
			emit movedRight(m_state.speed);
#if DEBUG_MOVE
			qDebug() << "Moved Right at speed" << m_state.speed;
#endif
			break;
		case Direction::DownRight:
			emit movedDownRight(m_state.speed);
#if DEBUG_MOVE
			qDebug() << "Moved DownRight at speed" << m_state.speed;
#endif
			break;
		case Direction::Down:
			emit movedDown(m_state.speed);
#if DEBUG_MOVE
			qDebug() << "Moved Down at speed" << m_state.speed;
#endif
			break;
		case Direction::DownLeft:
			emit movedDownLeft(m_state.speed);
#if DEBUG_MOVE
			qDebug() << "Moved DownLeft at speed" << m_state.speed;
#endif
			break;
		case Direction::Left:
			emit movedLeft(m_state.speed);
#if DEBUG_MOVE
			qDebug() << "Moved Left at speed" << m_state.speed;
#endif
			break;
		case Direction::UpLeft:
			emit movedUpLeft(m_state.speed);
#if DEBUG_MOVE
			qDebug() << "Moved UpLeft at speed" << m_state.speed;
#endif
			break;
		case Direction::None:
		default:
			emit stoppedMoving();
#if DEBUG_MOVE
			qDebug() << "Stopped Moving";
#endif
		}

		m_prevState.direction = m_state.direction;
		m_prevState.speed = m_state.speed;
	}

	if (m_state.zoom != m_prevState.zoom) {
		if (m_state.zoom > 0) {
			emit zoomedIn(m_state.zoom);
#if DEBUG_ZOOM
			qDebug() << "Zoomed In at speed" << m_state.zoom;
#endif
		} else if (m_state.zoom < 0) {
			emit zoomedOut(m_state.zoom * -1);
#if DEBUG_ZOOM
			qDebug() << "Zoomed Out at speed" << m_state.zoom * -1;
#endif
		} else {
			emit stoppedZooming();
#if DEBUG_ZOOM
			qDebug() << "Stopped Zooming";
#endif
		}

		m_prevState.zoom = m_state.zoom;
	}

	update();
}

void JoystickWidget::setMousePos(QPointF position)
{
	QLineF limitLine(centerPoint(), position);
	if (limitLine.length() > m_maxDistFromCenter)
		limitLine.setLength(m_maxDistFromCenter);
	m_mousePos = limitLine.p2();
}

void JoystickWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		m_shouldBindToMousePos =
			QLineF(centerPoint(), event->pos()).length() <=
			m_maxDistFromCenter;
		setMousePos(event->pos());
		emitChanges();
	} else if (event->button() == Qt::MiddleButton) {
		m_scrollWheelPos = 0;
		emitChanges();
	} else if (event->button() == Qt::RightButton) {
		m_shouldBindToMousePos = false;
		setMousePos(event->pos());
		m_scrollWheelPos = 0;
		emitChanges();
	}
}

void JoystickWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		m_shouldBindToMousePos = false;
		setMousePos(event->pos());
		emitChanges();
	}
}

void JoystickWidget::mouseMoveEvent(QMouseEvent *event)
{
	setMousePos(event->pos());
	emitChanges();
}

void JoystickWidget::wheelEvent(QWheelEvent *event)
{
	m_scrollWheelPos += (float)event->angleDelta().y() / 8;
	m_zoomDirection = (m_scrollWheelPos < 0 ? -1 : 1);
	if (std::abs(m_scrollWheelPos) > m_zoomSpeedSteps.first().deg)
		m_scrollWheelPos =
			m_zoomSpeedSteps.first().deg * m_zoomDirection;
	emitChanges();
}

void JoystickWidget::paintEvent(QPaintEvent * /*event*/)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	QColor primary = palette().text().color();
	QColor secondary = palette().window().color();
	secondary = (secondary.lightness() > 128) ? (secondary.darker(115))
						  : (secondary.lighter(140));

	if (m_scrollWheelPos) {
		painter.setPen(
			QPen(m_scrollWheelPos > 0 ? Qt::green : Qt::red, 2));
		painter.drawArc(m_moveSpeedSteps.first().bounds.adjusted(-2, -2,
									 2, 2),
				90 * 16,
				(m_zoomArcSpan * std::abs(m_scrollWheelPos)) /
					m_zoomSpeedSteps.first().deg *
					m_zoomDirection * -16);
	}

	painter.setPen(primary);

	QLineF line;
	line.setP1(centerPoint());
	line.setLength(m_maxDistFromCenter + 4);
	line.setAngle(90);
	painter.drawLine(line);

	for (int i = 0; i < m_zoomSpeedSteps.length(); i++) {
		line.setAngle(90 + m_zoomSpeedSteps.at(i).dispDeg);
		painter.drawLine(line);
		line.setAngle(90 - m_zoomSpeedSteps.at(i).dispDeg);
		painter.drawLine(line);
	}

	for (int i = 0; i < (m_moveSpeedSteps.length() - 1); i++) {
		painter.setBrush(m_moveSpeedSteps.at(i).color);
		painter.drawEllipse(m_moveSpeedSteps.at(i).bounds);
	}

	line.setLength(m_maxDistFromCenter);
	float angles[8] = {337.5, 292.5, 247.5, 202.5,
			   157.5, 112.5, 67.5,  22.5};
	for (int i = 0; i < 8; i++) {
		line.setAngle(angles[i]);
		painter.drawLine(line);
	}

	painter.setBrush(m_moveSpeedSteps.last().color);
	painter.drawEllipse(m_moveSpeedSteps.last().bounds);

	painter.setBrush(secondary);
	painter.drawEllipse(centerCircleBounds());
}

void JoystickWidget::resizeEvent(QResizeEvent * /*event*/)
{
	int width = size().width();
	int height = size().height();
	int offset = centerCircleBounds().width() + 2;
	m_maxDistFromCenter = ((width > height ? height : width) - offset) / 2;
	updateMoveSpeedSteps();
	updateZoomSpeedSteps();
}

QSize JoystickWidget::minimumSizeHint() const
{
	QSize size;
	size.setWidth(m_maxDistFromCenter);
	size.setHeight(m_maxDistFromCenter);
	return size;
}
