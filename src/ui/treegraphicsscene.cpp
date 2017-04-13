#include "treegraphicsscene.h"
#include "treegraphicsitem.h"
#include "dataiconmanager.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QScrollBar>

#define MAX_NUM_GRAPH_ICONS 10000

TreeGraphicsScene::TreeGraphicsScene()
    : rootIcon(NULL),
      selectedIcon(NULL),
      canDeleteRoot(false)
{
    setItemIndexMethod(QGraphicsScene::NoIndex);
}

void TreeGraphicsScene::setCanDeleteRoot(bool value){
    canDeleteRoot = value;
}

/*void TreeGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event){
    QGraphicsScene::mouseReleaseEvent(event);
    QList <QGraphicsItem *> children;
    QPainterPath path(event->scenePos());
    path.addRect(event->scenePos().x(), event->scenePos().y(), 1, 1);
    setSelectionArea(path);
    children = selectedItems();
    if (!children.isEmpty() && !dynamic_cast<QGraphicsPathItem *>(children.first())){
        if (event->button() == Qt::RightButton){
            //Test...
            //addItemToGraph((TreeGraphicsItem *)children.first(), new DataIconManager(), 0);
            removeItemFromGraph((TreeGraphicsItem *)children.first(), -1);
            //Test End
        }
    }
    clearSelection();
}*/

void TreeGraphicsScene::selectIcon(TreeGraphicsItem *icon, bool expand){
    TreeGraphicsItem *lastSelectedIcon = selectedIcon;
    QList <TreeGraphicsItem *> branch;
    if (lastSelectedIcon){
        lastSelectedIcon->unselect();
    }
    selectedIcon = icon;
    if (selectedIcon){
        if (expand){
            if (selectedIcon->getIsExpanded()){
                contractBranch(selectedIcon);
                selectedIcon->setIsExpanded(false);
            }else{
                expandBranch(selectedIcon);
            }
            if (!selectedIcon->isPrimaryIcon()){
                lastSelectedIcon = selectedIcon;
                selectedIcon = selectedIcon->getPrimaryIcon();
                if (!selectedIcon->isVisible()){
                    branch = selectedIcon->getAllIconsInBranch();
                    for (int i = 0; i < branch.size(); i++){
                        expandBranch(branch.at(i));
                    }
                }
                views().first()->centerOn(selectedIcon);//May need to expand the branch???
            }
            selectedIcon->reposition();
        }
        selectedIcon->setIconSelected();
    }
}

void TreeGraphicsScene::contractBranch(TreeGraphicsItem *icon, bool contractAll){
    QList <QGraphicsItem *> children;
    if (icon){
        children = icon->childItems();
        for (int i = 0; i < children.size(); i++){
            children.at(i)->setVisible(false);
            ((TreeGraphicsItem *)children.at(i))->path->setVisible(false);
            if (contractAll){
                ((TreeGraphicsItem *)children.at(i))->setIsExpanded(false);
                contractBranch((TreeGraphicsItem *)children.at(i), true);
            }else{
                contractBranch((TreeGraphicsItem *)children.at(i));
            }
        }
    }
}

void TreeGraphicsScene::expandBranch(TreeGraphicsItem *icon, bool expandAll){
    QList <QGraphicsItem *> children;
    if (icon){
        children = icon->childItems();
        icon->setIsExpanded(true);
        for (int i = 0; i < children.size(); i++){
            children.at(i)->setVisible(true);
            ((TreeGraphicsItem *)children.at(i))->path->setVisible(true);
            if (expandAll){
                expandBranch((TreeGraphicsItem *)children.at(i), true);
            }else if (((TreeGraphicsItem *)children.at(i))->getIsExpanded()){
                expandBranch((TreeGraphicsItem *)children.at(i));
            }
        }
    }
}

bool TreeGraphicsScene::drawGraph(DataIconManager *rootData, bool allowDuplicates){
    QList <DataIconManager *> objects;
    QList <DataIconManager *> children = rootData->getChildren();
    QList <TreeGraphicsItem *> parentIcons;
    QVector <short> numChildren;
    TreeGraphicsItem *newIcon = NULL;
    if (rootData){
        rootIcon = new TreeGraphicsItem(NULL, rootData);
        addItem(rootIcon);
        objects.append(children);
        numChildren.append(children.size());
        parentIcons.append(rootIcon);
        while (!objects.isEmpty() && !numChildren.isEmpty() && !parentIcons.isEmpty()){
            newIcon = addItemToGraph(parentIcons.first(), objects.first(), 0, false, allowDuplicates);
            if (!newIcon || newIcon->isDescendant(newIcon)){
                return false;
            }
            if (objects.first()->hasIcons()){
                children.clear();
            }else{
                children = objects.first()->getChildren();
            }
            numChildren.first()--;
            if (numChildren.first() == 0){
                numChildren.removeFirst();
                parentIcons.removeFirst();
            }
            objects.removeFirst();
            if (!children.isEmpty()){
                parentIcons.prepend(newIcon);
                numChildren.prepend(children.size());
                objects = children + objects;
            }
        }
    }
    //setSceneRect(sceneRect().marginsAdded(QMarginsF(100, 100, 1000, 1000)));
    return true;
}

