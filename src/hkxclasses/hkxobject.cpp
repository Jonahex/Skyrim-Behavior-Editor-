#include "src/hkxclasses/hkxobject.h"

#include <regex>

#include "src/filetypes/behaviorfile.h"
#include "src/xml/hkxxmlreader.h"
#include "src/hkxclasses/behavior/hkbvariablebindingset.h"
#include "src/ui/treegraphicsitem.h"
#include "src/hkxclasses/behavior/modifiers/hkbmodifier.h"

/**
 * HkxObject
 */

HkxObject::HkxObject(HkxFile *parent, long ref)
    : parentFile(parent),
      dataValid(true),
      isWritten(false),
      isMerged(false),
      refsUpdated(false),
      reference(ref)
{
    if (!parentFile){
        CRITICAL_ERROR_MESSAGE("HkxObject constructor was passed a null parent file!!! This program will terminate!!!");
        std::terminate();
    }
}

HkxSignature HkxObject::getSignature() const{
    //std::lock_guard <std::mutex> guard(mutex);  not needed, this should never change...
    return signature;
}

void HkxObject::setIsFileChanged(bool ischanged){
    std::lock_guard <std::mutex> guard(mutex);
    parentFile->setIsChanged(ischanged);
}

void HkxObject::setReference(int ref){
    std::lock_guard <std::mutex> guard(mutex);
    reference = ref;
}

long HkxObject::getReference() const{
    std::lock_guard <std::mutex> guard(mutex);
    return reference;
}

bool HkxObject::getIsMerged() const{
    std::lock_guard <std::mutex> guard(mutex);
    return isMerged;
}

void HkxObject::setIsMerged(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    isMerged = value;
}

bool HkxObject::getRefsUpdated() const{
    std::lock_guard <std::mutex> guard(mutex);
    return refsUpdated;
}

void HkxObject::setRefsUpdated(bool value){
    std::lock_guard <std::mutex> guard(mutex);
    refsUpdated = value;
}

void HkxObject::setParentFile(HkxFile *parent){
    std::lock_guard <std::mutex> guard(mutex);
    (parent) ? parentFile = parent : NULL;
}

int HkxObject::getIndexOfGenerator(const HkxSharedPtr &gen) const{
    std::lock_guard <std::mutex> guard(mutex);
    auto index = -1;
    BehaviorFile *behavior = dynamic_cast<BehaviorFile *>(parentFile);
    (behavior) ? index = behavior->getIndexOfGenerator(gen) : NULL;
    return index;
}

QString HkxObject::getParentFilename() const{
    std::lock_guard <std::mutex> guard(mutex);
    return parentFile->getFileName();
}

HkxFile *HkxObject::getParentFile() const{
    std::lock_guard <std::mutex> guard(mutex);
    return parentFile;
}

QVector<HkxObject *> HkxObject::getChildrenOtherTypes() const{
    return QVector<HkxObject *>();
}

QString HkxObject::getReferenceString() const{
    std::lock_guard <std::mutex> guard(mutex);
    QString temp("#"+QString::number(reference));
    reference >= 0 ? NULL: temp = "null";
    return temp;
}

QString HkxObject::getBoolAsString(bool b) const{
    if (b){
        return "true";
    }
    return "false";
}

HkxObject::HkxType HkxObject::getType() const{
    std::lock_guard <std::mutex> guard(mutex);
    return typeCheck;
}

void HkxObject::setDataValidity(bool isValid){
    std::lock_guard <std::mutex> guard(mutex);
    dataValid = isValid;
    if (typeCheck != TYPE_OTHER){
        static_cast<DataIconManager *>(this)->setIconValidity(isValid);
    }
}

bool HkxObject::isDataValid() const{
    std::lock_guard <std::mutex> guard(mutex);
    return dataValid;
}

void HkxObject::setIsWritten(bool written){
    std::lock_guard <std::mutex> guard(mutex);
    isWritten = written;
}

bool HkxObject::getIsWritten() const{
    std::lock_guard <std::mutex> guard(mutex);
    return isWritten;
}

