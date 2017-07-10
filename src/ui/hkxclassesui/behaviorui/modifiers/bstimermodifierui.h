#ifndef BSTIMERMODIFIERUI_H
#define BSTIMERMODIFIERUI_H

#include <QGroupBox>

#include "src/utility.h"

class HkxObject;
class QGridLayout;
class TableWidget;
class BSTimerModifier;
class LineEdit;
class QLineEdit;
class DoubleSpinBox;
class CheckBox;
class ComboBox;
class GenericTableWidget;
class hkbVariableBindingSet;
class QuadVariableWidget;

class BSTimerModifierUI: QGroupBox
{
    Q_OBJECT
    friend class HkDataUI;
public:
    BSTimerModifierUI();
    virtual ~BSTimerModifierUI(){}
    void loadData(HkxObject *data);
signals:
    void viewVariables(int index);
    void viewProperties(int index);
    void viewEvents(int index);
    void modifierNameChanged(const QString & newName, int index);
private slots:
    void setName();
    void setEnable();
    void setAlarmTimeSeconds();
    void setAlarmEventId(int index, const QString & name);
    void setAlarmEventPayload();
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void connectSignals();
    void disconnectSignals();
    void connectToTables(GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *events);
    void variableRenamed(const QString & name, int index);
    void eventRenamed(const QString & name, int index);
    void selectTableToView(bool viewproperties, const QString & path);
    bool setBinding(int index, int row, const QString & variableName, const QString & path, hkVariableType type, bool isProperty);
    void loadBinding(int row, int colunm, hkbVariableBindingSet *varBind, const QString & path);
private:
    static QStringList headerLabels;
    BSTimerModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    DoubleSpinBox *alarmTimeSeconds;
    QLineEdit *alarmEventPayload;
};

#endif // BSTIMERMODIFIERUI_H