#include "timermodifierui.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/hkbstringeventpayload.h"
#include "src/hkxclasses/behavior/modifiers/hkbtimermodifier.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/genericdatawidgets.h"

#include <QGridLayout>
#include <QHeaderView>

#define BASE_NUMBER_OF_ROWS 5

#define NAME_ROW 0
#define ENABLE_ROW 1
#define ALARM_TIME_SECONDS_ROW 2
#define ALARM_EVENT_ID_ROW 3
#define ALARM_EVENT_PAYLOAD_ROW 4

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define BINDING_COLUMN 2
#define VALUE_COLUMN 3

#define BINDING_ITEM_LABEL QString("Use Property     ")

QStringList TimerModifierUI::headerLabels = {
    "Name",
    "Type",
    "Bound Variable",
    "Value"
};

TimerModifierUI::TimerModifierUI()
    : bsData(NULL),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      enable(new CheckBox),
      alarmTimeSeconds(new DoubleSpinBox),
      alarmEventPayload(new QLineEdit)
{
    setTitle("TimerModifierUI");
    table->setRowCount(BASE_NUMBER_OF_ROWS);
    table->setColumnCount(headerLabels.size());
    table->setHorizontalHeaderLabels(headerLabels);
    table->setItem(NAME_ROW, NAME_COLUMN, new TableWidgetItem("name"));
    table->setItem(NAME_ROW, TYPE_COLUMN, new TableWidgetItem("hkStringPtr", Qt::AlignCenter));
    table->setItem(NAME_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(NAME_ROW, VALUE_COLUMN, name);
    table->setItem(ENABLE_ROW, NAME_COLUMN, new TableWidgetItem("enable"));
    table->setItem(ENABLE_ROW, TYPE_COLUMN, new TableWidgetItem("hkBool", Qt::AlignCenter));
    table->setItem(ENABLE_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ENABLE_ROW, VALUE_COLUMN, enable);
    table->setItem(ALARM_TIME_SECONDS_ROW, NAME_COLUMN, new TableWidgetItem("alarmTimeSeconds"));
    table->setItem(ALARM_TIME_SECONDS_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setItem(ALARM_TIME_SECONDS_ROW, BINDING_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_VARIABLES_TABLE_TIP, true));
    table->setCellWidget(ALARM_TIME_SECONDS_ROW, VALUE_COLUMN, alarmTimeSeconds);
    table->setItem(ALARM_EVENT_ID_ROW, NAME_COLUMN, new TableWidgetItem("alarmEventId"));
    table->setItem(ALARM_EVENT_ID_ROW, TYPE_COLUMN, new TableWidgetItem("hkInt32", Qt::AlignCenter));
    table->setItem(ALARM_EVENT_ID_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setItem(ALARM_EVENT_ID_ROW, VALUE_COLUMN, new TableWidgetItem(BINDING_ITEM_LABEL+"NONE", Qt::AlignLeft | Qt::AlignVCenter, QColor(Qt::lightGray), QBrush(Qt::black), VIEW_EVENTS_TABLE_TIP));
    table->setItem(ALARM_EVENT_PAYLOAD_ROW, NAME_COLUMN, new TableWidgetItem("alarmEventPayload"));
    table->setItem(ALARM_EVENT_PAYLOAD_ROW, TYPE_COLUMN, new TableWidgetItem("hkEventPayload", Qt::AlignCenter));
    table->setItem(ALARM_EVENT_PAYLOAD_ROW, BINDING_COLUMN, new TableWidgetItem("N/A", Qt::AlignCenter));
    table->setCellWidget(ALARM_EVENT_PAYLOAD_ROW, VALUE_COLUMN, alarmEventPayload);
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
}

void TimerModifierUI::connectSignals(){
    connect(name, SIGNAL(editingFinished()), this, SLOT(setName()), Qt::UniqueConnection);
    connect(enable, SIGNAL(released()), this, SLOT(setEnable()), Qt::UniqueConnection);
    connect(alarmTimeSeconds, SIGNAL(editingFinished()), this, SLOT(setAlarmTimeSeconds()), Qt::UniqueConnection);
    connect(alarmEventPayload, SIGNAL(editingFinished()), this, SLOT(setAlarmEventPayload()), Qt::UniqueConnection);
    connect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)), Qt::UniqueConnection);
}

