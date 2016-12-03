#include "hkxfile.h"
#include "src/xml/hkxxmlreader.h"
#include "src/xml/hkxxmlwriter.h"
#include "src/ui/mainwindow.h"

#include "src/hkxclasses/generators/bsistatetagginggenerator.h"
#include "src/hkxclasses/generators/bscyclicblendtransitiongenerator.h"
#include "src/hkxclasses/generators/bsboneswitchgenerator.h"
#include "src/hkxclasses/generators/bsboneswitchgeneratorbonedata.h"
#include "src/hkxclasses/generators/bssynchronizedclipgenerator.h"
#include "src/hkxclasses/generators/bsoffsetanimationgenerator.h"
#include "src/hkxclasses/generators/bgsgamebryosequencegenerator.h"

#include "src/hkxclasses/generators/hkbgenerator.h"
#include "src/hkxclasses/generators/hkbstatemachinestateinfo.h"
#include "src/hkxclasses/generators/hkbstatemachine.h"
#include "src/hkxclasses/generators/hkbmodifiergenerator.h"
#include "src/hkxclasses/generators/hkbmanualselectorgenerator.h"
#include "src/hkxclasses/generators/hkbblendergeneratorchild.h"
#include "src/hkxclasses/generators/hkbblendergenerator.h"
#include "src/hkxclasses/generators/hkbposematchinggenerator.h"
#include "src/hkxclasses/generators/hkbclipgenerator.h"
#include "src/hkxclasses/generators/hkbbehaviorreferencegenerator.h"

#include "src/hkxclasses/modifiers/bsisactivemodifier.h"
#include "src/hkxclasses/modifiers/bslimbikmodifier.h"
#include "src/hkxclasses/modifiers/bsspeedsamplermodifier.h"
#include "src/hkxclasses/modifiers/bslookatmodifier.h"
#include "src/hkxclasses/modifiers/bsmodifyoncemodifier.h"
#include "src/hkxclasses/modifiers/bseventonfalsetotruemodifier.h"
#include "src/hkxclasses/modifiers/bseventondeactivatemodifier.h"
#include "src/hkxclasses/modifiers/bsragdollcontactlistenermodifier.h"
#include "src/hkxclasses/modifiers/bsdirectatmodifier.h"
#include "src/hkxclasses/modifiers/bsdecomposevectormodifier.h"
#include "src/hkxclasses/modifiers/bscomputeaddboneanimmodifier.h"
#include "src/hkxclasses/modifiers/bsdisttriggermodifier.h"
#include "src/hkxclasses/modifiers/bseventeveryneventsmodifier.h"
#include "src/hkxclasses/modifiers/bsinterpvaluemodifier.h"
#include "src/hkxclasses/modifiers/bspassbytargettriggermodifier.h"
#include "src/hkxclasses/modifiers/bstimermodifier.h"
#include "src/hkxclasses/modifiers/bstweenermodifier.h"
#include "src/hkxclasses/modifiers/bsistatemanagermodifier.h"

