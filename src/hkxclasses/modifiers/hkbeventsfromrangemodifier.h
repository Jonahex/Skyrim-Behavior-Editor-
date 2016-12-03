#ifndef HKBEVENTSFROMRANGEMODIFIER_H
#define HKBEVENTSFROMRANGEMODIFIER_H

#include "hkbmodifier.h"

class hkbEventsFromRangeModifier: public hkbModifier
{
    friend class BehaviorGraphView;
public:
    hkbEventsFromRangeModifier(BehaviorFile *parent, long ref = 0);
    virtual ~hkbEventsFromRangeModifier();
    bool readData(const HkxXmlReader & reader, long index);
    bool link();
    void unlink();
    QString getName() const;
    bool evaulateDataValidity();
    static QString getClassname();
    bool write(HkxXMLWriter *writer);
private:
    hkbEventsFromRangeModifier& operator=(const hkbEventsFromRangeModifier&);
    hkbEventsFromRangeModifier(const hkbEventsFromRangeModifier &);
private:
    static uint refCount;
    static QString classname;
    long userData;
    QString name;
    bool enable;
    qreal inputValue;
    qreal lowerBound;
    HkxObjectExpSharedPtr eventRanges;
};

#endif // HKBEVENTSFROMRANGEMODIFIER_H