void HkxObject::setType(HkxSignature sig, HkxType type){
    std::lock_guard <std::mutex> guard(mutex);
    signature = sig;
    typeCheck = type;
}

bool HkxObject::readReferences(const QByteArray &line, QVector <HkxSharedPtr> & children) const{
    auto size = 0;
    auto start = 0;
    auto ok = false;
    for (auto i = 0; i < line.size(); i++){
        if (line.at(i) == '#'){
            i++;
            start = i;
            size = 0;
            do{
                size++;
                i++;
            }while (i < line.size() && line.at(i) != ' ' && line.at(i) != '\n' && line.at(i) != '\r');
            QByteArray value(size, '\0');
            for (auto j = 0; j < size; j++){
                value[j] = line[start];
                start++;
            }
            children.append(HkxSharedPtr(nullptr, value.toLong(&ok)));
            if (!ok){
                return false;
            }
        }else if (line.at(i) == 'n'){
            start = i;
            size = 0;
            do{
                size++;
                i++;
            }while (i < line.size() && line.at(i) != ' ' && line.at(i) != '\n' && line.at(i) != '\r');
            QByteArray value(size, '\0');
            for (auto j = 0; j < size; j++){
                value[j] = line[start];
                start++;
            }
            if (value == "null"){
                children.append(HkxSharedPtr(nullptr));
                ok = true;
            } else {
                return false;
            }
        }
    }
    return ok;
}

bool HkxObject::readIntegers(const QByteArray &line, QVector<int> & ints) const{
    auto size = 0;
    auto start = 0;
    auto ok = true;
    for (auto i = 0; i < line.size(); i++){
        if ((line.at(i) >= '0' && line.at(i) <= '9') || (line.at(i) == '-')){
            start = i;
            size = 0;
            do{
                size++;
                i++;
            }while (i < line.size() && line.at(i) != ' ' && line.at(i) != '\n' && line.at(i) != '\r');
            QByteArray value(size, '\0');
            for (auto j = 0; j < size; j++){
                value[j] = line[start];
                start++;
            }
            ints.append(value.toInt(&ok));
            if (!ok){
                return false;
            }
        }
    }
    return ok;
}

bool HkxObject::toBool(const QByteArray &line, bool *ok) const{
    *ok = true;
    if (line == "true"){
        return true;
    }else if (line == "false"){
        return false;
    }else {
        *ok = false;
        return false;
    }
}

bool HkxObject::readDoubles(const QByteArray &line, QVector<qreal> & doubles) const{
    auto size = 0;
    auto start = 0;
    auto ok = false;
    for (auto i = 0; i < line.size(); i++){
        if ((line.at(i) >= '0' && line.at(i) <= '9') || (line.at(i) == '-')){
            start = i;
            size = 0;
            do{
                size++;
                i++;
            }while (i < line.size() && line.at(i) != ' ' && line.at(i) != '\n' && line.at(i) != '\r');
            QByteArray value(size, '\0');
            for (auto j = 0; j < size; j++){
                value[j] = line[start];
                start++;
            }
            doubles.append(value.toDouble(&ok));
            if (!ok){
                return false;
            }
        }
    }
    return ok;
}

hkVector3 HkxObject::readVector3(const QByteArray &lineIn, bool *ok) const{
    enum {X = 1, Y = 2, Z = 3};
    auto size = 0;
    auto start = 0;
    auto axisVar = 0;
    hkVector3 vector3;
    for (auto i = 0; i < lineIn.size(); i++){
        if ((lineIn.at(i) >= '0' && lineIn.at(i) <= '9') || (lineIn.at(i) == '-')){
            start = i;
            size = 0;
            do{
                size++;
                i++;
            }while (i < lineIn.size() && lineIn.at(i) != ' ' && lineIn.at(i) != ')');
            QByteArray value(size, '\0');
            for (auto j = 0; j < size; j++){
                value[j] = lineIn[start];
                start++;
            }
            axisVar++;
            switch (axisVar){
            case X:
                vector3.x = value.toDouble(ok);
                break;
            case Y:
                vector3.y = value.toDouble(ok);
                break;
            case Z:
                vector3.z = value.toDouble(ok);
                if (lineIn.at(i) != ')'){
                    *ok = false;
                    return hkVector3();
                }
                break;
            default:
                *ok = false;
                return hkVector3();
            }
            if (!*ok){
                return hkVector3();
            }
        }
    }
    return vector3;
}

