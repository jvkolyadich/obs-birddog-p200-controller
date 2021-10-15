#ifndef PRESETWIDGET_H
#define PRESETWIDGET_H

#define DEBUG_PRESETS false

#include <QWidget>
#include <QPushButton>
#include <QGridLayout>

#if DEBUG_PRESETS
#include <QDebug>
#endif

class PresetButton : public QPushButton {
	Q_OBJECT
public:
	PresetButton(int presetNum, QString name, QWidget *parent = nullptr)
		: QPushButton(name, parent)
	{
		m_preset = presetNum;
		connect(this, SIGNAL(clicked()), this,
			SLOT(emitClickedPreset()));
	};

public slots:
	void emitClickedPreset() { emit clickedPreset(m_preset); };

signals:
	void clickedPreset(int preset);

private:
	int m_preset;
};

class PresetWidget : public QWidget {
	Q_OBJECT
public:
	PresetWidget(QWidget *parent = nullptr);

public slots:
	void emitPreset(int preset);
	void setRows(int rows)
	{
		clearLayout();
		m_rows = rows;
		setupLayout();
	};
	void setColumns(int columns)
	{
		clearLayout();
		m_columns = columns;
		setupLayout();
	};

signals:
	void presetSet(unsigned short preset);
	void presetRecalled(unsigned short preset);

private:
	QGridLayout *m_gridLayout;
	QPushButton *m_storePosBtn;
	int m_rows;
	int m_columns;

	void setupLayout();
	void clearLayout();
};

#endif // PRESETWIDGET_H
