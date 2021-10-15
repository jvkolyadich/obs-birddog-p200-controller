#include "dockwidget.hpp"

#include <QMainWindow>

#include "obs-frontend-api.h"
#include "locale.h"

MODULE_EXPORT void load_ptz_controller_dock()
{
	const auto main_window =
		static_cast<QMainWindow *>(obs_frontend_get_main_window());
	obs_frontend_push_ui_translation(obs_module_get_string);
	obs_frontend_add_dock(new PTZControllerDock(main_window));
	obs_frontend_pop_ui_translation();
}

PTZControllerDock::PTZControllerDock(QWidget *parent) : QDockWidget(parent)
{
	setWindowTitle(LOCALE("DockName"));

	QWidget *windowWidget = new QWidget(this);

	Controller *controller = new Controller(windowWidget);

	QGridLayout *gridLayout = new QGridLayout(windowWidget);

	JoystickWidget *joystick = new JoystickWidget(windowWidget);
	connect(joystick, SIGNAL(stoppedMoving()), controller,
		SLOT(moveStop()));
	connect(joystick, SIGNAL(movedUp(unsigned short)), controller,
		SLOT(moveUp(unsigned short)));
	connect(joystick, SIGNAL(movedUpRight(unsigned short)), controller,
		SLOT(moveUpRight(unsigned short)));
	connect(joystick, SIGNAL(movedRight(unsigned short)), controller,
		SLOT(moveRight(unsigned short)));
	connect(joystick, SIGNAL(movedDownRight(unsigned short)), controller,
		SLOT(moveDownRight(unsigned short)));
	connect(joystick, SIGNAL(movedDown(unsigned short)), controller,
		SLOT(moveDown(unsigned short)));
	connect(joystick, SIGNAL(movedDownLeft(unsigned short)), controller,
		SLOT(moveDownLeft(unsigned short)));
	connect(joystick, SIGNAL(movedLeft(unsigned short)), controller,
		SLOT(moveLeft(unsigned short)));
	connect(joystick, SIGNAL(movedUpLeft(unsigned short)), controller,
		SLOT(moveUpLeft(unsigned short)));
	connect(joystick, SIGNAL(stoppedZooming()), controller,
		SLOT(zoomStop()));
	connect(joystick, SIGNAL(zoomedIn(unsigned short)), controller,
		SLOT(zoomIn(unsigned short)));
	connect(joystick, SIGNAL(zoomedOut(unsigned short)), controller,
		SLOT(zoomOut(unsigned short)));
	gridLayout->addWidget(joystick, 1, 1);

	QWidget *power = new QWidget(windowWidget);
	QGridLayout *powerLayout = new QGridLayout(power);
	QPushButton *powerOn = new QPushButton(LOCALE("PowerOn"));
	QPushButton *powerOff = new QPushButton(LOCALE("PowerOff"));
	powerLayout->addWidget(powerOn, 1, 1);
	powerLayout->addWidget(powerOff, 1, 2);
	connect(powerOn, SIGNAL(clicked()), controller, SLOT(powerOn()));
	connect(powerOff, SIGNAL(clicked()), controller, SLOT(powerOff()));
	gridLayout->addWidget(power, 2, 1);

	PresetWidget *presets = new PresetWidget(windowWidget);
	connect(presets, SIGNAL(presetSet(unsigned short)), controller,
		SLOT(presetSet(unsigned short)));
	connect(presets, SIGNAL(presetRecalled(unsigned short)), controller,
		SLOT(presetRecall(unsigned short)));
	gridLayout->addWidget(presets, 3, 1);

	QPushButton *settingsBtn = new QPushButton(this);
	settingsBtn->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	settingsBtn->setMaximumSize(22, 22);
	settingsBtn->setProperty("themeID", "configIconSmall");
	settingsBtn->setAutoDefault(false);
	settingsBtn->setFlat(true);
	gridLayout->addWidget(settingsBtn, 1, 1,
			      (Qt::AlignTop | Qt::AlignRight));

	m_settingsDialog = new SettingsDialog(windowWidget);
	connect(settingsBtn, SIGNAL(clicked()), m_settingsDialog, SLOT(show()));
	connect(m_settingsDialog, SIGNAL(changedIp(QString)), controller,
		SLOT(setIp(QString)));
	connect(m_settingsDialog, SIGNAL(changedPort(int)), controller,
		SLOT(setPort(int)));
	connect(m_settingsDialog, SIGNAL(changedMoveAdjustment(float)),
		joystick, SLOT(setMoveAdjustment(float)));
	connect(m_settingsDialog, SIGNAL(changedMoveAdjWeight(float)), joystick,
		SLOT(setMoveAdjWeight(float)));
	connect(m_settingsDialog, SIGNAL(changedZoomAdjustment(float)),
		joystick, SLOT(setZoomAdjustment(float)));
	connect(m_settingsDialog, SIGNAL(changedZoomAdjWeight(float)), joystick,
		SLOT(setZoomAdjWeight(float)));
	connect(m_settingsDialog, SIGNAL(changedDegPerZoom(int)), joystick,
		SLOT(setDegPerZoom(int)));
	connect(m_settingsDialog, SIGNAL(changedZoomArcSpan(int)), joystick,
		SLOT(setZoomArcSpan(int)));
	connect(m_settingsDialog, SIGNAL(autoPowerOn()), controller,
		SLOT(powerOn()));
	connect(m_settingsDialog, SIGNAL(autoPowerOff()), controller,
		SLOT(powerOff()));
	connect(m_settingsDialog, SIGNAL(changedPresetRows(int)), presets,
		SLOT(setRows(int)));
	connect(m_settingsDialog, SIGNAL(changedPresetColumns(int)), presets,
		SLOT(setColumns(int)));
	m_settingsDialog->loadSettings();

	setFeatures(AllDockWidgetFeatures);
	setObjectName("BirdDogP200ControllerDock");
	setFloating(true);
	hide();
	setWidget(windowWidget);
}

PTZControllerDock::~PTZControllerDock()
{
	m_settingsDialog->sendPowerOff();
};
