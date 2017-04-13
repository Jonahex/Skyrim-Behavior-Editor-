#include "dataiconmanager.h"
#include "treegraphicsitem.h"

#include <QGraphicsScene>

QString DataIconManager::getName() const{
    return "";
}

bool DataIconManager::hasChildren() const{
    return false;
}

bool DataIconManager::hasIcons() const{
    if (icons.size() > 1){
        return true;
    }
    return false;
}

void DataIconManager::updateIconNames(){
    icons.first()->scene()->update();
    /*for (int i = 0; i < icons.size(); i++){
        icons.at(i)->update(QRectF(icons.at(i)->pos(), QSizeF(icons.at(i)->boundingRect().size())));
    }*/
}

QList<DataIconManager *> DataIconManager::getChildren() const{
    return QList<DataIconManager *> ();
}

int DataIconManager::getIndexOfObj(DataIconManager *) const{
    return -1;
}

bool DataIconManager::wrapObjectAt(int index, DataIconManager *obj, DataIconManager *parentObj){
    if (obj && parentObj){
        if (parentObj->insertObjectAt(parentObj->getIndexOfObj(this), obj) && obj->insertObjectAt(index, this)){
            return true;
        }
    }
    return false;
}

bool DataIconManager::insertObjectAt(int, DataIconManager *){
    return false;
}

bool DataIconManager::removeObjectAt(int ){
    return false;
}

DataIconManager::DataIconManager(HkxFile *parent, long ref)
    : HkDynamicObject(parent, ref)
{
    //
}

TreeGraphicsItem * DataIconManager::reconnectToNext(){
    TreeGraphicsItem *iconRemoved = NULL;
    QList<QGraphicsItem *> children;
    if (icons.size() > 1){
        iconRemoved = icons.at(1);
        icons.removeAt(1);
        if (iconRemoved){
            if (iconRemoved->parentItem() && icons.first()){
                children = iconRemoved->parentItem()->childItems();
                icons.first()->setParent((TreeGraphicsItem *)iconRemoved->parentItem(), children.indexOf(iconRemoved));
                return iconRemoved;
            }
        }
    }
    return NULL;
}

void DataIconManager::appendIcon(TreeGraphicsItem *icon){
    int index = -1;
    if (icon && icons.isEmpty() || !icons.contains(icon)){
        if (icons.size() > 1){
            index = icon->determineInsertionIndex();
            if (index > -1 && index < icons.size()){
                icons.insert(index, icon);
                return;
            }
        }
        icons.append(icon);
    }
}

void DataIconManager::removeIcon(TreeGraphicsItem *icon){
    icons.removeAll(icon);
}