hkQuadVariable HkxObject::readVector4(const QByteArray &lineIn, bool *ok) const{
    enum {X = 1, Y = 2, Z = 3, W = 4};
    auto size = 0;
    auto start = 0;
    auto axisVar = 0;
    hkQuadVariable vector;
    for (auto i = 0; i < lineIn.size(); i++){
        if ((lineIn.at(i) >= '0' && lineIn.at(i) <= '9') || (lineIn.at(i) == '-')){
            start = i;
            size = 0;
            do{
                size++;
                i++;
            }while (i < lineIn.size() && lineIn.at(i) != ' ' && lineIn.at(i) != ')');
            QByteArray value(size, '\0');
            for (auto j = 0; j < size; j++){
                value[j] = lineIn[start];
                start++;
            }
            axisVar++;
            switch (axisVar){
            case X:
                vector.x = value.toDouble(ok);
                break;
            case Y:
                vector.y = value.toDouble(ok);
                break;
            case Z:
                vector.z = value.toDouble(ok);
                break;
            case W:
                vector.w = value.toDouble(ok);
                if (lineIn.at(i) != ')'){
                    *ok = false;
                    return hkQuadVariable();
                }
                break;
            default:
                *ok = false;
                return hkQuadVariable();
            }
            if (!*ok){
                return hkQuadVariable();
            }
        }
    }
    return vector;
}

hkQsTransform HkxObject::readQsTransform(const QByteArray &lineIn, bool *ok) const{
    hkQsTransform transform;

    std::regex matcher("\\((.+?)\\)");

    std::string str = lineIn.toStdString();

    std::vector<std::string> matches;
    std::smatch match;
    while (std::regex_search(str, match, matcher))
    {
        matches.push_back(match[1]);
        str = match.suffix();
    }

    if (matches.size() != 3)
    {
        *ok = false;
        return transform;
    }

    const QStringList v1split = QString::fromStdString(matches[0]).split(' ', Qt::SkipEmptyParts);
    if (v1split.size() != 3)
    {
        *ok = false;
        return transform;
    }
    transform.v1.x = v1split[0].toDouble(ok);
    transform.v1.y = v1split[1].toDouble(ok);
    transform.v1.z = v1split[2].toDouble(ok);

    const QStringList v2split = QString::fromStdString(matches[1]).split(' ', Qt::SkipEmptyParts);
    if (v2split.size() != 4)
    {
        *ok = false;
        return transform;
    }
    transform.v2.x = v2split[0].toDouble(ok);
    transform.v2.y = v2split[1].toDouble(ok);
    transform.v2.z = v2split[2].toDouble(ok);
    transform.v2.w = v2split[3].toDouble(ok);

    const QStringList v3split = QString::fromStdString(matches[2]).split(' ', Qt::SkipEmptyParts);
    if (v3split.size() != 3)
    {
        *ok = false;
        return transform;
    }
    transform.v3.x = v3split[0].toDouble(ok);
    transform.v3.y = v3split[1].toDouble(ok);
    transform.v3.z = v3split[2].toDouble(ok);

    return transform;
}

