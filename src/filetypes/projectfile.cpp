#include "projectfile.h"
#include "behaviorfile.h"
#include "characterfile.h"
#include "skeletonfile.h"

#include "src/animData/skyrimanimationmotiondata.h"
#include "src/animData/skyrimanimdata.h"
#include "src/animSetData/skyrimanimsetdata.h"
#include "src/animSetData/hkcrc.h"
#include "src/xml/hkxxmlreader.h"
#include "src/xml/hkxxmlwriter.h"
#include "src/ui/mainwindow.h"
#include "src/hkxclasses/behavior/modifiers/hkbmodifier.h"
#include "src/hkxclasses/behavior/hkbprojectdata.h"
#include "src/hkxclasses/behavior/hkbprojectstringdata.h"
#include "src/hkxclasses/hkrootlevelcontainer.h"
#include "src/hkxclasses/behavior/generators/hkbclipgenerator.h"
#include "src/hkxclasses/behavior/generators/hkbbehaviorreferencegenerator.h"
#include <mutex>
#include <thread>

ProjectFile::ProjectFile(MainWindow *window, const QString & name, bool autogeneratedata, const QString &relativecharacterfilepath)
    : HkxFile(window, name),
      largestRef(0),
      projectIndex(-1),
      skyrimAnimData(new SkyrimAnimData),
      skyrimAnimSetData(new SkyrimAnimSetData)
{
    projectName = fileName().section("/", -1, -1).remove(".hkx");
    projectFolderName = fileName().section("/", -2, -2);
    projectAnimationsPath = "meshes/actors/"+projectFolderName+"/animations";
    getReader().setFile(this);
    if (autogeneratedata && relativecharacterfilepath != ""){
        hkRootLevelContainer *root = new hkRootLevelContainer(this);
        root->addVariant("hkbProjectData");
        root->setVariantAt(0, new hkbProjectData(this, 0, new hkbProjectStringData(this, 0, relativecharacterfilepath)));
        setRootObject(HkxSharedPtr(root));
        setIsChanged(true);
    }
}

void ProjectFile::setCharacterFile(CharacterFile *file){
    character = file;
}

bool ProjectFile::isClipGenNameTaken(const QString &name) const{
    for (int i = 0; i < behaviorFiles.size(); i++){
        if (behaviorFiles.at(i)->isClipGenNameTaken(name)){
            return true;
        }
    }
    return false;
}

bool ProjectFile::readAnimationData(const QString & filename, bool searchforproject){
    QFile *animfile = new QFile(filename);
    QString projectname = fileName().section("/", -1, -1);
    if (!animfile->exists()){
        delete animfile;
        animfile = new QFile(QDir::currentPath()+"/animationdatasinglefile.txt");
        if (!animfile->exists()){
            FATAL_RUNTIME_ERROR("animationdatasinglefile.txt is missing from the application directory!");
        }
    }
    bool result = true;
    if (searchforproject){
        result = skyrimAnimData->parse(animfile, projectname);
    }else{
        result = skyrimAnimData->parse(animfile);
    }
    if (!result){
        delete animfile;
        FATAL_RUNTIME_ERROR("ProjectFile::readAnimationData(): The project animation data file could not be parsed!!!");
    }
    delete animfile;
    projectIndex = skyrimAnimData->getProjectIndex(projectname);
    return true;
}

bool ProjectFile::readAnimationSetData(const QString & filename){
    QFile *animsetfile = new QFile(QString(filename).replace("animationdatasinglefile.txt", "animationsetdatasinglefile.txt"));
    if (!animsetfile->exists()){
        delete animsetfile;
        animsetfile = new QFile(QDir::currentPath()+"/animationsetdatasinglefile.txt");
        if (!animsetfile->exists()){
            FATAL_RUNTIME_ERROR("animationsetdatasinglefile.txt is missing from the application directory!");
        }
    }
    if (!skyrimAnimSetData->parse(animsetfile)){
        delete animsetfile;
        FATAL_RUNTIME_ERROR("ProjectFile::readAnimationSetData(): The project animation set data file could not be parsed!!!");
    }
    delete animsetfile;
    return true;
}

bool ProjectFile::removeClipGenFromAnimData(const QString & animationname, const QString & clipname, const QString & variablename){
    bool result = skyrimAnimData->removeClipGenerator(projectName+".txt", clipname);
    skyrimAnimSetData->removeAnimationFromCache(projectName+".txt", animationname, variablename, clipname);
    return result;
}

bool ProjectFile::removeAnimationFromAnimData(const QString &name){
    return skyrimAnimData->removeAnimation(projectName+".txt", character->getAnimationNames().indexOf(name));    //Unsafe...
}


