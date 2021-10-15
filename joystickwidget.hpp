#ifndef JOYSTICKWIDGET_H
#define JOYSTICKWIDGET_H

#define DEBUG_MOVE false
#define DEBUG_ZOOM false

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <cmath>

#if DEBUG_MOVE || DEBUG_ZOOM
#include <QDebug>
#endif

class JoystickWidget : public QWidget {
	Q_OBJECT
public:
	JoystickWidget(QWidget *parent = nullptr);

public slots:
	void setMoveAdjustment(float adjustment)
	{
		m_moveAdjustment = adjustment;
		updateMoveSpeedSteps();
	};
	void setMoveAdjWeight(float weight)
	{
		m_moveAdjustmentWeight = weight;
		updateMoveSpeedSteps();
	};
	void setZoomAdjustment(float adjustment)
	{
		m_zoomAdjustment = adjustment;
		updateZoomSpeedSteps();
	};
	void setZoomAdjWeight(float weight)
	{
		m_zoomAdjustmentWeight = weight;
		updateZoomSpeedSteps();
	};
	void setDegPerZoom(int degPerZoom)
	{
		m_degPerZoom = degPerZoom;
		updateZoomSpeedSteps();
		if (std::abs(m_scrollWheelPos) > m_zoomSpeedSteps.first().deg) {
			m_scrollWheelPos =
				m_zoomSpeedSteps.first().deg * m_zoomDirection;
			update();
		}
	};
	void setZoomArcSpan(int zoomArcSpan)
	{
		m_zoomArcSpan = zoomArcSpan;
		updateZoomSpeedSteps();
	};

protected:
	void paintEvent(QPaintEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;
	void resizeEvent(QResizeEvent *event) override;
	QSize minimumSizeHint() const override;

signals:
	void stoppedMoving();
	void movedUp(unsigned short speed);
	void movedUpRight(unsigned short speed);
	void movedRight(unsigned short speed);
	void movedDownRight(unsigned short speed);
	void movedDown(unsigned short speed);
	void movedDownLeft(unsigned short speed);
	void movedLeft(unsigned short speed);
	void movedUpLeft(unsigned short speed);

	void stoppedZooming();
	void zoomedIn(unsigned short speed);
	void zoomedOut(unsigned short speed);

private:
	enum Direction {
		None,
		Up,
		UpRight,
		Right,
		DownRight,
		Down,
		DownLeft,
		Left,
		UpLeft
	};
	struct State {
		Direction direction;
		int speed;
		int zoom;
	};
	struct MoveSpeedStep {
		int speed;
		float radius;
		QRectF bounds;
		QColor color;
	};

	struct ZoomSpeedStep {
		int speed;
		float deg;
		float dispDeg;
	};

	State m_state;
	State m_prevState;
	QPointF m_mousePos;
	float m_scrollWheelPos;
	float m_maxDistFromCenter;
	bool m_shouldBindToMousePos;
	int m_maxMoveSpeed;
	int m_maxZoomSpeed;
	float m_moveAdjustment;
	float m_moveAdjustmentWeight;
	float m_zoomAdjustment;
	float m_zoomAdjustmentWeight;
	int m_zoomDirection;
	int m_degPerZoom;
	float m_zoomArcSpan;
	QVector<MoveSpeedStep> m_moveSpeedSteps;
	QVector<ZoomSpeedStep> m_zoomSpeedSteps;

	QPointF centerPoint();
	QRectF centerCircleBounds();
	float calcMoveSpeedStepAt(int index);
	float calcZoomSpeedStepAt(int index);
	void updateMoveSpeedSteps();
	void updateZoomSpeedSteps();
	void updateState();
	void emitChanges();
	void setMousePos(QPointF position);

	void setMaxMoveSpeed(int speed) { m_maxMoveSpeed = speed + 1; };
	void setMaxZoomSpeed(int speed) { m_maxZoomSpeed = speed + 1; };
};

#endif // JOYSTICKWIDGET_H