bool HkxObject::readMultipleVector4(const QByteArray &lineIn,  QVector <hkQuadVariable> & vectors) const{
    enum {X = 1, Y = 2, Z = 3, W = 4};
    auto size = 0;
    auto start = 0;
    auto axisVar = 0;
    hkQuadVariable vector;
    auto ok = false;
    auto loop = false;
    for (auto i = 0; i < lineIn.size(); i++){
        if (lineIn.at(i) == '('){
            loop = true;
            i++;
            while (loop && i < lineIn.size()){
                if ((lineIn.at(i) >= '0' && lineIn.at(i) <= '9') || (lineIn.at(i) == '-')){
                    start = i;
                    size = 0;
                    do{
                        size++;
                        i++;
                    }while (i < lineIn.size() && lineIn.at(i) != ' ' && lineIn.at(i) != ')');
                    QByteArray value(size, '\0');
                    for (auto j = 0; j < size; j++){
                        value[j] = lineIn[start];
                        start++;
                    }
                    axisVar++;
                    switch (axisVar){
                    case X:
                        vector.x = value.toDouble(&ok);
                        break;
                    case Y:
                        vector.y = value.toDouble(&ok);
                        break;
                    case Z:
                        vector.z = value.toDouble(&ok);
                        break;
                    case W:
                        vector.w = value.toDouble(&ok);
                        if (!ok){
                            return false;
                        }
                        loop = false;
                        vectors.append(vector);
                        axisVar = 0;
                        if (lineIn.at(i) != ')'){
                            return false;
                        }
                        break;
                    default:
                        return false;
                    }
                }
                i++;
            }
        }
    }
    return ok;
}

void HkxObject::updateReferences(long &){}

void HkxObject::unlink(){}

bool HkxObject::readData(const HkxXmlReader & , long & ){return false;}

bool HkxObject::merge(HkxObject *){return false;}

void HkxObject::mergeEventIndex(int, int){}

bool HkxObject::isEventReferenced(int) const{return false;}

bool HkxObject::isVariableReferenced(int, bool isProperty) const{return false;}

void HkxObject::updateEventIndices(int ){}

void HkxObject::fixMergedEventIndices(BehaviorFile *){}

bool HkxObject::fixMergedIndices(BehaviorFile *){return true;}

bool HkxObject::write(HkxXMLWriter *){return false;}

QString HkxObject::evaluateDataValidity(){return "";}

/**
 * HkxObjectExpSharedPtr
 */

HkxSharedPtr::HkxSharedPtr(HkxObject *obj, long ref)
    :QExplicitlySharedDataPointer(obj), smtreference(ref)
{
    //
}

bool HkxSharedPtr::operator==(const HkxSharedPtr & other) const{
    if (data() == other.data()){
        return true;
    }
    return false;
}

void HkxSharedPtr::setShdPtrReference(long ref){
    smtreference = ref;
}

long HkxSharedPtr::getShdPtrReference() const{
    return smtreference;
}

bool HkxSharedPtr::readShdPtrReference(long index, const HkxXmlReader & reader){
    auto ok = true;
    auto temp = reader.getElementValueAt(index);
    if (temp.at(0) == '#'){
        temp.remove(0, 1);
    }
    if (temp == "null"){
        setShdPtrReference(-1);
    }else{
        setShdPtrReference(temp.toLong(&ok));
    }
    return ok;
}

/**
 * HkDynamicObject
 */

HkDynamicObject::HkDynamicObject(HkxFile *parent, long ref)
    : HkxObject(parent, ref)
{
    //
}

HkxSharedPtr& HkDynamicObject::getVariableBindingSet(){
    std::lock_guard <std::mutex> guard(mutex);
    return variableBindingSet;
}

hkbVariableBindingSet *HkDynamicObject::getVariableBindingSetData() const{
    std::lock_guard <std::mutex> guard(mutex);
    return static_cast<hkbVariableBindingSet *>(variableBindingSet.data());
}

void HkDynamicObject::addBinding(const QString & path, int varIndex, bool isProperty){
    std::lock_guard <std::mutex> guard(mutex);
    if (variableBindingSet.data()){
        static_cast<hkbVariableBindingSet *>(variableBindingSet.data())->addBinding(path, varIndex,(hkbVariableBindingSet::hkBinding::BindingType)(isProperty));
    }else{
        hkbVariableBindingSet *bind = new hkbVariableBindingSet(getParentFile());
        bind->addBinding(path, varIndex,(hkbVariableBindingSet::hkBinding::BindingType)(isProperty));
        variableBindingSet = HkxSharedPtr(bind);
    }
}

void HkDynamicObject::removeBinding(const QString & path){
    std::lock_guard <std::mutex> guard(mutex);
    if (variableBindingSet.data()){
        static_cast<hkbVariableBindingSet *>(variableBindingSet.data())->removeBinding(path);
    }
}

