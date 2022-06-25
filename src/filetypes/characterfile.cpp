#include "characterfile.h"
#include "skeletonfile.h"
#include "projectfile.h"
#include "src/xml/hkxxmlreader.h"
#include "src/xml/hkxxmlwriter.h"
#include "src/ui/mainwindow.h"
#include "src/hkxclasses/behavior/hkbcharacterdata.h"
#include "src/hkxclasses/behavior/hkbcharacterstringdata.h"
#include "src/hkxclasses/behavior/hkbboneweightarray.h"
#include "src/hkxclasses/behavior/hkbfootikdriverinfo.h"
#include "src/hkxclasses/behavior/hkbhandikdriverinfo.h"
#include "src/hkxclasses/behavior/hkbvariablevalueset.h"
#include "src/hkxclasses/behavior/hkbmirroredskeletoninfo.h"
#include "src/hkxclasses/hkrootlevelcontainer.h"

CharacterFile::CharacterFile(MainWindow *window, ProjectFile *projectfile, const QString & name, bool autogeneratedata, const QString &skeletonrelativepath)
    : HkxFile(window, name),
      project(projectfile),
      skeleton(nullptr),
      largestRef(0)
{
    getReader().setFile(this);
    if (autogeneratedata && skeletonrelativepath != ""){
        auto root = new hkRootLevelContainer(this);
        auto stringdata = new hkbCharacterStringData(this, 0);
        stringdata->setName(name.section("/", -1, -1).remove(".hkx"));
        stringdata->setRigName(skeletonrelativepath);
        stringdata->setRagdollName(skeletonrelativepath);
        stringdata->setBehaviorFilename("Behaviors\\Master.hkx");
        auto info = new hkbMirroredSkeletonInfo(this);
        auto values = new hkbVariableValueSet(this);
        auto characterdata = new hkbCharacterData(this, 0, stringdata, values, info);
        root->addVariant("hkbCharacterData", characterdata);
        setRootObject(HkxSharedPtr(root));
        setIsChanged(false);
    }
}

HkxSharedPtr * CharacterFile::findCharacterData(long ref){
    //std::lock_guard <std::mutex> guard(mutex);
    if (characterData->getReference() == ref){
        return &characterData;
    }
    return nullptr;
}

HkxSharedPtr * CharacterFile::findCharacterPropertyValues(long ref){
    //std::lock_guard <std::mutex> guard(mutex);
    for (auto i = 0; i < boneWeightArrays.size(); i++){
        if (boneWeightArrays.at(i).data() && boneWeightArrays.at(i).getShdPtrReference() == ref){
            return &boneWeightArrays[i];
        }
    }
    return nullptr;
}

QString CharacterFile::getBehaviorDirectoryName() const{
    //std::lock_guard <std::mutex> guard(mutex);
    auto ptr = static_cast<hkbCharacterStringData *>(stringData.data());
    QString name;
    (ptr) ? name = QString(ptr->getBehaviorFilename()).replace("\\", "/").section("/", -2, -2) : LogFile::writeToLog(": CharacterFile: 'stringData' is nullptr!");
    return name;
}

QString CharacterFile::getRigName() const{
    //std::lock_guard <std::mutex> guard(mutex);
    auto ptr = static_cast<hkbCharacterStringData *>(stringData.data());
    QString name;
    (ptr) ? name = QString(ptr->getRigName()).replace("\\", "/").section("/", -2, -1) : LogFile::writeToLog(": CharacterFile: 'stringData' is nullptr!");
    return name;
}

hkbCharacterData * CharacterFile::getCharacterData() const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (characterData.data() && characterData->getSignature() == HKB_CHARACTER_DATA){
        return static_cast<hkbCharacterData *>(characterData.data());
    }
    return nullptr;
}

QStringList CharacterFile::getRigBoneNames() const
{
    if (skeleton == nullptr)
    {
        LogFile::writeToLog(": CharacterFile: 'skeleton' is nullptr!");
        return {};
    }
    const int rigIndex = skeleton->getRigSkeletonIndex();
    if (rigIndex == -1)
    {
        return {};
    }
    return skeleton->getBonesFromSkeletonAt(rigIndex);
}