#include "src/hkxclasses/modifiers/hkbmodifierlist.h"
#include "src/hkxclasses/modifiers/hkbtwistmodifier.h"
#include "src/hkxclasses/modifiers/hkbeventdrivenmodifier.h"
#include "src/hkxclasses/modifiers/hkbfootikcontrolsmodifier.h"
#include "src/hkxclasses/modifiers/hkbevaluateexpressionmodifier.h"
#include "src/hkxclasses/modifiers/hkbrotatecharactermodifier.h"
#include "src/hkxclasses/modifiers/hkbdampingmodifier.h"
#include "src/hkxclasses/modifiers/hkbkeyframebonesmodifier.h"
#include "src/hkxclasses/modifiers/hkbpoweredragdollcontrolsmodifier.h"
#include "src/hkxclasses/modifiers/hkbtimermodifier.h"
#include "src/hkxclasses/modifiers/hkbrigidbodyragdollcontrolsmodifier.h"
#include "src/hkxclasses/modifiers/hkbgetupmodifier.h"
#include "src/hkxclasses/modifiers/hkbcomputedirectionmodifier.h"
#include "src/hkxclasses/modifiers/hkbevaluatehandlemodifier.h"
#include "src/hkxclasses/modifiers/hkbgethandleonbonemodifier.h"
#include "src/hkxclasses/modifiers/hkbsensehandlemodifier.h"
#include "src/hkxclasses/modifiers/hkbdelayedmodifier.h"
#include "src/hkxclasses/modifiers/hkbdetectclosetogroundmodifier.h"
#include "src/hkxclasses/modifiers/hkbattachmentmodifier.h"
#include "src/hkxclasses/modifiers/hkbattributemodifier.h"
#include "src/hkxclasses/modifiers/hkbcombinetransformsmodifier.h"
#include "src/hkxclasses/modifiers/hkbcomputerotationfromaxisanglemodifier.h"
#include "src/hkxclasses/modifiers/hkbcomputerotationtotargetmodifier.h"
#include "src/hkxclasses/modifiers/hkbeventsfromrangemodifier.h"
#include "src/hkxclasses/modifiers/hkbextractragdollposemodifier.h"
#include "src/hkxclasses/modifiers/hkbgetworldfrommodelmodifier.h"
#include "src/hkxclasses/modifiers/hkblookatmodifier.h"
#include "src/hkxclasses/modifiers/hkbmirrormodifier.h"
#include "src/hkxclasses/modifiers/hkbmovecharactermodifier.h"
#include "src/hkxclasses/modifiers/hkbtransformvectormodifier.h"
#include "src/hkxclasses/modifiers/hkbproxymodifier.h"

#include "src/hkxclasses/hkxobject.h"
#include "src/hkxclasses/hkbgeneratortransitioneffect.h"
#include "src/hkxclasses/hkbeventrangedataarray.h"
#include "src/hkxclasses/hkbboneweightarray.h"
#include "src/hkxclasses/hkbboneindexarray.h"
#include "src/hkxclasses/hkbexpressiondataarray.h"
#include "src/hkxclasses/hkbstatemachinetransitioninfoarray.h"
#include "src/hkxclasses/hkbstatemachineeventpropertyarray.h"
#include "src/hkxclasses/hkbblendingtransitioneffect.h"
#include "src/hkxclasses/hkbexpressioncondition.h"
#include "src/hkxclasses/generators/hkbbehaviorgraph.h"
#include "src/hkxclasses/hkbcliptriggerarray.h"
#include "src/hkxclasses/hkbstringeventpayload.h"
#include "src/hkxclasses/hkbbehaviorgraphdata.h"
#include "src/hkxclasses/hkbbehaviorgraphstringdata.h"
#include "src/hkxclasses/hkbvariablevalueset.h"
#include "src/hkxclasses/hkbvariablebindingset.h"
#include "src/hkxclasses/hkrootlevelcontainer.h"

#include <QStringList>

/**
 * Class: HkxFile
 *
 */

HkxFile::HkxFile(MainWindow *window, const QString & name)
    : QFile(name),
      ui(window)
{
    parse();
}

void HkxFile::closeFile(){
    if (isOpen()){
        close();
    }
}

HkxObjectExpSharedPtr & HkxFile::getRootObject(){
    return rootObject;
}

bool HkxFile::parse(){
    return true;
}

bool HkxFile::link(){
    return true;
}

void HkxFile::setRootObject(HkxObjectExpSharedPtr & obj){
    rootObject = obj;
}

MainWindow * HkxFile::getUI(){
    return ui;
}

void HkxFile::writeToLog(const QString & message, bool isError){
    if (ui){
        ui->writeToLog(message, isError);
    }
}

void HkxFile::setProgressData(const QString & message, int value){
    ui->setProgressData(message, value);
}

HkxFile::~HkxFile(){
    //
}

/**
 * Class: BehaviorFile
 *
 */

BehaviorFile::BehaviorFile(MainWindow *window, const QString & name)
    : HkxFile(window, name),
      largestRef(0)
{
    reader.setBehaviorFile(this);
    if (!parse()){
        writeToLog("MainWindow: parse() failed!");
        getUI()->drawGraph = false;
    }
    reader.clear();
}

template <typename T>
bool BehaviorFile::appendAndReadData(int index, T * obj){
    //Skip the current line.
    index++;
    if (!obj->readData(reader, index)){
        return false;
    }
    return true;
}