void HkDynamicObject::removeBinding(int varIndex){
    std::lock_guard <std::mutex> guard(mutex);
    if (variableBindingSet.data()){
        static_cast<hkbVariableBindingSet *>(variableBindingSet.data())->removeBinding(varIndex);
    }
}

bool HkDynamicObject::isVariableReferenced(int variableindex, bool isProperty) const{
    std::lock_guard <std::mutex> guard(mutex);
    if (variableBindingSet.constData() && static_cast<const hkbVariableBindingSet *>(variableBindingSet.constData())->isVariableRefed(variableindex, isProperty)){
        return true;
    }
    return false;
}

bool HkDynamicObject::merge(HkxObject *recessiveObject){ //TO DO: Make thread safe!!!
    std::lock_guard <std::mutex> guard(mutex);
    hkbVariableBindingSet *obj = nullptr;
    if (!getIsMerged() && recessiveObject && recessiveObject->getSignature() == HKB_VARIABLE_BINDING_SET){
        obj = static_cast<hkbVariableBindingSet *>(recessiveObject);
        if (variableBindingSet.data()){
            variableBindingSet->merge(obj);
        }else if (obj){
            variableBindingSet = HkxSharedPtr(obj);
            getParentFile()->addObjectToFile(obj, -1);
        }
        return true;
    }
    return false;
}

void HkDynamicObject::setBindingReference(int ref){
    std::lock_guard <std::mutex> guard(mutex);
    if (variableBindingSet.data()){
        variableBindingSet->setReference(ref);
    }
}

void HkDynamicObject::updateReferences(long &ref){
    std::lock_guard <std::mutex> guard(mutex);
    setReference(ref);
    if (variableBindingSet.data()){
        variableBindingSet->setReference(ref++);
    }
}

void HkDynamicObject::mergeVariableIndices(int oldindex, int newindex){
    std::lock_guard <std::mutex> guard(mutex);
    if (variableBindingSet.data()){
        static_cast<hkbVariableBindingSet *>(variableBindingSet.data())->mergeVariableIndex(oldindex, newindex);
    }
}

bool HkDynamicObject::fixMergedIndices(BehaviorFile *dominantfile){
    std::lock_guard <std::mutex> guard(mutex);
    if (variableBindingSet.data()){
        return variableBindingSet->fixMergedIndices(dominantfile);
    }
    return true;
}

bool HkDynamicObject::linkVar(){
    std::lock_guard <std::mutex> guard(mutex);
    auto ptr = static_cast<BehaviorFile *>(getParentFile())->findHkxObject(variableBindingSet.getShdPtrReference());
    if (ptr){
        if ((*ptr)->getSignature() != HKB_VARIABLE_BINDING_SET){
            LogFile::writeToLog("HkDynamicObject: linkVar()!\nThe linked object is not a HKB_VARIABLE_BINDING_SET!\nRemoving the link to the invalid object!");
            variableBindingSet = nullptr;
            return false;
        }
        variableBindingSet = *ptr;
    }
    return true;
}

void HkDynamicObject::unlink(){
    std::lock_guard <std::mutex> guard(mutex);
    variableBindingSet = HkxSharedPtr();
}

QString HkDynamicObject::evaluateDataValidity(){
    std::lock_guard <std::mutex> guard(mutex);
    if (variableBindingSet.data()){
        setDataValidity(false);
        if (variableBindingSet->getSignature() != HKB_VARIABLE_BINDING_SET){
            variableBindingSet = HkxSharedPtr();
            return QString(getParentFilename()+": HkDynamicObject: Ref: "+getReferenceString()+": variableBindingSet is invalid type! Signature: "+QString::number(variableBindingSet->getSignature(), 16)+"\n");
        }else if (static_cast<hkbVariableBindingSet *>(variableBindingSet.data())->getNumberOfBindings() < 1){
            variableBindingSet = HkxSharedPtr();
            return QString(getParentFilename()+": HkDynamicObject: Ref: "+getReferenceString()+": variableBindingSet has no bindings! Setting null value!");
        }
    }
    setDataValidity(true);
    return QString();
}