int CharacterFile::getNumberOfBones(bool ragdoll) const{
    //std::lock_guard <std::mutex> guard(mutex);
    auto count = -1;
    (skeleton) ? count = skeleton->getNumberOfBones(ragdoll) : LogFile::writeToLog(": CharacterFile: 'skeleton' is nullptr!");
    return count;
}

QString CharacterFile::getRagdollName() const{
    //std::lock_guard <std::mutex> guard(mutex);
    auto ptr = static_cast<hkbCharacterStringData *>(stringData.data());
    QString name;
    (ptr) ? name = ptr->getRagdollName() : LogFile::writeToLog(": CharacterFile: 'stringData' is nullptr!");
    return name;
}

QStringList CharacterFile::getRagdollBoneNames() const
{
    if (skeleton == nullptr)
    {
        LogFile::writeToLog(": CharacterFile: 'skeleton' is nullptr!");
        return {};
    }
    const int rigIndex = skeleton->getRagdollSkeletonIndex();
    if (rigIndex == -1)
    {
        return {};
    }
    return skeleton->getBonesFromSkeletonAt(rigIndex);
}

QStringList CharacterFile::getAnimationNames() const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (stringData.data() && stringData->getSignature() == HKB_CHARACTER_STRING_DATA){
        return static_cast<hkbCharacterStringData *>(stringData.data())->getAnimationNames();
    }
    return QStringList();
}

QString CharacterFile::getAnimationNameAt(int index) const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (stringData.data() && stringData->getSignature() == HKB_CHARACTER_STRING_DATA){
        return static_cast<hkbCharacterStringData *>(stringData.data())->getAnimationNameAt(index);
    }
    return QString();
}

QString CharacterFile::getCharacterPropertyNameAt(int index) const{
    //std::lock_guard <std::mutex> guard(mutex);
    auto ptr = static_cast<hkbCharacterStringData *>(stringData.data());
    QString name;
    (ptr) ? name = ptr->getCharacterPropertyNameAt(index) : LogFile::writeToLog(": CharacterFile: 'stringData' is nullptr!");
    return name;
}

int CharacterFile::getCharacterPropertyIndex(const QString &name) const{
    //std::lock_guard <std::mutex> guard(mutex);
    auto ptr = static_cast<hkbCharacterStringData *>(stringData.data());
    auto index = -1;
    (ptr) ? index = ptr->getCharacterPropertyIndex(name) : LogFile::writeToLog(": CharacterFile: 'stringData' is nullptr!");
    return index;
}

QStringList CharacterFile::getCharacterPropertyNames() const{
    //std::lock_guard <std::mutex> guard(mutex);
    auto ptr = static_cast<hkbCharacterData *>(characterData.data());
    QStringList names;
    (ptr) ? names = ptr->getCharacterPropertyNames() : LogFile::writeToLog(": CharacterFile: 'characterData' is nullptr!");
    return names;
}

QStringList CharacterFile::getCharacterPropertyTypenames() const{
    //std::lock_guard <std::mutex> guard(mutex);
    auto ptr = static_cast<hkbCharacterData *>(characterData.data());
    QStringList names;
    (ptr) ? names = ptr->getCharacterPropertyTypenames() : LogFile::writeToLog(": CharacterFile: 'characterData' is nullptr!");
    return names;
}

hkVariableType CharacterFile::getCharacterPropertyTypeAt(int index) const{
    //std::lock_guard <std::mutex> guard(mutex);
    auto ptr = static_cast<hkbCharacterData *>(characterData.data());
    hkVariableType type = VARIABLE_TYPE_INT8;
    (ptr) ? type = ptr->getCharacterPropertyTypeAt(index) : LogFile::writeToLog(": CharacterFile: 'characterData' is nullptr!");
    return type;
}

