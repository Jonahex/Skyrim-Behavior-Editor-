#ifndef CHARACTERFILE_H
#define CHARACTERFILE_H

#include "src/filetypes/hkxfile.h"

class SkeletonFile;
class hkbCharacterData;
class ProjectFile;
class hkbBoneWeightArray;
class SkyrimAnimationMotionData;
class hkaSkeleton;

class CharacterFile final: public HkxFile
{
    friend class ProjectFile;
public:
    CharacterFile(MainWindow *window, ProjectFile *projectfile, const QString & name, bool autogeneratedata = false, const QString & skeletonrelativepath = "");
    CharacterFile& operator=(const CharacterFile&) = delete;
    CharacterFile(const CharacterFile &) = delete;
    ~CharacterFile();
public:
    bool addObjectToFile(HkxObject *obj, long ref = -1);
    QString getSkeletonFileName() const;
    QString getRootObjectReferenceString();
    QString getBehaviorDirectoryName() const;
    QString getRigName() const;
    QStringList getRigBoneNames() const;
    int getNumberOfBones(bool ragdoll = false) const;
    QString getRagdollName() const;
    QStringList getRagdollBoneNames() const;
    QStringList getAnimationNames() const;
    QString getAnimationNameAt(int index) const;
    QString getCharacterPropertyNameAt(int index) const;
    int getCharacterPropertyIndex(const QString & name) const;
    QStringList getCharacterPropertyNames() const;
    QStringList getCharacterPropertyTypenames() const;
    hkVariableType getCharacterPropertyTypeAt(int index) const;
    int getAnimationIndex(const QString & name) const;
    int getNumberOfAnimations() const;
    bool isAnimationUsed(const QString & animationname) const;
    QString getRootBehaviorFilename() const;
    void getCharacterPropertyBoneWeightArray(const QString &name, hkbBoneWeightArray *ptrtosetdata) const;
    bool merge(CharacterFile *recessivefile);
    HkxSharedPtr& getCharacterPropertyValues();
    HkxSharedPtr& getFootIkDriverInfo();
    HkxSharedPtr& getHandIkDriverInfo();
    HkxSharedPtr& getStringData();
    HkxSharedPtr& getMirroredSkeletonInfo();
    bool appendAnimation(SkyrimAnimationMotionData *motiondata);
    SkyrimAnimationMotionData getAnimationMotionData(int animationindex) const;
    HkxSharedPtr * findCharacterPropertyValues(long ref);
    HkxSharedPtr * findCharacterData(long ref);
    void setSkeletonFile(SkeletonFile *skel);
    bool parse();
    void write();
    QStringList getLocalFrameNames() const;
    HkxObject * getCharacterStringData() const;
    hkaSkeleton * getSkeleton(bool isragdoll = false) const;
    ProjectFile* getProjectFile() const;
protected:
    bool link();
private:
    void addAnimation(const QString & name);
    hkbBoneWeightArray * addNewBoneWeightArray();
    hkbCharacterData * getCharacterData() const;
    void addFootIK();
    void addHandIK();
    void disableFootIK();
    void disableHandIK();
private:
    ProjectFile *project;
    SkeletonFile *skeleton;
    HkxSharedPtr characterData;
    HkxSharedPtr characterPropertyValues;
    HkxSharedPtr footIkDriverInfo;
    HkxSharedPtr handIkDriverInfo;
    HkxSharedPtr stringData;
    HkxSharedPtr mirroredSkeletonInfo;
    QVector <HkxSharedPtr> boneWeightArrays;
    long largestRef;
    mutable std::mutex mutex;
};

#endif // CHARACTERFILE_H
