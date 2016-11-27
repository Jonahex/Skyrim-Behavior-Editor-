#ifndef BSBONESWITCHGENERATOR_H
#define BSBONESWITCHGENERATOR_H

#include "hkbgenerator.h"

class BSBoneSwitchGenerator: public hkbGenerator
{
    friend class BehaviorGraphView;
public:
    BSBoneSwitchGenerator(BehaviorFile *parent, long ref = 0);
    virtual ~BSBoneSwitchGenerator();
    bool readData(const HkxXmlReader & reader, long index);
    bool link();
    void unlink();
    QString getName() const;
    bool evaulateDataValidity();
    static QString getClassname();
    int getIndexToInsertIcon() const;
    bool write(HkxXMLWriter *writer);
private:
    bool wrapObject(hkbGenerator *objToInject, hkbGenerator *childToReplace);
    bool setChildAt(HkxObject *newChild, ushort index = 0);
    bool appendObject(hkbGenerator *objToAppend);
    bool removeObject(hkbGenerator *objToRemove, bool removeAll = true);
    int addChildrenToList(QList <HkxObjectExpSharedPtr> & list, bool reverseOrder = true);
    BSBoneSwitchGenerator& operator=(const BSBoneSwitchGenerator&);
    BSBoneSwitchGenerator(const BSBoneSwitchGenerator &);
private:
    static uint refCount;
    static QString classname;
    long userData;
    QString name;
    HkxObjectExpSharedPtr pDefaultGenerator;
    QList <HkxObjectExpSharedPtr> ChildrenA;
};

#endif // BSBONESWITCHGENERATOR_H