QString ProjectFile::getCharacterFilePathAt(int index) const{
    if (!stringData.data()){
        return "";
    }
    return static_cast<hkbProjectStringData *>(stringData.data())->getCharacterFilePathAt(index);
}

HkxSharedPtr * ProjectFile::findProjectData(long ref){
    if (projectData.getReference() == ref){
        return &projectData;
    }
    return nullptr;
}

HkxSharedPtr * ProjectFile::findProjectStringData(long ref){
    if (stringData.getReference() == ref){
        return &stringData;
    }
    return nullptr;
}

bool ProjectFile::addObjectToFile(HkxObject *obj, long ref){
    if (ref > largestRef){
        largestRef = ref;
    }else{
        largestRef++;
    }
    obj->setReference(largestRef);
    if (obj->getSignature() == HKB_PROJECT_DATA){
        projectData = HkxSharedPtr(obj, ref);
    }else if (obj->getSignature() == HKB_PROJECT_STRING_DATA){
        stringData = HkxSharedPtr(obj, ref);
    }else if (obj->getSignature() == HK_ROOT_LEVEL_CONTAINER){
        setRootObject(HkxSharedPtr(obj, ref));
    }else{
        writeToLog("ProjectFile: addObjectToFile() failed!\nInvalid type enum for this object!\nObject signature is: "+QString::number(obj->getSignature(), 16), true);
        return false;
    }
    return true;
}

