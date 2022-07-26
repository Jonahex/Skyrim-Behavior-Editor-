#include "bgsgamebryosequencegeneratorui.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/behavior/generators/BGSGamebryoSequenceGenerator.h"
#include "src/ui/genericdatawidgets.h"
#include "src/filetypes/behaviorfile.h"
#include "src/ui/treegraphicsitem.h"
#include "src/ui/genericdatawidgets.h"

#include <QHeaderView>

#define BASE_NUMBER_OF_ROWS 4

#define NAME_ROW 0
#define SEQUENCE_ROW 1
#define BLEND_MODE_FUNCTION_ROW 2
#define PERCENT_ROW 3

#define NAME_COLUMN 0
#define TYPE_COLUMN 1
#define VALUE_COLUMN 2

const QStringList BGSGamebryoSequenceGeneratorUI::headerLabels = {
    "Name",
    "Type",
    "Value"
};

BGSGamebryoSequenceGeneratorUI::BGSGamebryoSequenceGeneratorUI()
    : bsData(nullptr),
      topLyt(new QGridLayout),
      table(new TableWidget(QColor(Qt::white))),
      name(new LineEdit),
      pSequence(new LineEdit),
      eBlendModeFunction(new ComboBox),
      fPercent(new DoubleSpinBox)
{
    setTitle("BGSGamebryoSequenceGenerator");
    table->setRowCount(BASE_NUMBER_OF_ROWS);
    table->setColumnCount(headerLabels.size());
    table->setHorizontalHeaderLabels(headerLabels);
    table->setItem(NAME_ROW, NAME_COLUMN, new TableWidgetItem("name"));
    table->setItem(NAME_ROW, TYPE_COLUMN, new TableWidgetItem("hkStringPtr", Qt::AlignCenter));
    table->setCellWidget(NAME_ROW, VALUE_COLUMN, name);
    table->setItem(SEQUENCE_ROW, NAME_COLUMN, new TableWidgetItem("pSequence"));
    table->setItem(SEQUENCE_ROW, TYPE_COLUMN, new TableWidgetItem("hkStringPtr", Qt::AlignCenter));
    table->setCellWidget(SEQUENCE_ROW, VALUE_COLUMN, pSequence);
    table->setItem(BLEND_MODE_FUNCTION_ROW, NAME_COLUMN, new TableWidgetItem("eBlendModeFunction"));
    table->setItem(BLEND_MODE_FUNCTION_ROW, TYPE_COLUMN, new TableWidgetItem("BlendModeFunction", Qt::AlignCenter));
    table->setCellWidget(BLEND_MODE_FUNCTION_ROW, VALUE_COLUMN, eBlendModeFunction);
    table->setItem(PERCENT_ROW, NAME_COLUMN, new TableWidgetItem("fPercent"));
    table->setItem(PERCENT_ROW, TYPE_COLUMN, new TableWidgetItem("hkReal", Qt::AlignCenter));
    table->setCellWidget(PERCENT_ROW, VALUE_COLUMN, fPercent);
    topLyt->addWidget(table, 0, 0, 8, 3);
    setLayout(topLyt);
    toggleSignals(true);
}

void BGSGamebryoSequenceGeneratorUI::toggleSignals(bool toggleconnections)
{
    if (toggleconnections)
    {
        connect(name, &LineEdit::textEdited, this, &BGSGamebryoSequenceGeneratorUI::setName, Qt::UniqueConnection);
        connect(pSequence, &LineEdit::textEdited, this, &BGSGamebryoSequenceGeneratorUI::setSequence, Qt::UniqueConnection);
        connect(eBlendModeFunction, static_cast<void (ComboBox::*)(int)>(&ComboBox::currentIndexChanged), this, &BGSGamebryoSequenceGeneratorUI::setBlendModeFunction, Qt::UniqueConnection);
        connect(fPercent, &DoubleSpinBox::editingFinished, this, &BGSGamebryoSequenceGeneratorUI::setPercent, Qt::UniqueConnection);
    }
	else
    {
        disconnect(name, &LineEdit::textEdited, this, &BGSGamebryoSequenceGeneratorUI::setName);
        disconnect(pSequence, &LineEdit::textEdited, this, &BGSGamebryoSequenceGeneratorUI::setSequence);
        disconnect(eBlendModeFunction, static_cast<void (ComboBox::*)(int)>(&ComboBox::currentIndexChanged), this, &BGSGamebryoSequenceGeneratorUI::setBlendModeFunction);
        disconnect(fPercent, &DoubleSpinBox::editingFinished, this, &BGSGamebryoSequenceGeneratorUI::setPercent);
    }
}

void BGSGamebryoSequenceGeneratorUI::loadData(HkxObject *data){
    toggleSignals(false);
    if (data){
        if (data->getSignature() == BGS_GAMEBYRO_SEQUENCE_GENERATOR){
            bsData = static_cast<BGSGamebryoSequenceGenerator *>(data);
            name->setText(bsData->getName());
            pSequence->setText(bsData->getPSequence());
            (!eBlendModeFunction->count()) ? eBlendModeFunction->insertItems(0, bsData->BlendModeFunction) : NULL;
            eBlendModeFunction->setCurrentIndex(bsData->BlendModeFunction.indexOf(bsData->getEBlendModeFunction()));
            fPercent->setValue(bsData->getFPercent());
        }else{
            LogFile::writeToLog(QString("BGSGamebryoSequenceGeneratorUI::loadData(): The data passed to the UI is the wrong type!\nSIGNATURE: "+QString::number(data->getSignature(), 16)).toLocal8Bit().data());
        }
    }else{
        LogFile::writeToLog("BGSGamebryoSequenceGeneratorUI::loadData(): The data passed to the UI is nullptr!!!");
    }
    toggleSignals(true);
}

void BGSGamebryoSequenceGeneratorUI::setName(const QString & newname){
    if (bsData){
        bsData->setName(newname);
        bsData->updateIconNames();
        emit generatorNameChanged(name->text(), static_cast<BehaviorFile *>(bsData->getParentFile())->getIndexOfGenerator(bsData));
    }else{
        LogFile::writeToLog("BGSGamebryoSequenceGeneratorUI::setName(): The data is nullptr!!");
    }
}

void BGSGamebryoSequenceGeneratorUI::setSequence(const QString & sequence){
    (bsData) ? bsData->setPSequence(sequence) : LogFile::writeToLog("BGSGamebryoSequenceGeneratorUI::setSequence(): The data is nullptr!!");
}

void BGSGamebryoSequenceGeneratorUI::setBlendModeFunction(int index){
    (bsData) ? bsData->setEBlendModeFunction(index) : LogFile::writeToLog("BGSGamebryoSequenceGeneratorUI::setBlendModeFunction(): The data is nullptr!!");
}

void BGSGamebryoSequenceGeneratorUI::setPercent(){
    (bsData) ? bsData->setFPercent(fPercent->value()) : LogFile::writeToLog("BGSGamebryoSequenceGeneratorUI::setPercent(): The data is nullptr!!");
}

