#ifndef DAMPINGMODIFIERUI_H
#define DAMPINGMODIFIERUI_H

#include <QGroupBox>

#include "src/utility.h"

class HkxObject;
class QGridLayout;
class TableWidget;
class hkbDampingModifier;
class LineEdit;
class DoubleSpinBox;
class CheckBox;
class ComboBox;
class GenericTableWidget;
class hkbVariableBindingSet;
class QuadVariableWidget;
class SpinBox;

class DampingModifierUI: QGroupBox
{
    Q_OBJECT
    friend class HkDataUI;
public:
    DampingModifierUI();
    virtual ~DampingModifierUI(){}
    void loadData(HkxObject *data);
signals:
    void viewVariables(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void viewProperties(int index, const QString & typeallowed, const QStringList &typesdisallowed);
    void modifierNameChanged(const QString & newName, int index);
private slots:
    void setName();
    void setEnable();
    void setKP();
    void setKI();
    void setKD();
    void setEnableScalarDamping();
    void setEnableVectorDamping();
    void setRawValue();
    void setDampedValue();
    void setRawVector();
    void setDampedVector();
    void setVecErrorSum();
    void setVecPreviousError();
    void setErrorSum();
    void setPreviousError();
    void viewSelected(int row, int column);
    void setBindingVariable(int index, const QString & name);
private:
    void connectSignals();
    void disconnectSignals();
    void connectToTables(GenericTableWidget *variables, GenericTableWidget *properties);
    void variableRenamed(const QString & name, int index);
    void selectTableToView(bool viewproperties, const QString & path);
    bool setBinding(int index, int row, const QString & variableName, const QString & path, hkVariableType type, bool isProperty);
    void loadBinding(int row, int colunm, hkbVariableBindingSet *varBind, const QString & path);
private:
    static QStringList headerLabels;
    hkbDampingModifier *bsData;
    QGridLayout *topLyt;
    TableWidget *table;
    LineEdit *name;
    CheckBox *enable;
    DoubleSpinBox *kP;
    DoubleSpinBox *kI;
    DoubleSpinBox *kD;
    CheckBox *enableScalarDamping;
    CheckBox *enableVectorDamping;
    DoubleSpinBox *rawValue;
    DoubleSpinBox *dampedValue;
    QuadVariableWidget *rawVector;
    QuadVariableWidget *dampedVector;
    QuadVariableWidget *vecErrorSum;
    QuadVariableWidget *vecPreviousError;
    DoubleSpinBox *errorSum;
    DoubleSpinBox *previousError;
};

#endif // DAMPINGMODIFIERUI_H