//template <typename T>
bool BehaviorFile::addObjectToFile(HkxObject *obj, long ref){
    if (ref > largestRef){
        largestRef = ref;
    }else{
        largestRef++;
    }
    obj->setReference(largestRef);
    if (obj->getType() == HkxObject::TYPE_GENERATOR){
        if (obj->getSignature() == HKB_STATE_MACHINE_STATE_INFO ||
            obj->getSignature() == HKB_BLENDER_GENERATOR_CHILD ||
            obj->getSignature() == BS_BONE_SWITCH_GENERATOR_BONE_DATA
           )
        {
            generatorChildren.append(HkxObjectExpSharedPtr(obj, ref));
        }else{
            generators.append(HkxObjectExpSharedPtr(obj, ref));
        }
    }else if (obj->getType() == HkxObject::TYPE_MODIFIER){
        modifiers.append(HkxObjectExpSharedPtr(obj, ref));
    }else if (obj->getType() == HkxObject::TYPE_OTHER){
        otherTypes.append(HkxObjectExpSharedPtr(obj, ref));
    }else{
        writeToLog("BehaviorFile: addObjectToFile() failed!\nInvalid type enum for this object!\nObject signature is: "+QString::number(obj->getSignature(), 16), true);
        return false;
    }
    return true;
}