bool ProjectFile::parse(){
    if (!getReader().parse()){
        return false;
    }
    int index = 2;
    bool ok = true;
    HkxSignature signature;
    QByteArray value;
    long ref = 0;
    //setProgressData("Creating HKX objects...", 60);
    while (index < getReader().getNumElements()){
        value = getReader().getNthAttributeNameAt(index, 1);
        if (value == "class"){
            value = getReader().getNthAttributeValueAt(index, 2);
            if (value != ""){
                ref = getReader().getNthAttributeValueAt(index, 0).remove(0, 1).toLong(&ok);
                if (!ok){
                    writeToLog("ProjectFile: parse() failed!\nThe object reference string contained invalid characters and failed to convert to an integer!", true);
                    return false;
                }
                signature = (HkxSignature)value.toULongLong(&ok, 16);
                if (!ok){
                    writeToLog("ProjectFile: parse() failed!\nThe object signature string contained invalid characters and failed to convert to an integer!", true);
                    return false;
                }
                if (signature == HKB_PROJECT_DATA){
                    if (!appendAndReadData(index, new hkbProjectData(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_PROJECT_STRING_DATA){
                    if (!appendAndReadData(index, new hkbProjectStringData(this, ref))){
                        return false;
                    }
                }else if (signature == HK_ROOT_LEVEL_CONTAINER){
                    if (!appendAndReadData(index, new hkRootLevelContainer(this, ref))){
                        return false;
                    }
                }else{
                    writeToLog("ProjectFile: parse()!\nUnknown signature detected!\nUnknown object class name is: "+getReader().getNthAttributeValueAt(index, 1)+"\nUnknown object signature is: "+QString::number(signature, 16));
                    return false;
                }
            }
        }
        index++;
    }
    closeFile();
    getReader().clear();
    //setProgressData("Linking HKX objects...", 80);
    if (!link()){
        writeToLog("ProjectFile: parse() failed because link() failed!", true);
        return false;
    }
    return true;
}

bool ProjectFile::link(){
    if (!getRootObject().constData()){
        writeToLog("ProjectFile: link() failed!\nThe root object of this project file is nullptr!", true);
        return false;
    }else if (getRootObject()->getSignature() != HK_ROOT_LEVEL_CONTAINER){
        writeToLog("ProjectFile: link() failed!\nThe root object of this project file is NOT a hkRootLevelContainer!\nThe root object signature is: "+QString::number(getRootObject()->getSignature(), 16), true);
        return false;
    }
    if (!getRootObject().data()->link()){
        writeToLog("ProjectFile: link() failed!\nThe root object of this project file failed to link to it's children!", true);
        return false;
    }
    if (!projectData.data()->link()){
        writeToLog("ProjectFile: link() failed!\nprojectData failed to link to it's children!\n", true);
        return false;
    }
    return true;
}

void ProjectFile::removeUnreferencedFiles(const hkbBehaviorReferenceGenerator *gentoignore){
    QStringList filestoremove;
    QStringList referencedbehaviors;
    QVector <int> behaviorindices;
    QString rootbehaviorname = character->getRootBehaviorFilename().toLower();
    for (auto i = 0; i < behaviorFiles.size(); i++){
        if (behaviorFiles.at(i)->fileName().toLower().section("/", -1, -1) == rootbehaviorname){
            behaviorindices.append(i);
        }else{
            filestoremove.append(behaviorFiles.at(i)->fileName());
        }
    }
    for (auto i = 0; i < behaviorindices.size(); i++){
        referencedbehaviors = behaviorFiles.at(behaviorindices.at(i))->getReferencedBehaviors(gentoignore);
        for (auto j = 0; j < referencedbehaviors.size(); j++){
            for (auto k = filestoremove.size() - 1; k > -1; k--){
                if (!QString::compare(referencedbehaviors.at(j), filestoremove.at(k).section("/", -1, -1), Qt::CaseInsensitive)){
                    for (auto l = 0; l < behaviorFiles.size(); l++){
                        if (behaviorFiles.at(l)->fileName() == filestoremove.at(k)){
                            behaviorindices.append(l);
                            filestoremove.removeAt(k);
                            k = -1;
                            l = behaviorFiles.size();
                        }
                    }
                }
            }
        }
    }
    ui->removeBehaviorGraphs(filestoremove);
}

bool ProjectFile::merge(ProjectFile *recessiveproject){ //Make sure to update event and variable indices when merging!!!
    bool value = false;
    if (recessiveproject){
        QList <BehaviorFile *> dominantbehaviors(behaviorFiles);
        QList <BehaviorFile *> recessivebehaviors(recessiveproject->behaviorFiles);
        QVector <HkxObject *> objectsnotfound;
        QString objname;
        if (!QString::compare(projectName, recessiveproject->projectName, Qt::CaseInsensitive)){
            for (auto i = 0; i < dominantbehaviors.size(); i++){
                for (auto j = recessivebehaviors.size(); j < -1; j--){
                    if (!QString::compare(dominantbehaviors.at(i)->fileName().section("/", -1, -1), recessivebehaviors.at(j)->fileName().section("/", -1, -1), Qt::CaseInsensitive)){
                        objectsnotfound = dominantbehaviors.at(i)->merge(recessivebehaviors.at(j));
                        recessivebehaviors.removeAt(j);
                        for (auto k = 0; k < recessivebehaviors.size() && !objectsnotfound.isEmpty(); k++){
                            recessivebehaviors.at(k)->merge(objectsnotfound);
                        }
                        for (auto k = 0; k < objectsnotfound.size(); k++){
                            if (objectsnotfound.at(k)->getType() == HkxObject::TYPE_GENERATOR){
                                objname = static_cast<hkbGenerator *>(objectsnotfound.at(k))->getName();
                            }else if (objectsnotfound.at(k)->getType() == HkxObject::TYPE_MODIFIER){
                                objname = static_cast<hkbModifier *>(objectsnotfound.at(k))->getName();
                            }else{
                                FATAL_RUNTIME_ERROR(QString("ProjectFile: merge(): Attempting to merge invalid object type!!!"));
                            }
                            writeToLog("ProjectFile: merge(): The object type \""+QString::number(objectsnotfound.at(k)->getSignature(), 16)
                                       +"\" named \""+objname+"\" was not found in the recessive behavior!!!");
                        }
                    }
                }
            }
            value = true;
        }else{
            writeToLog("ProjectFile: merge() failed!\nProject names are different!\n");
        }
    }else{
        writeToLog("ProjectFile: merge() failed!\nrecessiveproject is nullptr!\n");
    }
    return value;
}

void ProjectFile::addProjectToAnimData(){   //Unsafe...
    QStringList projectfiles;
    projectfiles.append(behaviorFiles.first()->fileName().section("/", -2, -1).replace("/", "\\"));
    projectfiles.append(character->fileName().section("/", -2, -1).replace("/", "\\"));
    projectfiles.append("character assets\\"+character->skeleton->fileName().section("/", -1, -1).replace("/", "\\"));
    auto index = skyrimAnimData->addNewProject(projectName+".txt", projectfiles);
    if (index == -1){
        WARNING_MESSAGE(QString("ProjectFile::addProjectToAnimData(): Project: "+projectName+".txt"+" already exists in the animation data!!!"));
    }else{
        projectIndex = index;
    }
    if (!skyrimAnimSetData->addNewProject(projectName+"ProjectData\\"+projectName+".txt")){
        WARNING_MESSAGE(QString("ProjectFile::addProjectToAnimData(): Project: "+projectName+".txt"+" already exists in the animation set data!!!"));
    }
}

AnimCacheProjectData *ProjectFile::getProjectCacheData() const{
    return skyrimAnimSetData->getProjectCacheData(projectName);
}

void ProjectFile::setAnimationIndexDuration(int indexofanimationlist, int animationindex, qreal duration){
    ProjectAnimData *project = skyrimAnimData->getProjectAnimData(projectName);
    if (project){
        if (indexofanimationlist == -1){
            project->animationMotionData.last()->animationIndex = animationindex;
            project->animationMotionData.last()->duration = duration;
        }else if (indexofanimationlist < project->animationMotionData.size()){
            project->animationMotionData.at(indexofanimationlist)->animationIndex = animationindex;
            project->animationMotionData.at(indexofanimationlist)->duration = duration;
        }
    }else{
        FATAL_RUNTIME_ERROR("ProjectFile::setAnimationIndexDuration(): skyrimAnimData->getProjectAnimData Failed!");
    }
}

void ProjectFile::generateAnimClipDataForProject(){
    HkxObject *generator;
    SkyrimClipGeneratoData *clipGenDataPtr;
    for (int i = 0; i < behaviorFiles.size(); i++){
        for (int j = 0; j < behaviorFiles.at(i)->generators.size(); j++){
            generator = behaviorFiles.at(i)->generators.at(j).data();
            if (generator->getSignature() == HKB_CLIP_GENERATOR){
                clipGenDataPtr = new SkyrimClipGeneratoData(static_cast<hkbClipGenerator *>(generator)->getClipGeneratorAnimData(skyrimAnimData->getProjectAnimData(projectName), getAnimationIndex(static_cast<hkbClipGenerator *>(generator)->animationName)));
                if (!skyrimAnimData->appendClipGenerator(projectName, clipGenDataPtr)){
                    writeToLog((QString("ProjectFile::generateAnimDataForProject(): Duplicate clip generator \""+clipGenDataPtr->getClipGeneratorName()+"found in: "+behaviorFiles.at(i)->fileName().section("/", -1, -1))));
                }
            }
        }
    }
}

void ProjectFile::loadEncryptedAnimationNames(){
    HkCRC encryptor;
    QStringList list = getAnimationNames();
    for (int i = 0; i < list.size(); i++){
        encryptedAnimationNames.append(QString(encryptor.compute(list.at(i).section("\\", -1, -1).toLower().replace(".hkx", "").toLocal8Bit())));
    }
}

void ProjectFile::addEncryptedAnimationName(const QString &unencryptedname){
    encryptedAnimationNames.append(HkCRC().compute(unencryptedname.section("\\", -1, -1).toLower().replace(".hkx", "").toLocal8Bit()));
}

void ProjectFile::removeEncryptedAnimationName(int index){
    if (index > -1 && index < encryptedAnimationNames.size()){
        encryptedAnimationNames.removeAt(index);
    }
}

void ProjectFile::deleteBehaviorFile(const QString &filename){
    for (auto i = behaviorFiles.size() - 1; i > -1; i--){
        if (filename == behaviorFiles.at(i)->fileName()){
            skyrimAnimData->removeBehaviorFromProject(projectName, behaviorFiles.at(i)->fileName().section("/", -2, -1).replace("/", "\\"));
            if (!behaviorFiles.at(i)->remove()){
                WARNING_MESSAGE("ProjectFile::deleteBehaviorFile(): File \""+behaviorFiles.at(i)->fileName()+"\" was not deleted from the file system!");
            }
            delete behaviorFiles.at(i);
            behaviorFiles.removeAt(i);
        }
    }
}

int ProjectFile::getAnimationIndex(const QString & name) const{
    if (character){
        return character->getAnimationIndex(name);
    }
    return -1;
}

bool ProjectFile::isAnimationUsed(const QString &animationname){
    HkxObject *generator;
    for (int i = 0; i < behaviorFiles.size(); i++){
        for (int j = 0; j < behaviorFiles.at(i)->generators.size(); j++){
            generator = behaviorFiles.at(i)->generators.at(j).data();
            if (generator->getSignature() == HKB_CLIP_GENERATOR){
                if (!QString::compare(animationname, static_cast<hkbClipGenerator *>(generator)->getAnimationName(), Qt::CaseInsensitive)){
                    writeToLog("ProjectFile: isAnimationUsed()!\nAnimation is used in the Clip Generator \""+static_cast<hkbClipGenerator *>(generator)->getName()+"\" in behavior: "+behaviorFiles.at(i)->fileName().section("/",-1,-1));
                    return true;
                }
            }
        }
    }
    return false;
}

QStringList ProjectFile::getAnimationNames() const{
    if (character){
        return character->getAnimationNames();
    }
    return QStringList();
}

QString ProjectFile::findAnimationNameFromEncryptedData(const QString &encryptedname) const{
    bool ok = true;
    ULONGLONG value1;
    ULONGLONG value2 = encryptedname.toULongLong(&ok, 10);
    if (!ok){
        value2 = encryptedname.toULongLong(&ok, 16);
        if (!ok){
            FATAL_RUNTIME_ERROR("ProjectFile::findAnimationNameFromEncryptedData(): encryptedname.toULong(&ok, 10) Failed!");
        }
    }
    for (int i = 0; i < encryptedAnimationNames.size(); i++){
        value1 = encryptedAnimationNames.at(i).toULongLong(&ok, 16);
        if (!ok){
            FATAL_RUNTIME_ERROR("ProjectFile::findAnimationNameFromEncryptedData(): encryptedAnimationNames.at(i).toULong(&ok, 16) Failed!");
        }
        if (value1 == value2){
            if (!character){
                FATAL_RUNTIME_ERROR("ProjectFile::findAnimationNameFromEncryptedData(): character is nullptr!");
            }
            return character->getAnimationNameAt(i).toLower().section("\\", -1, -1).replace(".hkx", "");
        }
    }
    return "";
}

bool ProjectFile::isProjectNameTaken() const{
    return skyrimAnimData->isProjectNameTaken(projectName);
}

QString ProjectFile::getProjectName() const{
    return projectName;
}

bool ProjectFile::appendClipGeneratorAnimData(const QString &name){
    if (name == ""){
        return false;
    }
    return skyrimAnimData->appendClipGenerator(projectName, new SkyrimClipGeneratoData(skyrimAnimData->getProjectAnimData(projectName), name));
}

void ProjectFile::setLocalTimeForClipGenAnimData(const QString &clipname, int triggerindex, qreal time){
    skyrimAnimData->setLocalTimeForClipGenAnimData(projectName, clipname, triggerindex, time);
}

void ProjectFile::setEventNameForClipGenAnimData(const QString &clipname, int triggerindex, const QString &eventname){
    skyrimAnimData->setEventNameForClipGenAnimData(projectName, clipname, triggerindex, eventname);
}

void ProjectFile::setClipNameAnimData(const QString &oldclipname, const QString &newclipname){
    skyrimAnimData->setClipNameAnimData(projectName, oldclipname, newclipname);
}

void ProjectFile::setAnimationIndexForClipGen(int index, const QString &clipGenName){
    skyrimAnimData->setAnimationIndexForClipGen(projectName, clipGenName, index);
}

void ProjectFile::setPlaybackSpeedAnimData(const QString &clipGenName, qreal speed){
    skyrimAnimData->setPlaybackSpeedAnimData(projectName, clipGenName, speed);
}

void ProjectFile::setCropStartAmountLocalTimeAnimData(const QString &clipGenName, qreal time){
    skyrimAnimData->setCropStartAmountLocalTimeAnimData(projectName, clipGenName, time);
}

void ProjectFile::setCropEndAmountLocalTimeAnimData(const QString &clipGenName, qreal time){
    skyrimAnimData->setCropEndAmountLocalTimeAnimData(projectName, clipGenName, time);
}

void ProjectFile::appendClipTriggerToAnimData(const QString &clipGenName, const QString &eventname){
    skyrimAnimData->appendClipTriggerToAnimData(projectName, clipGenName, eventname);
}

void ProjectFile::removeClipTriggerToAnimDataAt(const QString &clipGenName, int index){
    skyrimAnimData->removeClipTriggerToAnimDataAt(projectName, clipGenName, index);
}

/*SkyrimAnimData *ProjectFile::getAnimData() const{
    return skyrimAnimData;
}*/

void ProjectFile::write(){
    ulong ref = 1;
    getRootObject().data()->setReference(ref);
    getRootObject().data()->setIsWritten(false);
    ref++;
    stringData.data()->setReference(ref);
    stringData.data()->setIsWritten(false);
    ref++;
    projectData.data()->setReference(ref);
    projectData.data()->setIsWritten(false);
    ref++;
    getWriter().setFile(this);
    getWriter().writeToXMLFile();
}

ProjectFile::~ProjectFile(){
    if (character){
        delete character;
    }
    if (skyrimAnimData){
        delete skyrimAnimData;
    }
    if (skyrimAnimSetData){
        delete skyrimAnimSetData;
    }
    for (int i = 0; i < behaviorFiles.size(); i++){
        if (behaviorFiles.at(i)){
            delete behaviorFiles.at(i);
        }
    }
}