int CharacterFile::getAnimationIndex(const QString &name) const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (stringData.data() && stringData->getSignature() == HKB_CHARACTER_STRING_DATA){
        return static_cast<hkbCharacterStringData *>(stringData.data())->getAnimationIndex(name);
    }
    return -1;
}

int CharacterFile::getNumberOfAnimations() const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (stringData.data() && stringData->getSignature() == HKB_CHARACTER_STRING_DATA){
        return static_cast<hkbCharacterStringData *>(stringData.data())->getNumberOfAnimations();
    }
    return -1;
}

bool CharacterFile::isAnimationUsed(const QString &animationname) const{
    //std::lock_guard <std::mutex> guard(mutex);
    auto used = false;
    (project) ? used = project->isAnimationUsed(animationname) : LogFile::writeToLog(": CharacterFile: 'project' is nullptr!");
    return used;
}

QString CharacterFile::getRootBehaviorFilename() const{
    //std::lock_guard <std::mutex> guard(mutex);
    QString string;
    if (stringData.data() && stringData->getSignature() == HKB_CHARACTER_STRING_DATA){
        string = static_cast<hkbCharacterStringData *>(stringData.data())->getBehaviorFilename();
        (string.contains("\\")) ? string = string.section("\\", -1, -1) : string = string.section("/", -1, -1);
    }
    return string;
}

void CharacterFile::getCharacterPropertyBoneWeightArray(const QString &name, hkbBoneWeightArray *ptrtosetdata) const{
    //std::lock_guard <std::mutex> guard(mutex);
    auto strData = static_cast<hkbCharacterStringData *>(stringData.data());
    auto data = static_cast<hkbCharacterData *>(characterData.data());
    if (strData && data){
        auto index = strData->getCharacterPropertyIndex(name);
        ptrtosetdata->copyBoneWeights(static_cast<hkbBoneWeightArray *>(data->getVariantVariable(index)));
    }
}

bool CharacterFile::merge(CharacterFile *recessivefile){    //TO DO: merge character properties...
    //std::lock_guard <std::mutex> guard(mutex);
    if (recessivefile){
        if (!static_cast<hkbCharacterData *>(characterData.data())->merge(recessivefile->characterData.data())){
            WARNING_MESSAGE("CharacterFile: merge() failed! hkbCharacterData failed to merge!");
        }else{
            return true;
        }
    }
    return false;
}

void CharacterFile::setSkeletonFile(SkeletonFile *skel){
    //std::lock_guard <std::mutex> guard(mutex);
    skeleton = skel;
}

bool CharacterFile::addObjectToFile(HkxObject *obj, long ref){
    //std::lock_guard <std::mutex> guard(mutex);
    if (obj){
        if (ref < 1){
            largestRef++;
            ref = largestRef;
        }else if (ref > largestRef){
            largestRef = ref;
        }else{
            largestRef++;
        }
        obj->setReference(largestRef);
        if (obj->getSignature() == HKB_BONE_WEIGHT_ARRAY){
            boneWeightArrays.append(HkxSharedPtr(obj, ref));
        }else if (obj->getSignature() == HKB_CHARACTER_DATA){
            characterData = HkxSharedPtr(obj, ref);
        }else if (obj->getSignature() == HKB_CHARACTER_STRING_DATA){
            stringData = HkxSharedPtr(obj, ref);
        }else if (obj->getSignature() == HKB_MIRRORED_SKELETON_INFO){
            mirroredSkeletonInfo = HkxSharedPtr(obj, ref);
        }else if (obj->getSignature() == HKB_VARIABLE_VALUE_SET){
            characterPropertyValues = HkxSharedPtr(obj, ref);
        }else if (obj->getSignature() == HKB_HAND_IK_DRIVER_INFO){
            handIkDriverInfo = HkxSharedPtr(obj, ref);
        }else if (obj->getSignature() == HKB_FOOT_IK_DRIVER_INFO){
            footIkDriverInfo = HkxSharedPtr(obj, ref);
        }else if (obj->getSignature() == HKB_CHARACTER_DATA){
            characterData = HkxSharedPtr(obj, ref);
        }else if (obj->getSignature() == HK_ROOT_LEVEL_CONTAINER){
            setRootObject(HkxSharedPtr(obj, ref));
        }else{
            LogFile::writeToLog("CharacterFile: addObjectToFile() failed!\nInvalid type enum for this object!\nObject signature is: "+QString::number(obj->getSignature(), 16));
            return false;
        }
        //obj->setParentFile(this);
        return true;
    }
    return false;
}