bool BehaviorFile::parse(){
    if (!reader.parse()){
        return false;
    }
    int index = 2;
    bool ok = true;
    qulonglong signature;
    QByteArray value;
    long ref = 0;
    setProgressData("Creating HKX objects...", 60);
    while (index < reader.getNumElements()){
        value = reader.getNthAttributeNameAt(index, 1);
        if (value == "class"){
            value = reader.getNthAttributeValueAt(index, 2);
            if (value != ""){
                ref = reader.getNthAttributeValueAt(index, 0).remove(0, 1).toLong(&ok);
                if (!ok){
                    writeToLog("BehaviorFile: parse() failed!\nThe object reference string contained invalid characters and failed to convert to an integer!", true);
                    return false;
                }
                signature = value.toULongLong(&ok, 16);
                if (!ok){
                    writeToLog("BehaviorFile: parse() failed!\nThe object signature string contained invalid characters and failed to convert to an integer!", true);
                    return false;
                }
                if (signature == HKB_STATE_MACHINE_STATE_INFO){
                    if (!appendAndReadData(index, new hkbStateMachineStateInfo(this, NULL, ref))){
                        return false;
                    }
                }else if (signature == HKB_STATE_MACHINE){
                    if (!appendAndReadData(index, new hkbStateMachine(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_VARIABLE_BINDING_SET){
                    if (!appendAndReadData(index, new hkbVariableBindingSet(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_MODIFIER_GENERATOR){
                    if (!appendAndReadData(index, new hkbModifierGenerator(this, ref))){
                        return false;
                    }
                }else if (signature == BS_I_STATE_TAGGING_GENERATOR){
                    if (!appendAndReadData(index, new BSiStateTaggingGenerator(this, ref))){
                        return false;
                    }
                }else if (signature == BS_CYCLIC_BLEND_TRANSITION_GENERATOR){
                    if (!appendAndReadData(index, new BSCyclicBlendTransitionGenerator(this, ref))){
                        return false;
                    }
                }else if (signature == BS_BONE_SWITCH_GENERATOR){
                    if (!appendAndReadData(index, new BSBoneSwitchGenerator(this, ref))){
                        return false;
                    }
                }else if (signature == BS_BONE_SWITCH_GENERATOR_BONE_DATA){
                    if (!appendAndReadData(index, new BSBoneSwitchGeneratorBoneData(this, ref))){
                        return false;
                    }
                }else if (signature == BS_SYNCHRONIZED_CLIP_GENERATOR){
                    if (!appendAndReadData(index, new BSSynchronizedClipGenerator(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_MANUAL_SELECTOR_GENERATOR){
                    if (!appendAndReadData(index, new hkbManualSelectorGenerator(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_BLENDER_GENERATOR_CHILD){
                    if (!appendAndReadData(index, new hkbBlenderGeneratorChild(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_BONE_WEIGHT_ARRAY){
                    if (!appendAndReadData(index, new hkbBoneWeightArray(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_MODIFIER_LIST){
                    if (!appendAndReadData(index, new hkbModifierList(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_BLENDER_GENERATOR){
                    if (!appendAndReadData(index, new hkbBlenderGenerator(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_BLENDING_TRANSITION_EFFECT){
                    if (!appendAndReadData(index, new hkbBlendingTransitionEffect(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_EXPRESSION_CONDITION){
                    if (!appendAndReadData(index, new hkbExpressionCondition(this, "", ref))){
                        return false;
                    }
                }else if (signature == BS_OFFSET_ANIMATION_GENERATOR){
                    if (!appendAndReadData(index, new BSOffsetAnimationGenerator(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_TWIST_MODIFIER){
                    if (!appendAndReadData(index, new hkbTwistModifier(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_EVENT_DRIVEN_MODIFIER){
                    if (!appendAndReadData(index, new hkbEventDrivenModifier(this, ref))){
                        return false;
                    }
                }else if (signature == BS_IS_ACTIVE_MODIFIER){
                    if (!appendAndReadData(index, new BSIsActiveModifier(this, ref))){
                        return false;
                    }
                }else if (signature == BS_LIMB_IK_MODIFIER){
                    if (!appendAndReadData(index, new BSLimbIKModifier(this, ref))){
                        return false;
                    }
                }else if (signature == BS_INTERP_VALUE_MODIFIER){
                    if (!appendAndReadData(index, new BSInterpValueModifier(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_FOOT_IK_CONTROLS_MODIFIER){
                    if (!appendAndReadData(index, new hkbFootIkControlsModifier(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_GET_HANDLE_ON_BONE_MODIFIER){
                    if (!appendAndReadData(index, new hkbGetHandleOnBoneModifier(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_TRANSFORM_VECTOR_MODIFIER){
                    if (!appendAndReadData(index, new hkbTransformVectorModifier(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_PROXY_MODIFIER){
                    if (!appendAndReadData(index, new hkbProxyModifier(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_LOOK_AT_MODIFIER){
                    if (!appendAndReadData(index, new hkbLookAtModifier(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_MIRROR_MODIFIER){
                    if (!appendAndReadData(index, new hkbMirrorModifier(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_GET_WORLD_FROM_MODEL_MODIFIER){
                    if (!appendAndReadData(index, new hkbGetWorldFromModelModifier(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_SENSE_HANDLE_MODIFIER){
                    if (!appendAndReadData(index, new hkbSenseHandleModifier(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_EVALUATE_EXPRESSION_MODIFIER){
                    if (!appendAndReadData(index, new hkbEvaluateExpressionModifier(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_EVALUATE_HANDLE_MODIFIER){
                    if (!appendAndReadData(index, new hkbEvaluateHandleModifier(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_ATTACHMENT_MODIFIER){
                    if (!appendAndReadData(index, new hkbAttachmentModifier(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_ATTRIBUTE_MODIFIER){
                    if (!appendAndReadData(index, new hkbAttributeModifier(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_COMBINE_TRANSFORMS_MODIFIER){
                    if (!appendAndReadData(index, new hkbCombineTransformsModifier(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_COMPUTE_ROTATION_FROM_AXIS_ANGLE_MODIFIER){
                    if (!appendAndReadData(index, new hkbComputeRotationFromAxisAngleModifier(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_COMPUTE_ROTATION_TO_TARGET_MODIFIER){
                    if (!appendAndReadData(index, new hkbComputeRotationToTargetModifier(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_EVENTS_FROM_RANGE_MODIFIER){
                    if (!appendAndReadData(index, new hkbEventsFromRangeModifier(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_MOVE_CHARACTER_MODIFIER){
                    if (!appendAndReadData(index, new hkbMoveCharacterModifier(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_EXTRACT_RAGDOLL_POSE_MODIFIER){
                    if (!appendAndReadData(index, new hkbExtractRagdollPoseModifier(this, ref))){
                        return false;
                    }
                }else if (signature == BS_MODIFY_ONCE_MODIFIER){
                    if (!appendAndReadData(index, new BSModifyOnceModifier(this, ref))){
                        return false;
                    }
                }else if (signature == BS_EVENT_ON_DEACTIVATE_MODIFIER){
                    if (!appendAndReadData(index, new BSEventOnDeactivateModifier(this, ref))){
                        return false;
                    }
                }else if (signature == BS_EVENT_EVERY_N_EVENTS_MODIFIER){
                    if (!appendAndReadData(index, new BSEventEveryNEventsModifier(this, ref))){
                        return false;
                    }
                }else if (signature == BS_RAGDOLL_CONTACT_LISTENER_MODIFIER){
                    if (!appendAndReadData(index, new BSRagdollContactListenerModifier(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_POWERED_RAGDOLL_CONTROLS_MODIFIER){
                    if (!appendAndReadData(index, new hkbPoweredRagdollControlsModifier(this, ref))){
                        return false;
                    }
                }else if (signature == BS_EVENT_ON_FALSE_TO_TRUE_MODIFIER){
                    if (!appendAndReadData(index, new BSEventOnFalseToTrueModifier(this, ref))){
                        return false;
                    }
                }else if (signature == BS_DIRECT_AT_MODIFIER){
                    if (!appendAndReadData(index, new BSDirectAtModifier(this, ref))){
                        return false;
                    }
                }else if (signature == BS_DIST_TRIGGER_MODIFER){
                    if (!appendAndReadData(index, new BSDistTriggerModifier(this, ref))){
                        return false;
                    }
                }else if (signature == BS_DECOMPOSE_VECTOR_MODIFIER){
                    if (!appendAndReadData(index, new BSDecomposeVectorModifier(this, ref))){
                        return false;
                    }
                }else if (signature == BS_COMPUTE_ADD_BONE_ANIM_MODIFIER){
                    if (!appendAndReadData(index, new BSComputeAddBoneAnimModifier(this, ref))){
                        return false;
                    }
                }else if (signature == BS_TWEENER_MODIFIER){
                    if (!appendAndReadData(index, new BSTweenerModifier(this, ref))){
                        return false;
                    }
                }else if (signature == BS_I_STATE_MANAGER_MODIFIER){
                    if (!appendAndReadData(index, new BSIStateManagerModifier(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_TIMER_MODIFIER){
                    if (!appendAndReadData(index, new hkbTimerModifier(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_ROTATE_CHARACTER_MODIFIER){
                    if (!appendAndReadData(index, new hkbRotateCharacterModifier(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_DAMPING_MODIFIER){
                    if (!appendAndReadData(index, new hkbDampingModifier(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_DELAYED_MODIFIER){
                    if (!appendAndReadData(index, new hkbDelayedModifier(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_GET_UP_MODIFIER){
                    if (!appendAndReadData(index, new hkbGetUpModifier(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_EXPRESSION_DATA_ARRAY){
                    if (!appendAndReadData(index, new hkbExpressionDataArray(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_BONE_INDEX_ARRAY){
                    if (!appendAndReadData(index, new hkbBoneIndexArray(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_KEY_FRAME_BONES_MODIFIER){
                    if (!appendAndReadData(index, new hkbKeyframeBonesModifier(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_COMPUTE_DIRECTION_MODIFIER){
                    if (!appendAndReadData(index, new hkbComputeDirectionModifier(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_RIGID_BODY_RAGDOLL_CONTROLS_MODIFIER){
                    if (!appendAndReadData(index, new hkbRigidBodyRagdollControlsModifier(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_POSE_MATCHING_GENERATOR){
                    if (!appendAndReadData(index, new hkbPoseMatchingGenerator(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_CLIP_GENERATOR){
                    if (!appendAndReadData(index, new hkbClipGenerator(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_CLIP_TRIGGER_ARRAY){
                    if (!appendAndReadData(index, new hkbClipTriggerArray(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_GENERATOR_TRANSITION_EFFECT){
                    if (!appendAndReadData(index, new hkbGeneratorTransitionEffect(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_BEHAVIOR_REFERENCE_GENERATOR){
                    if (!appendAndReadData(index, new hkbBehaviorReferenceGenerator(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_STATE_MACHINE_TRANSITION_INFO_ARRAY){
                    if (!appendAndReadData(index, new hkbStateMachineTransitionInfoArray(this, NULL, ref))){
                        return false;
                    }
                }else if (signature == HKB_STATE_MACHINE_EVENT_PROPERTY_ARRAY){
                    if (!appendAndReadData(index, new hkbStateMachineEventPropertyArray(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_STRING_EVENT_PAYLOAD){
                    if (!appendAndReadData(index, new hkbStringEventPayload(this, "", ref))){
                        return false;
                    }
                }else if (signature == BS_SPEED_SAMPLER_MODIFIER){
                    if (!appendAndReadData(index, new BSSpeedSamplerModifier(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_DETECT_CLOSE_TO_GROUND_MODIFIER){
                    if (!appendAndReadData(index, new hkbDetectCloseToGroundModifier(this, ref))){
                        return false;
                    }
                }else if (signature == BS_PASS_BY_TARGET_TRIGGER_MODIFIER){
                    if (!appendAndReadData(index, new BSLookAtModifier(this, ref))){
                        return false;
                    }
                }else if (signature == BS_TIMER_MODIFIER){
                    if (!appendAndReadData(index, new BSTimerModifier(this, ref))){
                        return false;
                    }
                }else if (signature == BS_LOOK_AT_MODIFIER){
                    if (!appendAndReadData(index, new BSLookAtModifier(this, ref))){
                        return false;
                    }
                }else if (signature == BGS_GAMEBYRO_SEQUENCE_GENERATOR){
                    if (!appendAndReadData(index, new BGSGamebryoSequenceGenerator(this, ref))){
                        return false;
                    }
                }else if (signature == HKB_BEHAVIOR_GRAPH){
                    if (!appendAndReadData(index, new hkbBehaviorGraph(this, ref))){
                        return false;
                    }
                    behaviorGraph = generators.last();
                    generators.removeLast();
                }else if (signature == HKB_BEHAVIOR_GRAPH_DATA){
                    if (!appendAndReadData(index, new hkbBehaviorGraphData(this, ref))){
                        return false;
                    }
                    graphData = otherTypes.last();
                    otherTypes.removeLast();
                }else if (signature == HKB_BEHAVIOR_GRAPH_STRING_DATA){
                    if (!appendAndReadData(index, new hkbBehaviorGraphStringData(this, ref))){
                        return false;
                    }
                    stringData = otherTypes.last();
                    otherTypes.removeLast();
                }else if (signature == HKB_VARIABLE_VALUE_SET){
                    if (!appendAndReadData(index, new hkbVariableValueSet(this, ref))){
                        return false;
                    }
                    variableValues = otherTypes.last();
                    otherTypes.removeLast();
                }else if (signature == HK_ROOT_LEVEL_CONTAINER){
                    if (!appendAndReadData(index, new hkRootLevelContainer(this, ref))){
                        return false;
                    }
                    setRootObject(otherTypes.last());
                    otherTypes.removeLast();
                }else{
                    writeToLog("BehaviorFile: parse()!\nUnknown signature detected!\nUnknown object class name is: "+reader.getNthAttributeValueAt(index, 1)+"\nUnknown object signature is: "+QString::number(signature, 16));
                }
            }
        }
        index++;
    }
    closeFile();
    setProgressData("Linking HKX objects...", 80);
    if (!link()){
        writeToLog("BehaviorFile: parse() failed because link() failed!", true);
        return false;
    }
    return true;
}

bool BehaviorFile::link(){
    if (!getRootObject().constData()){
        writeToLog("BehaviorFile: link() failed!\nThe root object of this behavior file is NULL!", true);
        return false;
    }else if (getRootObject()->getSignature() != HK_ROOT_LEVEL_CONTAINER){
        writeToLog("BehaviorFile: link() failed!\nThe root object of this behavior file is NOT a hkRootLevelContainer!\nThe root object signature is: "+QString::number(getRootObject()->getSignature(), 16), true);
        return false;
    }
    if (!getRootObject().data()->link()){
        writeToLog("BehaviorFile: link() failed!\nThe root object of this behavior file failed to link to it's children!", true);
        return false;
    }
    for (int i = generators.size() - 1; i >= 0; i--){
        if (!generators.at(i).data()->link()){
            writeToLog("BehaviorFile: link() failed!\nA generator failed to link to it's children!\nObject signature: "+QString::number(generators.at(i)->getSignature(), 16)+"\nObject reference: "+QString::number(generators.at(i).getReference()), true);
            return false;
        }
    }
    for (int i = modifiers.size() - 1; i >= 0; i--){
        if (!modifiers.at(i).data()->link()){
            writeToLog("BehaviorFile: link() failed!\nA modifier failed to link to it's children!\nObject signature: "+QString::number(modifiers.at(i)->getSignature(), 16)+"\nObject reference: "+QString::number(generators.at(i).getReference()), true);
            return false;
        }
    }
    for (int i = generatorChildren.size() - 1; i >= 0; i--){
        if (!generatorChildren.at(i).data()->link()){
            writeToLog("BehaviorFile: link() failed!\nA generator child failed to link to it's children!\nObject signature: "+QString::number(generatorChildren.at(i)->getSignature(), 16)+"\nObject reference: "+QString::number(generators.at(i).getReference()), true);
            return false;
        }
    }
    for (int i = otherTypes.size() - 1; i >= 0; i--){
        if (!otherTypes.at(i).data()->link()){
            writeToLog("BehaviorFile: link() failed!\nA generator child failed to link to it's children!\nObject signature: "+QString::number(otherTypes.at(i)->getSignature(), 16)+"\nObject reference: "+QString::number(generators.at(i).getReference()), true);
            return false;
        }
    }
    if (!behaviorGraph.data()->link()){
        writeToLog("BehaviorFile: link() failed!\nhkbBehaviorGraph failed to link to it's children!\n", true);
        return false;
    }
    if (!variableValues.data()->link()){
        writeToLog("BehaviorFile: link() failed!\nhkbVariableValueSet failed to link to it's children!\n", true);
        return false;
    }
    if (!graphData.data()->link()){
        writeToLog("BehaviorFile: link() failed!\nhkbBehaviorGraphData failed to link to it's children!\n", true);
        return false;
    }
    return true;
}

QString BehaviorFile::getRootObjectReferenceString(){
    return getRootObject().data()->getReferenceString();
}

void BehaviorFile::write(){
    ulong ref = 1;
    getRootObject().data()->setReference(ref);
    ref++;
    stringData.data()->setReference(ref);
    ref++;
    variableValues.data()->setReference(ref);
    ref++;
    graphData.data()->setReference(ref);
    ref++;
    for (int i = 0; i < generators.size(); i++, ref++){
        generators.at(i).data()->setIsWritten(false);
        generators.at(i).data()->setReference(ref);
    }
    for (int i = 0; i < generatorChildren.size(); i++, ref++){
        generatorChildren.at(i).data()->setIsWritten(false);
        generatorChildren.at(i).data()->setReference(ref);
    }
    for (int i = 0; i < modifiers.size(); i++, ref++){
        modifiers.at(i).data()->setIsWritten(false);
        modifiers.at(i).data()->setReference(ref);
    }
    for (int i = 0; i < otherTypes.size(); i++, ref++){
        otherTypes.at(i).data()->setIsWritten(false);
        otherTypes.at(i).data()->setReference(ref);
    }
    writer.setBehaviorFile(this);
    writer.writeToXMLFile();
}

HkxObjectExpSharedPtr * BehaviorFile::findGenerator(long ref){
    if (ref < 0){
        return NULL;
    }
    for (int i = 0; i < generators.size(); i++){
        if (ref == generators.at(i).getReference()){
            return &generators[i];
        }
    }
    return NULL;
}

HkxObjectExpSharedPtr * BehaviorFile::findGeneratorChild(long ref){
    if (ref < 0){
        return NULL;
    }
    for (int i = 0; i < generatorChildren.size(); i++){
        if (ref == generatorChildren.at(i).getReference()){
            return &generatorChildren[i];
        }
    }
    return NULL;
}

HkxObjectExpSharedPtr * BehaviorFile::findModifier(long ref){
    if (ref < 0){
        return NULL;
    }
    for (int i = 0; i < modifiers.size(); i++){
        if (ref == modifiers.at(i).getReference()){
            return &modifiers[i];
        }
    }
    return NULL;
}

HkxObjectExpSharedPtr * BehaviorFile::findHkxObject(long ref){
    if (ref < 0){
        return NULL;
    }
    for (int i = 0; i < otherTypes.size(); i++){
        if (ref == otherTypes.at(i).getReference()){
            return &otherTypes[i];
        }
    }
    return NULL;
}

void BehaviorFile::removeBindings(int varIndex){
    for (int i = 0; i < otherTypes.size(); i++){
        if (otherTypes.at(i).data()->getSignature() == HKB_VARIABLE_BINDING_SET){
            static_cast<hkbVariableBindingSet *>(otherTypes.at(i).data())->removeBinding(varIndex);
        }
    }
}

HkxObjectExpSharedPtr * BehaviorFile::findBehaviorGraph(long ref){
    if (behaviorGraph.getReference() == ref){
        return &behaviorGraph;
    }
    return NULL;
}

QVector<int> BehaviorFile::removeGeneratorData(){
    QVector<int> removedIndices;
    HkxObject *obj = NULL;
    for (int i = generators.size() - 1; i >= 0; i--){
        obj = generators.at(i).data();
        if (obj){
            if (obj->ref < 2){
                generators.removeAt(i);
                removedIndices.append(i);
            }else if (obj->getSignature() == HKB_STATE_MACHINE){
                if (obj->ref <= static_cast<hkbStateMachine *>(obj)->getNumberOfStates() + 1){
                    generators.removeAt(i);
                    removedIndices.append(i);
                }
            }/*else if (obj->getSignature() == HKB_BLENDER_GENERATOR){
                if (obj->ref <= static_cast<hkbBlenderGenerator *>(obj)->getNumberOfChildren() + 1){
                    generators.removeAt(i);
                    removedIndices.append(i);
                }
            }else if (obj->getSignature() == HKB_POSE_MATCHING_GENERATOR){
                if (obj->ref <= static_cast<hkbPoseMatchingGenerator *>(obj)->getNumberOfChildren() + 1){
                    generators.removeAt(i);
                    removedIndices.append(i);
                }
            }*/
        }
    }
    return removedIndices;
}

QVector<int> BehaviorFile::removeModifierData(){
    QVector<int> removedIndices;
    for (int i = modifiers.size() - 1; i >= 0; i--){
        if (modifiers.at(i).constData() && modifiers.at(i).constData()->ref < 2){
            modifiers.removeAt(i);
            removedIndices.append(i);
        }
    }
    return removedIndices;
}

QVector<int> BehaviorFile::removeOtherData(){
    QVector<int> removedIndices;
    for (int i = otherTypes.size() - 1; i >= 0; i--){
        if (otherTypes.at(i).constData() && otherTypes.at(i).constData()->ref < 2){
            otherTypes.removeAt(i);
            removedIndices.append(i);
        }
    }
    return removedIndices;
}

hkVariableType BehaviorFile::getVariableTypeAt(int index) const{
    return static_cast<hkbBehaviorGraphData *>(graphData.data())->getVariableTypeAt(index);
}

QStringList BehaviorFile::getGeneratorNamesAndTypeNames() const{
    QStringList list;
    for (int i = 0; i < generators.size(); i++){
        if (generators.at(i).constData()->getType() == HkxObject::TYPE_GENERATOR){
            list.append(static_cast<hkbGenerator *>(generators.at(i).data())->getName());
            list.append(static_cast<hkbGenerator *>(generators.at(i).data())->getClassname());
        }
    }
    return list;
}

HkxObject * BehaviorFile::getBehaviorGraphData() const{
    return graphData.data();
}

int BehaviorFile::getIndexOfGenerator(const HkxObjectExpSharedPtr & obj) const{
    return generators.indexOf(obj);
}

bool BehaviorFile::setGeneratorData(HkxObjectExpSharedPtr & ptrToSet, int index){
    if (index >= 0 && index < generators.size()){
        ptrToSet = generators.at(index);
        return true;
    }
    return false;
}

hkbGenerator *BehaviorFile::getGeneratorDataAt(int index){
    if (index >= 0 && index < generators.size()){
        return static_cast<hkbGenerator *>(generators[index].data());
    }
    return NULL;
}

int BehaviorFile::getIndexOfModifier(const HkxObjectExpSharedPtr & obj) const{
    return modifiers.indexOf(obj);
}

bool BehaviorFile::setModifierData(HkxObjectExpSharedPtr & ptrToSet, int index){
    if (index >= 0 && index < modifiers.size()){
        ptrToSet = modifiers.at(index);
        return true;
    }
    return false;
}

HkxObject* BehaviorFile::getModifierDataAt(int index){
    if (index >= 0 && index < modifiers.size()){
        return modifiers[index].data();
    }
    return NULL;
}

QStringList BehaviorFile::getVariableNames() const{
    return static_cast<hkbBehaviorGraphData *>(graphData.data())->getVariableNames();
}

QStringList BehaviorFile::getEventNames() const{
    return static_cast<hkbBehaviorGraphData *>(graphData.data())->getEventNames();
}

BehaviorFile::~BehaviorFile(){
    //
}