void TimerModifierUI::disconnectSignals(){
    disconnect(name, SIGNAL(editingFinished()), this, SLOT(setName()));
    disconnect(enable, SIGNAL(released()), this, SLOT(setEnable()));
    disconnect(alarmTimeSeconds, SIGNAL(editingFinished()), this, SLOT(setAlarmTimeSeconds()));
    disconnect(alarmEventPayload, SIGNAL(editingFinished()), this, SLOT(setAlarmEventPayload()));
    disconnect(table, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(viewSelected(int,int)));
}

void TimerModifierUI::connectToTables(GenericTableWidget *variables, GenericTableWidget *properties, GenericTableWidget *events){
    if (variables && properties && events){
        disconnect(variables, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(properties, SIGNAL(elementSelected(int,QString)), 0, 0);
        disconnect(events, SIGNAL(elementSelected(int,QString)), 0, 0);
        connect(variables, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(properties, SIGNAL(elementSelected(int,QString)), this, SLOT(setBindingVariable(int,QString)), Qt::UniqueConnection);
        connect(events, SIGNAL(elementSelected(int,QString)), this, SLOT(setAlarmEventId(int,QString)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewVariables(int)), variables, SLOT(showTable(int)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewProperties(int)), properties, SLOT(showTable(int)), Qt::UniqueConnection);
        connect(this, SIGNAL(viewEvents(int)), events, SLOT(showTable(int)), Qt::UniqueConnection);
    }else{
        CRITICAL_ERROR_MESSAGE(QString("TimerModifierUI::connectToTables(): One or more arguments are NULL!!"))
    }
}

void TimerModifierUI::loadData(HkxObject *data){
    disconnectSignals();
    if (data){
        if (data->getSignature() == HKB_TIMER_MODIFIER){
            bsData = static_cast<hkbTimerModifier *>(data);
            hkbVariableBindingSet *varBind = NULL;
            hkbStringEventPayload *payload = static_cast<hkbStringEventPayload *>(bsData->alarmEvent.payload.data());
            name->setText(bsData->name);
            enable->setChecked(bsData->enable);
            alarmTimeSeconds->setValue(bsData->alarmTimeSeconds);
            QString text = static_cast<BehaviorFile *>(bsData->getParentFile())->getEventNameAt(bsData->alarmEvent.id);
            if (text != ""){
                table->item(ALARM_EVENT_ID_ROW, VALUE_COLUMN)->setText(text);
            }else{
                table->item(ALARM_EVENT_ID_ROW, VALUE_COLUMN)->setText("None");
            }
            if (payload){
                alarmEventPayload->setText(payload->data);
            }else{
                alarmEventPayload->setText("");
            }
            varBind = static_cast<hkbVariableBindingSet *>(bsData->variableBindingSet.data());
            if (varBind){
                loadBinding(ENABLE_ROW, BINDING_COLUMN, varBind, "enable");
                loadBinding(ALARM_TIME_SECONDS_ROW, BINDING_COLUMN, varBind, "alarmTimeSeconds");
            }else{
                table->item(ENABLE_ROW, BINDING_COLUMN)->setText(BINDING_ITEM_LABEL+"NONE");
                table->item(ALARM_TIME_SECONDS_ROW, BINDING_COLUMN)->setText(BINDING_ITEM_LABEL+"NONE");
            }
        }else{
            CRITICAL_ERROR_MESSAGE(QString("TimerModifierUI::loadData(): The data is an incorrect type!!"));
        }
    }else{
        CRITICAL_ERROR_MESSAGE(QString("TimerModifierUI::loadData(): The data is NULL!!"));
    }
    connectSignals();
}

void TimerModifierUI::setName(){
    if (bsData){
        if (bsData->name != name->text()){
            bsData->name = name->text();
            static_cast<DataIconManager*>((bsData))->updateIconNames();
            bsData->getParentFile()->toggleChanged(true);
            emit modifierNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfModifier(bsData));
        }
    }else{
        CRITICAL_ERROR_MESSAGE(QString("TimerModifierUI::setName(): The data is NULL!!"));
    }
}

void TimerModifierUI::setEnable(){
    if (bsData){
        if (bsData->enable != enable->isChecked()){
            bsData->enable = enable->isChecked();
            bsData->getParentFile()->toggleChanged(true);
        }
    }else{
        CRITICAL_ERROR_MESSAGE(QString("TimerModifierUI::setEnable(): The data is NULL!!"));
    }
}

void TimerModifierUI::setAlarmTimeSeconds(){
    if (bsData){
        if (bsData->alarmTimeSeconds != alarmTimeSeconds->value()){
            bsData->alarmTimeSeconds = alarmTimeSeconds->value();
            bsData->getParentFile()->toggleChanged(true);
        }
    }else{
        CRITICAL_ERROR_MESSAGE(QString("TimerModifierUI::setalarmTimeSeconds(): The data is NULL!!"));
    }
}

void TimerModifierUI::setAlarmEventId(int index, const QString & name){
    if (bsData){
        index--;
        if (bsData->alarmEvent.id != index){
            bsData->alarmEvent.id = index;
            table->item(ALARM_EVENT_ID_ROW, VALUE_COLUMN)->setText(name);
            bsData->getParentFile()->toggleChanged(true);
        }
    }else{
        CRITICAL_ERROR_MESSAGE(QString("TimerModifierUI::setAlarmEventId(): The data is NULL!!"));
    }
}

void TimerModifierUI::setAlarmEventPayload(){
    hkbStringEventPayload *payload;
    if (bsData){
        payload = static_cast<hkbStringEventPayload *>(bsData->alarmEvent.payload.data());
        if (alarmEventPayload->text() != ""){
            if (payload){
                payload->data = alarmEventPayload->text();
            }else{
                payload = new hkbStringEventPayload(bsData->getParentFile(), alarmEventPayload->text());
                bsData->getParentFile()->addObjectToFile(payload, -1);
                bsData->alarmEvent.payload = HkxSharedPtr(payload);
            }
        }else{
            bsData->alarmEvent.payload = HkxSharedPtr();
        }
        bsData->getParentFile()->toggleChanged(true);
    }else{
        CRITICAL_ERROR_MESSAGE(QString("TimerModifierUI::setalarmEventPayload(): The data is NULL!!"));
    }
}

void TimerModifierUI::viewSelected(int row, int column){
    if (bsData){
        bool isProperty = false;
        if (column == BINDING_COLUMN){
            switch (row){
            case ENABLE_ROW:
                if (table->item(ENABLE_ROW, BINDING_COLUMN)->checkState() != Qt::Unchecked){
                    isProperty = true;
                }
                selectTableToView(isProperty, "enable");
                break;
            case ALARM_TIME_SECONDS_ROW:
                if (table->item(ALARM_TIME_SECONDS_ROW, BINDING_COLUMN)->checkState() != Qt::Unchecked){
                    isProperty = true;
                }
                selectTableToView(isProperty, "alarmTimeSeconds");
                break;
            default:
                return;
            }
        }else if (column == VALUE_COLUMN && row == ALARM_EVENT_ID_ROW){
            emit viewEvents(bsData->alarmEvent.id + 1);
        }
    }else{
        CRITICAL_ERROR_MESSAGE(QString("TimerModifierUI::viewSelected(): The 'bsData' pointer is NULL!!"))
    }
}

void TimerModifierUI::selectTableToView(bool viewisProperty, const QString & path){
    if (bsData){
        if (viewisProperty){
            if (bsData->variableBindingSet.data()){
                emit viewProperties(static_cast<hkbVariableBindingSet *>(bsData->variableBindingSet.data())->getVariableIndexOfBinding(path) + 1);
            }else{
                emit viewProperties(0);
            }
        }else{
            if (bsData->variableBindingSet.data()){
                emit viewVariables(static_cast<hkbVariableBindingSet *>(bsData->variableBindingSet.data())->getVariableIndexOfBinding(path) + 1);
            }else{
                emit viewVariables(0);
            }
        }
    }else{
        CRITICAL_ERROR_MESSAGE(QString("TimerModifierUI::selectTableToView(): The data is NULL!!"));
    }
}

void TimerModifierUI::eventRenamed(const QString & name, int index){
    if (bsData){
        index--;
        if (index == bsData->alarmEvent.id){
            table->item(ALARM_EVENT_ID_ROW, VALUE_COLUMN)->setText(name);
        }
    }else{
        CRITICAL_ERROR_MESSAGE(QString("TimerModifierUI::eventRenamed(): The data is NULL!!"));
    }
}

void TimerModifierUI::variableRenamed(const QString & name, int index){
    if (bsData){
        index--;
        hkbVariableBindingSet *bind = static_cast<hkbVariableBindingSet *>(bsData->variableBindingSet.data());
        if (bind){
            int bindIndex = bind->getVariableIndexOfBinding("enable");
            if (bindIndex == index){
                table->item(ENABLE_ROW, BINDING_COLUMN)->setText(name);
            }
            bindIndex = bind->getVariableIndexOfBinding("alarmTimeSeconds");
            if (bindIndex == index){
                table->item(ALARM_TIME_SECONDS_ROW, BINDING_COLUMN)->setText(name);
            }
        }
    }else{
        CRITICAL_ERROR_MESSAGE(QString("TimerModifierUI::variableRenamed(): The 'bsData' pointer is NULL!!"))
    }
}

bool TimerModifierUI::setBinding(int index, int row, const QString &variableName, const QString &path, hkVariableType type, bool isProperty){
    hkbVariableBindingSet *varBind = static_cast<hkbVariableBindingSet *>(bsData->variableBindingSet.data());
    if (bsData){
        if (index == 0){
            varBind->removeBinding(path);
            table->item(row, BINDING_COLUMN)->setText(BINDING_ITEM_LABEL+"NONE");
        }else if ((!isProperty && static_cast<BehaviorFile *>(bsData->getParentFile())->getVariableTypeAt(index - 1) == type) ||
                  (isProperty && static_cast<BehaviorFile *>(bsData->getParentFile())->getCharacterPropertyTypeAt(index - 1) == type)){
            if (!varBind){
                varBind = new hkbVariableBindingSet(bsData->getParentFile());
                bsData->variableBindingSet = HkxSharedPtr(varBind);
            }
            if (isProperty){
                if (!varBind->addBinding(path, variableName, index - 1, hkbVariableBindingSet::hkBinding::BINDING_TYPE_CHARACTER_PROPERTY)){
                    CRITICAL_ERROR_MESSAGE(QString("TimerModifierUI::setBinding(): The attempt to add a binding to this object's hkbVariableBindingSet failed!!"));
                }
            }else{
                if (!varBind->addBinding(path, variableName, index - 1, hkbVariableBindingSet::hkBinding::BINDING_TYPE_VARIABLE)){
                    CRITICAL_ERROR_MESSAGE(QString("TimerModifierUI::setBinding(): The attempt to add a binding to this object's hkbVariableBindingSet failed!!"));
                }
            }
            table->item(row, BINDING_COLUMN)->setText(BINDING_ITEM_LABEL+variableName);
            bsData->getParentFile()->toggleChanged(true);
        }else{
            WARNING_MESSAGE(QString("I'M SORRY HAL BUT I CAN'T LET YOU DO THAT.\n\nYou are attempting to bind a variable of an invalid type for this data field!!!"));
        }
    }else{
        CRITICAL_ERROR_MESSAGE(QString("TimerModifierUI::setBinding(): The data is NULL!!"));
    }
    return true;
}

void TimerModifierUI::setBindingVariable(int index, const QString &name){
    if (bsData){
        bool isProperty = false;
        int row = table->currentRow();
        switch (row){
        case ENABLE_ROW:
            if (table->item(ENABLE_ROW, BINDING_COLUMN)->checkState() != Qt::Unchecked){
                isProperty = true;
            }
            setBinding(index, row, name, "enable", VARIABLE_TYPE_BOOL, isProperty);
            break;
        case ALARM_TIME_SECONDS_ROW:
            if (table->item(ALARM_TIME_SECONDS_ROW, BINDING_COLUMN)->checkState() != Qt::Unchecked){
                isProperty = true;
            }
            setBinding(index, row, name, "alarmTimeSeconds", VARIABLE_TYPE_REAL, isProperty);
            break;
        default:
            return;
        }
        bsData->getParentFile()->toggleChanged(true);
    }else{
        CRITICAL_ERROR_MESSAGE(QString("TimerModifierUI::setBindingVariable(): The data is NULL!!"));
    }
}

void TimerModifierUI::loadBinding(int row, int colunm, hkbVariableBindingSet *varBind, const QString &path){
    if (bsData){
        if (varBind){
            int index = varBind->getVariableIndexOfBinding(path);
            QString varName;
            if (index != -1){
                if (varBind->getBindingType(path) == hkbVariableBindingSet::hkBinding::BINDING_TYPE_CHARACTER_PROPERTY){
                    varName = static_cast<BehaviorFile *>(bsData->getParentFile())->getCharacterPropertyNameAt(index, true);
                    table->item(row, colunm)->setCheckState(Qt::Checked);
                }else{
                    varName = static_cast<BehaviorFile *>(bsData->getParentFile())->getVariableNameAt(index);
                }
                if (varName == ""){
                    varName = "NONE";
                }
                table->item(row, colunm)->setText(BINDING_ITEM_LABEL+varName);
            }
        }else{
            CRITICAL_ERROR_MESSAGE(QString("TimerModifierUI::loadBinding(): The variable binding set is NULL!!"));
        }
    }else{
        CRITICAL_ERROR_MESSAGE(QString("TimerModifierUI::loadBinding(): The data is NULL!!"));
    }
}
