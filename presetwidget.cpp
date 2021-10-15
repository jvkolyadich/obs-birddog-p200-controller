#include "presetwidget.hpp"

#include "locale.h"

PresetWidget::PresetWidget(QWidget *parent) : QWidget(parent)
{
	m_gridLayout = new QGridLayout(this);

	m_rows = 4;
	m_columns = 3;

	setupLayout();
}

void PresetWidget::setupLayout()
{
	for (int i = 0; i < m_rows; i++) {
		for (int j = 0; j < m_columns; j++) {
			QString name(LOCALE("Preset") + " ");
			name.append(QString::number((m_columns * i) + (j + 1)));
			PresetButton *presetBtn = new PresetButton(
				((m_columns * i) + j), name, this);
			connect(presetBtn, SIGNAL(clickedPreset(int)), this,
				SLOT(emitPreset(int)));
			m_gridLayout->addWidget(presetBtn, (i + 1), (j + 1));
		}
	}
	m_storePosBtn = new QPushButton(LOCALE("StorePosition"), this);
	m_storePosBtn->setCheckable(true);
	m_gridLayout->addWidget(m_storePosBtn, (m_rows + 1), 1, 1, m_columns);
}

void PresetWidget::clearLayout()
{
	while (QLayoutItem *item = m_gridLayout->takeAt(0)) {
		if (QWidget *widget = item->widget())
			widget->deleteLater();
		delete item;
	}
}

void PresetWidget::emitPreset(int preset)
{
	if (m_storePosBtn->isChecked()) {
		emit presetSet(preset);
		m_storePosBtn->setChecked(false);
#if DEBUG_PRESETS
		qDebug() << "Set preset" << preset + 1;
#endif
	} else {
		emit presetRecalled(preset);
#if DEBUG_PRESETS
		qDebug() << "Recalled preset" << preset + 1;
#endif
	}
}