QString CharacterFile::getSkeletonFileName() const{
    //std::lock_guard <std::mutex> guard(mutex);
    QString name;
    (skeleton) ? name = skeleton->getFileName() : LogFile::writeToLog(": CharacterFile: 'skeleton' is nullptr!");
    return name;
}

bool CharacterFile::parse(){
    //std::lock_guard <std::mutex> guard(mutex);
    long index = 2;
    auto ok = false;
    HkxSignature signature;
    QByteArray value;
    auto ref = 0;
    auto appendread = [&](HkxObject *obj, const QString & nameoftype){
        (!appendAndReadData(index, obj)) ? LogFile::writeToLog("CharacterFile: parse(): Failed to read a "+nameoftype+" object! Ref: "+QString::number(ref)) : NULL;
    };
    if (getReader().parse()){
        for (; index < getReader().getNumElements(); index++){
            value = getReader().getNthAttributeNameAt(index, 1);
            if (value == "class"){
                value = getReader().getNthAttributeValueAt(index, 2);
                if (value != ""){
                    ref = getReader().getNthAttributeValueAt(index, 0).remove(0, 1).toLong(&ok);
                    (!ok) ? LogFile::writeToLog("CharacterFile: parse() failed! The object reference string contained invalid characters and failed to convert to an integer!") : NULL;
                    signature = (HkxSignature)value.toULongLong(&ok, 16);
                    (!ok) ? LogFile::writeToLog("CharacterFile: parse() failed! The object signature string contained invalid characters and failed to convert to an integer!") : NULL;
                    switch (signature){
                    case HKB_BONE_WEIGHT_ARRAY:
                        appendread(new hkbBoneWeightArray(this, ref), "HKB_BONE_WEIGHT_ARRAY"); break;
                    case HKB_FOOT_IK_DRIVER_INFO:
                        appendread(new hkbFootIkDriverInfo(this, ref), "HKB_FOOT_IK_DRIVER_INFO"); break;
                    case HKB_HAND_IK_DRIVER_INFO:
                        appendread(new hkbHandIkDriverInfo(this, ref), "HKB_HAND_IK_DRIVER_INFO"); break;
                    case HKB_MIRRORED_SKELETON_INFO:
                        appendread(new hkbMirroredSkeletonInfo(this, ref), "HKB_HAND_IK_DRIVER_INFO"); break;
                    case HKB_VARIABLE_VALUE_SET:
                        appendread(new hkbVariableValueSet(this, ref), "HKB_VARIABLE_VALUE_SET"); break;
                    case HKB_CHARACTER_DATA:
                        appendread(new hkbCharacterData(this, ref), "HKB_CHARACTER_DATA"); break;
                    case HKB_CHARACTER_STRING_DATA:
                        appendread(new hkbCharacterStringData(this, ref), "HKB_CHARACTER_STRING_DATA"); break;
                    case HK_ROOT_LEVEL_CONTAINER:
                        appendread(new hkRootLevelContainer(this, ref), "HK_ROOT_LEVEL_CONTAINER"); break;
                    default:
                        LogFile::writeToLog(fileName()+": Unknown signature detected! Unknown object class name is: "+getReader().getNthAttributeValueAt(index, 1)+" Unknown object signature is: "+QString::number(signature, 16));
                    }
                }
            }
        }
        closeFile();
        getReader().clear();
        (link()) ? ok = true : NULL;
    }else{
        LogFile::writeToLog(fileName()+": failed to parse!!!");
    }
    return ok;
}