//Appends "data" to the 'itemData' of "selectedIcon" after creating a new icon representing "data" and appending it to "selectedIcon"...
TreeGraphicsItem * TreeGraphicsScene::addItemToGraph(TreeGraphicsItem *selectedIcon, DataIconManager *data, int indexToInsert, bool inject, bool allowDuplicates){
    TreeGraphicsItem *newIcon = NULL;
    TreeGraphicsItem *parent = ((TreeGraphicsItem *)selectedIcon->parentItem());
    QList <QGraphicsItem *> children;
    if (data){
        if (selectedIcon){
            if (allowDuplicates){
                children = selectedIcon->childItems();
                for (int i = 0; i < children.size(); i++){
                    if (((TreeGraphicsItem *)children.at(i))->itemData == data){
                        if (!inject){
                            selectedIcon->itemData->insertObjectAt(indexToInsert, data);
                        }else if (parent){
                            selectedIcon->itemData->wrapObjectAt(indexToInsert, data, parent->itemData);
                            selectedIcon->setParent(newIcon, newIcon->getIndexofIconWithData(selectedIcon->itemData));//Not sure...
                        }
                        return NULL;
                    }
                }
            }
        }
        if (!inject){
            newIcon = new TreeGraphicsItem(selectedIcon, data, selectedIcon->getIndexofIconWithData(data));
            selectedIcon->itemData->insertObjectAt(indexToInsert, data);
        }else if (parent){
            newIcon = new TreeGraphicsItem(parent, data, parent->getIndexofIconWithData(selectedIcon->itemData));
            selectedIcon->itemData->wrapObjectAt(indexToInsert, data, parent->itemData);
            selectedIcon->setParent(newIcon, newIcon->getIndexofIconWithData(selectedIcon->itemData));
        }else{
            delete newIcon;
            return NULL;
        }
        if (newIcon->isDescendant(newIcon)){
            delete newIcon;
            return NULL;
        }
        //addItem(newIcon);  newIcon is added to scene already???
        //newIcon->reposition();
        addItem(newIcon->path);
    }
    return newIcon;
}

bool TreeGraphicsScene::reconnectIcon(TreeGraphicsItem *oldIconParent, DataIconManager *dataToReplace, DataIconManager *replacementData){
    TreeGraphicsItem *iconToReplace;
    TreeGraphicsItem *replacementIcon;
    int index = -1;
    if (oldIconParent && dataToReplace != replacementData){
        iconToReplace = oldIconParent->getChildWithData(dataToReplace);
        replacementIcon = oldIconParent->getReplacementIcon(replacementData);
        if (replacementIcon){
            index = oldIconParent->getIndexOfChild(iconToReplace);
            if (iconToReplace){
                removeItemFromGraph(iconToReplace, oldIconParent->itemData->getIndexOfObj(dataToReplace));
            }
            replacementIcon->setParent(oldIconParent, index);
            return true;
        }
    }
    return false;
}

bool TreeGraphicsScene::removeItemFromGraph(TreeGraphicsItem *item, int indexToRemove){
    QList <QGraphicsItem *> children;   //Storage for all referenced icons in the branch whose root is "item"...
    QList <QGraphicsItem *> tempList;   //Storage for the children of the first icon stored in "children"...
    QList <QGraphicsItem *> iconsToRemove;  //Storage for all icons to be removed from the graph...
    TreeGraphicsItem *itemToDeleteParent = NULL;
    TreeGraphicsItem *itemToDelete = NULL;  //Represents any icons to be removed that had children that were adopted by another icon representing the same data...
    int count = 0;  //Used to prevent possible infinite looping due to icons referencing ancestors...
    int index = -1; //Used to store the index of the position of "itemToDelete" in "children"...
    if (item){
        itemToDeleteParent = (TreeGraphicsItem *)item->parentItem();
        if (itemToDeleteParent){
            children.append(item);
            for (; count < MAX_NUM_GRAPH_ICONS, !children.isEmpty(); count++){  //Start cycling through children...
                itemToDelete = NULL;
                item = (TreeGraphicsItem *)children.first();
                tempList = item->childItems();
                if (!tempList.isEmpty() && item->isPrimaryIcon() && item->hasIcons()){  //"item" has children and has data that is referenced by other icons...
                    itemToDelete = item->reconnectToNextDuplicate();   //Reconnect "item" to the parent of the next icon that references it's data...
                    index = children.indexOf(itemToDelete);
                    if (index != -1){
                        children.replace(children.indexOf(itemToDelete), item); //"itemToDelete" is the
                    }
                    iconsToRemove.append(itemToDelete);
                    tempList.clear();
                }else{
                    iconsToRemove.append(item);
                }
                children.removeFirst();
                children = tempList + children;
            }
            for (int i = iconsToRemove.size() - 1; i >= 0; i--){
                delete iconsToRemove.at(i);
            }
            itemToDeleteParent->itemData->removeObjectAt(indexToRemove);
            rootIcon->reposition();
        }else if (canDeleteRoot){  //Icon with no parent must be the root...
            delete item;
            rootIcon = NULL;
        }
        if (count < MAX_NUM_GRAPH_ICONS){
            return true;
        }
    }
    return false;
}


