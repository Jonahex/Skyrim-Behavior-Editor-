#include "modifiergeneratorui.h"
#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/generators/hkbmodifiergenerator.h"
#include "src/ui/genericdatawidgets.h"
#include "src/filetypes/hkxfile.h"
#include "src/ui/behaviorgraphui/behaviorgraphui.h"
#include "src/ui/behaviorgraphui/behaviorgraphicons.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QLineEdit>
#include <QComboBox>
#include <QHeaderView>

/*
 * ModifierGeneratorUI
 */

QStringList ModifierGeneratorUI::headerLabels1 = {
    "Name",
    "Type",
    "Value"
};

ModifierGeneratorUI::ModifierGeneratorUI(HkxObjectTableWidget *genTable, HkxObjectTableWidget *modTable)
    : generatorTable(genTable),
      modifiersTable(modTable),
      behaviorView(NULL),
      bsData(NULL),
      lyt(new QVBoxLayout),
      table(new TableWidget),
      name(new QLineEdit),
      modifier(new QPushButton("NULL")),
      generator(new QPushButton("NULL"))
{
    setTitle("hkbModifierGenerator");
    table->setRowCount(3);
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels(headerLabels1);
    table->setCellWidget(0, 0, name);
    table->setItem(0, 1, new QTableWidgetItem("String"));
    table->setItem(0, 2, new QTableWidgetItem("N/A"));
    table->setItem(0, 3, new QTableWidgetItem("N/A"));
    table->setItem(1, 0, new QTableWidgetItem("modifier"));
    table->setItem(1, 1, new QTableWidgetItem("hkbModifier"));
    table->setCellWidget(1, 2, modifier);
    table->setItem(2, 0, new QTableWidgetItem("generator"));
    table->setItem(2, 1, new QTableWidgetItem("hkbGenerator"));
    table->setCellWidget(2, 2, generator);
    lyt->addWidget(table);
    setLayout(lyt);
    connect(name, SIGNAL(editingFinished()), this, SLOT(setName()));
    connect(modifier, SIGNAL(activated(int)), this, SLOT(setModifier(int)));
    connect(generator, SIGNAL(activated(int)), this, SLOT(setGenerator(int)));
}

void ModifierGeneratorUI::setGeneratorTable(HkxObjectTableWidget *genTable){
    if (genTable){
        generatorTable = genTable;
        connect(generatorTable, SIGNAL(elementSelected(int)), this, SLOT(setGenerator(int)));
        connect(generatorTable, SIGNAL(hideWindow()), this, SLOT(viewGenerators()));
    }
}

void ModifierGeneratorUI::setModifierTable(HkxObjectTableWidget *modTable){
    if (modTable){
        modifiersTable = modTable;
        connect(modifiersTable, SIGNAL(elementSelected(int)), this, SLOT(setModifier(int)));
        connect(modifiersTable, SIGNAL(hideWindow()), this, SLOT(viewModifiers()));
    }
}

void ModifierGeneratorUI::viewGenerators(){
    if (!generatorTable->isVisible()){
        generatorTable->show();
    }else{
        generatorTable->hide();
    }
}

void ModifierGeneratorUI::viewModifiers(){
    if (!modifiersTable->isVisible()){
        modifiersTable->show();
    }else{
        modifiersTable->hide();
    }
}

void ModifierGeneratorUI::setName(){
    if (bsData){
        bsData->name = name->text();
        bsData->updateIconNames();
        emit generatorNameChanged(name->text(), bsData->getParentFile()->getIndexOfGenerator(bsData) + 1);
    }
}

void ModifierGeneratorUI::setModifier(int index){
    /*if (behaviorView && index > -1){
        hkbModifier *ptr = bsData->getParentFile()->getModifierDataAt(index);
        if (!behaviorView->selectedIcon->getChildIcon(ptr)){
            if (!ptr || ptr == bsData || !behaviorView->reconnectBranch(bsData->modifier.data(), ptr, behaviorView->getSelectedItem())){
                QMessageBox msg;
                msg.setText("I'M SORRY HAL BUT I CAN'T LET YOU DO THAT.\n\nYou are attempting to create a circular branch or dead end!!!");
                msg.exec();
                return;
            }
        }
        modifier->setText(ptr->getName());
        if (index > 0){
            bsData->modifier = HkxObjectExpSharedPtr(ptr);
            behaviorView->removeGeneratorData();
        }
    }*/
    generatorTable->hide();
}

void ModifierGeneratorUI::setGenerator(int index){
    if (behaviorView && index > -1){
        hkbGenerator *ptr = bsData->getParentFile()->getGeneratorDataAt(index);
        if (!behaviorView->selectedIcon->getChildIcon(ptr)){
            if (!ptr || ptr == bsData || !behaviorView->reconnectBranch(bsData->generator.data(), ptr, behaviorView->getSelectedItem())){
                QMessageBox msg;
                msg.setText("I'M SORRY HAL BUT I CAN'T LET YOU DO THAT.\n\nYou are attempting to create a circular branch or dead end!!!");
                msg.exec();
                return;
            }
        }
        generator->setText(ptr->getName());
        if (index > 0){
            bsData->generator = HkxObjectExpSharedPtr(ptr);
            behaviorView->removeGeneratorData();
        }
    }
    generatorTable->hide();
}

void ModifierGeneratorUI::loadData(HkxObject *data){
    if (data && data->getSignature() == HKB_MODIFIER_GENERATOR){
        bsData = static_cast<hkbModifierGenerator *>(data);
        name->setText(bsData->name);
        //generator->setText(static_cast<hkbModifier *>(bsData->modifier.data())->getName());
        generator->setText(static_cast<hkbGenerator *>(bsData->generator.data())->getName());
    }
}