bool CharacterFile::link(){
    auto ok = true;
    if (!getRootObject().constData()){
        LogFile::writeToLog("CharacterFile: link() failed!\nThe root object of this character file is nullptr!");
        ok = false;
    }else if (getRootObject()->getSignature() != HK_ROOT_LEVEL_CONTAINER){
        LogFile::writeToLog("CharacterFile: link() failed!\nThe root object of this character file is NOT a hkRootLevelContainer!\nThe root object signature is: "+QString::number(getRootObject()->getSignature(), 16));
        ok = false;
    }
    if (!getRootObject()->link()){
        LogFile::writeToLog("CharacterFile: link() failed!\nThe root object of this character file failed to link to it's children!");
        ok = false;
    }
    if (!characterData.data() || !characterData->link()){
        LogFile::writeToLog("CharacterFile: link() failed!\ncharacterData failed to link to it's children!");
        ok = false;
    }
    if (!characterPropertyValues.data() || !characterPropertyValues->link()){
        LogFile::writeToLog("CharacterFile: link() failed!\ncharacterPropertyValues failed to link to it's children!");
        ok = false;
    }
    return ok;
}

void CharacterFile::addAnimation(const QString &name){
    //std::lock_guard <std::mutex> guard(mutex);
    if (stringData.data() && stringData->getSignature() == HKB_CHARACTER_STRING_DATA){
        static_cast<hkbCharacterStringData *>(stringData.data())->addAnimation(name);
    }
}

hkbBoneWeightArray *CharacterFile::addNewBoneWeightArray(){
    //std::lock_guard <std::mutex> guard(mutex);
    auto ptr = new hkbBoneWeightArray(this, 0, skeleton->getNumberOfBones());
    boneWeightArrays.append(HkxSharedPtr(ptr));
    return ptr;
}

void CharacterFile::write(){
    //std::lock_guard <std::mutex> guard(mutex);
    if (getRootObject().data()){
        ulong ref = getRootObject()->getReference();
        getRootObject()->setIsWritten(false);
        stringData->setIsWritten(false);
        characterData->setIsWritten(false);
        characterPropertyValues->setIsWritten(false);
        (mirroredSkeletonInfo.data()) ? mirroredSkeletonInfo->setIsWritten(false) : LogFile::writeToLog("CharacterFile::write(): 'mirroredSkeletonInfo' is nullptr!!");
        (handIkDriverInfo.data()) ? handIkDriverInfo->setIsWritten(false) : NULL;
        (footIkDriverInfo.data()) ? footIkDriverInfo->setIsWritten(false) : NULL;
        stringData->setReference(ref++);
        characterData->setReference(ref++);
        characterPropertyValues->setReference(ref++);
        mirroredSkeletonInfo->setReference(ref++);
        (handIkDriverInfo.data()) ? handIkDriverInfo->setReference(ref++) : NULL;
        (footIkDriverInfo.data()) ? footIkDriverInfo->setReference(ref++) : NULL;
        ref++;
        for (auto i = 0; i < boneWeightArrays.size(); i++, ref++){
            boneWeightArrays.at(i)->setIsWritten(false);
            boneWeightArrays.at(i)->setReference(ref);
        }
        getWriter().setFile(this);
        (!getWriter().writeToXMLFile()) ? LogFile::writeToLog("CharacterFile::write(): writeToXMLFile() failed!!") : NULL;
    }else{
        LogFile::writeToLog("CharacterFile::write(): The root object is nullptr!!");
    }
}

QStringList CharacterFile::getLocalFrameNames() const{
    //std::lock_guard <std::mutex> guard(mutex);
    QStringList names;
    (skeleton) ? names = skeleton->getLocalFrameNames() : LogFile::writeToLog(": CharacterFile: 'skeleton' is nullptr!");
    return names;
}

