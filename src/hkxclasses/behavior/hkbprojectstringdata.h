#ifndef HKBPROJECTSTRINGDATA_H
#define HKBPROJECTSTRINGDATA_H

#include <QStringList>

#include "src/hkxclasses/hkxobject.h"

class HkxFile;

class hkbProjectStringData: public HkxObject
{
    friend class BehaviorGraphView;
public:
    hkbProjectStringData(HkxFile *parent, long ref = 0);
    virtual ~hkbProjectStringData();
    bool readData(const HkxXmlReader & reader, long index);
    bool link();
    bool evaulateDataValidity();
    static QString getClassname();
    bool write(HkxXMLWriter *writer);
private:
    hkbProjectStringData& operator=(const hkbProjectStringData&);
    hkbProjectStringData(const hkbProjectStringData &);
private:
    static uint refCount;
    static QString classname;
    QStringList animationFilenames;
    QStringList behaviorFilenames;
    QStringList characterFilenames;
    QStringList eventNames;
    QString animationPath;
    QString behaviorPath;
    QString characterPath;
    QString fullPathToSource;
};

#endif // HKBPROJECTSTRINGDATA_H