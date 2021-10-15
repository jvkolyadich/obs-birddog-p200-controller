#include "settingsdialog.hpp"

#include "obs.h"
#include "util/config-file.h"
#include "util/platform.h"
#include "locale.h"

SettingsDialog::SettingsDialog(QWidget *parent, Qt::WindowFlags f)
	: QWidget(parent, f)
{
	setWindowTitle(LOCALE("ControllerSettings"));
	setMinimumSize(500, 600);

	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QGroupBox *connectionGroup = new QGroupBox(this);
	connectionGroup->setTitle(LOCALE("ConnectionSettings"));

	QGridLayout *connectionLayout = new QGridLayout(connectionGroup);

	QLabel *ipLbl = new QLabel(LOCALE("IPAddress") + ": ", connectionGroup);
	QString ipRange = "(?:[0-1]|1[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
	m_ipRegex = QRegExp("^" + ipRange + "\\." + ipRange + "\\." + ipRange +
			    "\\." + ipRange + "$");
	QRegExpValidator *ipValidator =
		new QRegExpValidator(m_ipRegex, connectionGroup);
	m_ipField = new QLineEdit(connectionGroup);
	m_ipField->setValidator(ipValidator);
	connect(m_ipField, SIGNAL(textChanged(QString)), this,
		SLOT(onIpChanged(QString)));

	QLabel *portLbl = new QLabel(LOCALE("Port") + ": ", connectionGroup);
	m_portRegex = QRegExp(
		"^(0|[1-9][0-9]{0,3}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5])$");
	QRegExpValidator *portValidator =
		new QRegExpValidator(m_portRegex, connectionGroup);
	m_portField = new QLineEdit(connectionGroup);
	m_portField->setValidator(portValidator);
	connect(m_portField, SIGNAL(textChanged(QString)), this,
		SLOT(onPortChanged(QString)));

	connectionLayout->addWidget(ipLbl, 1, 1);
	connectionLayout->addWidget(m_ipField, 1, 2);

	connectionLayout->addWidget(portLbl, 2, 1);
	connectionLayout->addWidget(m_portField, 2, 2);

	mainLayout->addWidget(connectionGroup);

	QGroupBox *joystickGroup = new QGroupBox(this);
	joystickGroup->setTitle(LOCALE("JoystickSettings"));

	QGridLayout *joystickLayout = new QGridLayout(joystickGroup);

	QLabel *moveAdjLbl =
		new QLabel(LOCALE("MovementAdjustment") + ": ", joystickGroup);
	m_moveAdjSlider = new QSlider(Qt::Horizontal, joystickGroup);
	m_moveAdjSlider->setRange(100, 400);
	connect(m_moveAdjSlider, SIGNAL(valueChanged(int)), this,
		SLOT(onMoveAdjChanged(int)));

	QLabel *moveAdjWeightLbl = new QLabel(
		LOCALE("MovementAdjustmentWeight") + ": ", joystickGroup);
	m_moveAdjWeightSlider = new QSlider(Qt::Horizontal, joystickGroup);
	m_moveAdjWeightSlider->setRange(0, 100);
	connect(m_moveAdjWeightSlider, SIGNAL(valueChanged(int)), this,
		SLOT(onMoveAdjWeightChanged(int)));

	QLabel *zoomAdjLbl =
		new QLabel(LOCALE("ZoomAdjustment") + ": ", joystickGroup);
	m_zoomAdjSlider = new QSlider(Qt::Horizontal, joystickGroup);
	m_zoomAdjSlider->setRange(100, 200);
	connect(m_zoomAdjSlider, SIGNAL(valueChanged(int)), this,
		SLOT(onZoomAdjChanged(int)));

	QLabel *zoomAdjWeightLbl = new QLabel(
		LOCALE("ZoomAdjustmentWeight") + ": ", joystickGroup);
	m_zoomAdjWeightSlider = new QSlider(Qt::Horizontal, joystickGroup);
	m_zoomAdjWeightSlider->setRange(0, 100);
	connect(m_zoomAdjWeightSlider, SIGNAL(valueChanged(int)), this,
		SLOT(onZoomAdjWeightChanged(int)));

	QLabel *degPerZoomLbl =
		new QLabel(LOCALE("ZoomSensitivity") + ": ", joystickGroup);
	m_degPerZoomSlider = new QSlider(Qt::Horizontal, joystickGroup);
	m_degPerZoomSlider->setRange(1, 90);
	m_degPerZoomSlider->setInvertedAppearance(true);
	connect(m_degPerZoomSlider, SIGNAL(valueChanged(int)), this,
		SLOT(onDegPerZoomChanged(int)));

	QLabel *zoomArcSpanLbl =
		new QLabel(LOCALE("ZoomIndicatorRadius") + ": ", joystickGroup);
	m_zoomArcSpanSlider = new QSlider(Qt::Horizontal, joystickGroup);
	m_zoomArcSpanSlider->setTickInterval(1);
	m_zoomArcSpanSlider->setRange(22, 180);
	connect(m_zoomArcSpanSlider, SIGNAL(valueChanged(int)), this,
		SLOT(onZoomArcSpanChanged(int)));

	joystickLayout->addWidget(moveAdjLbl, 1, 1);
	joystickLayout->addWidget(m_moveAdjSlider, 1, 2);

	joystickLayout->addWidget(moveAdjWeightLbl, 2, 1);
	joystickLayout->addWidget(m_moveAdjWeightSlider, 2, 2);

	joystickLayout->addWidget(zoomAdjLbl, 3, 1);
	joystickLayout->addWidget(m_zoomAdjSlider, 3, 2);

	joystickLayout->addWidget(zoomAdjWeightLbl, 4, 1);
	joystickLayout->addWidget(m_zoomAdjWeightSlider, 4, 2);

	joystickLayout->addWidget(degPerZoomLbl, 5, 1);
	joystickLayout->addWidget(m_degPerZoomSlider, 5, 2);

	joystickLayout->addWidget(zoomArcSpanLbl, 6, 1);
	joystickLayout->addWidget(m_zoomArcSpanSlider, 6, 2);

	mainLayout->addWidget(joystickGroup);

	QGroupBox *powerGroup = new QGroupBox(this);
	powerGroup->setTitle(LOCALE("PowerSettings"));

	QVBoxLayout *powerLayout = new QVBoxLayout(powerGroup);

	m_autoPowerOn = new QCheckBox(LOCALE("AutoPowerOn"), powerGroup);
	connect(m_autoPowerOn, SIGNAL(stateChanged(int)), this,
		SLOT(onAutoPowerChanged()));

	m_autoPowerOff = new QCheckBox(LOCALE("AutoPowerOff"), powerGroup);
	connect(m_autoPowerOff, SIGNAL(stateChanged(int)), this,
		SLOT(onAutoPowerChanged()));

	powerLayout->addWidget(m_autoPowerOn);
	powerLayout->addWidget(m_autoPowerOff);

	mainLayout->addWidget(powerGroup);

	QGroupBox *presetGroup = new QGroupBox(this);
	presetGroup->setTitle(LOCALE("PresetSettings"));

	QGridLayout *presetLayout = new QGridLayout(presetGroup);

	QLabel *presetRowsLbl =
		new QLabel(LOCALE("PresetRows") + ": ", presetGroup);
	m_presetRowsBox = new QSpinBox(presetGroup);
	m_presetRowsBox->setRange(2, 5);
	m_presetRowsBox->setValue(4);
	connect(m_presetRowsBox, SIGNAL(valueChanged(int)), this,
		SLOT(onPresetRowsChanged(int)));

	QLabel *presetColumnsLbl =
		new QLabel(LOCALE("PresetColumns") + ": ", presetGroup);
	m_presetColumnsBox = new QSpinBox(presetGroup);
	m_presetColumnsBox->setRange(1, 5);
	m_presetColumnsBox->setValue(3);
	connect(m_presetColumnsBox, SIGNAL(valueChanged(int)), this,
		SLOT(onPresetColumnsChanged(int)));

	presetLayout->addWidget(presetRowsLbl, 1, 1);
	presetLayout->addWidget(m_presetRowsBox, 1, 2);

	presetLayout->addWidget(presetColumnsLbl, 2, 1);
	presetLayout->addWidget(m_presetColumnsBox, 2, 2);

	mainLayout->addWidget(presetGroup);

	QDialogButtonBox *closeBtns = new QDialogButtonBox(
		QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	connect(closeBtns, &QDialogButtonBox::accepted, this,
		&SettingsDialog::onOkClicked);
	connect(closeBtns, &QDialogButtonBox::rejected, this,
		&SettingsDialog::onCancelClicked);

	mainLayout->addWidget(closeBtns);

	setFocus();
}

void SettingsDialog::onOkClicked()
{
#if DEBUG_SETTINGS
	qDebug() << "OK pressed";
#endif
	saveSettings();
	close();
};

void SettingsDialog::onCancelClicked()
{
#if DEBUG_SETTINGS
	qDebug() << "Cancel pressed";
#endif
	loadSettings();
	close();
};

void SettingsDialog::loadSettings()
{
	char *file = obs_module_config_path("config.json");
	obs_data_t *data = nullptr;
	if (file) {
		data = obs_data_create_from_json_file(file);
		bfree(file);
	}
	if (data) {
		m_ipField->setText(obs_data_get_string(data, "ip"));
		m_portField->setText(obs_data_get_string(data, "port"));

		m_moveAdjSlider->setValue(obs_data_get_int(data, "moveAdj"));
		m_moveAdjWeightSlider->setValue(
			obs_data_get_int(data, "moveAdjWeight"));
		m_zoomAdjSlider->setValue(obs_data_get_int(data, "zoomAdj"));
		m_zoomAdjWeightSlider->setValue(
			obs_data_get_int(data, "zoomAdjWeight"));
		m_degPerZoomSlider->setValue(
			obs_data_get_int(data, "degPerZoom"));
		m_zoomArcSpanSlider->setValue(
			obs_data_get_int(data, "zoomArcSpan"));

		m_autoPowerOn->setChecked(
			obs_data_get_bool(data, "autoPowerOn"));
		m_autoPowerOff->setChecked(
			obs_data_get_bool(data, "autoPowerOff"));

		m_presetRowsBox->setValue(obs_data_get_int(data, "presetRows"));
		m_presetColumnsBox->setValue(
			obs_data_get_int(data, "presetColumns"));

		obs_data_release(data);
	} else {
		m_ipField->setText("127.0.0.1");
		m_portField->setText("52381");

		m_moveAdjSlider->setValue(300);
		m_moveAdjWeightSlider->setValue(85);
		m_zoomAdjSlider->setValue(0);
		m_zoomAdjWeightSlider->setValue(0);
		m_degPerZoomSlider->setValue(15);
		m_zoomArcSpanSlider->setValue(150);

		m_autoPowerOn->setChecked(false);
		m_autoPowerOff->setChecked(false);

		m_presetRowsBox->setValue(4);
		m_presetColumnsBox->setValue(3);
	}

	if (m_isInitialLoad) {
		if (m_autoPowerOn->isChecked())
			emit autoPowerOn();
		m_isInitialLoad = false;
	}

	m_needsToSave = false;
#if DEBUG_SETTINGS
	qDebug() << "Settings loaded";
#endif
}

void SettingsDialog::saveSettings()
{
	char *file = obs_module_config_path("config.json");
	if (file) {
		obs_data_t *data = obs_data_create_from_json_file(file);
		if (!data)
			data = obs_data_create();

		obs_data_set_string(data, "ip",
				    m_ipField->text().toUtf8().constData());
		obs_data_set_string(data, "port",
				    m_portField->text().toUtf8().constData());

		obs_data_set_int(data, "moveAdj", m_moveAdjSlider->value());
		obs_data_set_int(data, "moveAdjWeight",
				 m_moveAdjWeightSlider->value());
		obs_data_set_int(data, "zoomAdj", m_zoomAdjSlider->value());
		obs_data_set_int(data, "zoomAdjWeight",
				 m_zoomAdjWeightSlider->value());
		obs_data_set_int(data, "degPerZoom",
				 m_degPerZoomSlider->value());
		obs_data_set_int(data, "zoomArcSpan",
				 m_zoomArcSpanSlider->value());

		obs_data_set_bool(data, "autoPowerOn",
				  m_autoPowerOn->isChecked());
		obs_data_set_bool(data, "autoPowerOff",
				  m_autoPowerOff->isChecked());

		obs_data_set_int(data, "presetRows", m_presetRowsBox->value());
		obs_data_set_int(data, "presetColumns",
				 m_presetColumnsBox->value());

		if (!obs_data_save_json(data, file)) {
			char *path = obs_module_config_path("");
			if (path) {
				os_mkdirs(path);
				bfree(path);
			}
			obs_data_save_json(data, file);
		}
		obs_data_release(data);
		bfree(file);
	}

	m_needsToSave = false;
#if DEBUG_SETTINGS
	qDebug() << "Settings saved";
#endif
}

void SettingsDialog::closeEvent(QCloseEvent *event)
{
	if (m_needsToSave) {
		QMessageBox::StandardButton msgBox;
		msgBox = QMessageBox::question(
			this, LOCALE("SaveSettings"), LOCALE("SaveSettingsMsg"),
			QMessageBox::Yes | QMessageBox::No |
				QMessageBox::Cancel);
		if (msgBox == QMessageBox::Yes) {
			saveSettings();
			event->accept();
		} else if (msgBox == QMessageBox::No) {
			loadSettings();
			event->accept();
		} else {
			event->ignore();
		}
	} else {
		event->accept();
	}
}

void SettingsDialog::sendPowerOff()
{
	if (m_autoPowerOff->isChecked())
		emit autoPowerOff();
};