HkxObject *CharacterFile::getCharacterStringData() const{
    //std::lock_guard <std::mutex> guard(mutex);
    return stringData.data();
}

hkaSkeleton *CharacterFile::getSkeleton(bool isragdoll) const{
    //std::lock_guard <std::mutex> guard(mutex);
    hkaSkeleton *skel = nullptr;
    (skeleton) ? skel = skeleton->getSkeleton(isragdoll) : LogFile::writeToLog(": CharacterFile: 'skeleton' is nullptr!");
    return skel;
}

void CharacterFile::addFootIK(){
    //std::lock_guard <std::mutex> guard(mutex);
    if (characterData.data()){
        auto chardata = static_cast<hkbCharacterData *>(characterData.data());
        if (!footIkDriverInfo.data()){
            auto footik = new hkbFootIkDriverInfo(this);
            HkxSharedPtr shdptr(footik);
            addObjectToFile(footik);
            footIkDriverInfo = shdptr;
            chardata->setFootIkDriverInfo(footik);
        }else{
            chardata->setFootIkDriverInfo(static_cast<hkbFootIkDriverInfo *>(footIkDriverInfo.data()));
        }
        setIsChanged(true);
    }
}

void CharacterFile::addHandIK(){
    //std::lock_guard <std::mutex> guard(mutex);
    if (characterData.data()){
        auto chardata = static_cast<hkbCharacterData *>(characterData.data());
        if (!handIkDriverInfo.data()){
            auto handik = new hkbHandIkDriverInfo(this);
            HkxSharedPtr shdptr(handik);
            addObjectToFile(handik);
            handIkDriverInfo = shdptr;
            chardata->setHandIkDriverInfo(handik);
        }else{
            chardata->setHandIkDriverInfo(static_cast<hkbHandIkDriverInfo *>(handIkDriverInfo.data()));
        }
        setIsChanged(true);
    }
}

void CharacterFile::disableFootIK(){
    //std::lock_guard <std::mutex> guard(mutex);
    if (characterData.data()){
        static_cast<hkbCharacterData *>(characterData.data())->setFootIkDriverInfo(nullptr);
        footIkDriverInfo = HkxSharedPtr();
        setIsChanged(true);
    }
}

void CharacterFile::disableHandIK(){
    //std::lock_guard <std::mutex> guard(mutex);
    if (characterData.data()){
        static_cast<hkbCharacterData *>(characterData.data())->setHandIkDriverInfo(nullptr);
        handIkDriverInfo = HkxSharedPtr();
        setIsChanged(true);
    }
}

HkxSharedPtr& CharacterFile::getMirroredSkeletonInfo(){
    //std::lock_guard <std::mutex> guard(mutex);
    return mirroredSkeletonInfo;
}

bool CharacterFile::appendAnimation(SkyrimAnimationMotionData *motiondata){
    //std::lock_guard <std::mutex> guard(mutex);
    if (project){
        return project->appendAnimation(motiondata);
    }
    return false;
}

SkyrimAnimationMotionData CharacterFile::getAnimationMotionData(int animationindex) const{
    //std::lock_guard <std::mutex> guard(mutex);
    if (project){
        return project->getAnimationMotionData(animationindex);
    }
    return SkyrimAnimationMotionData(nullptr);
}

HkxSharedPtr& CharacterFile::getStringData(){
    //std::lock_guard <std::mutex> guard(mutex);
    return stringData;
}

HkxSharedPtr& CharacterFile::getHandIkDriverInfo(){
    //std::lock_guard <std::mutex> guard(mutex);
    return handIkDriverInfo;
}

HkxSharedPtr& CharacterFile::getFootIkDriverInfo(){
    //std::lock_guard <std::mutex> guard(mutex);
    return footIkDriverInfo;
}

HkxSharedPtr& CharacterFile::getCharacterPropertyValues(){
    //std::lock_guard <std::mutex> guard(mutex);
    return characterPropertyValues;
}

CharacterFile::~CharacterFile(){
    if (skeleton){
        delete skeleton;
    }
}
