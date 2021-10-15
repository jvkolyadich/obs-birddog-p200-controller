#pragma once
#include <QDockWidget>
#include <QGridLayout>

#include "joystickwidget.hpp"
#include "settingsdialog.hpp"
#include "controller.hpp"
#include "presetwidget.hpp"

class PTZControllerDock : public QDockWidget {
	Q_OBJECT
public:
	PTZControllerDock(QWidget *parent = nullptr);
	~PTZControllerDock();

private:
	SettingsDialog *m_settingsDialog;
};
