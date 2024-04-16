#ifndef DEFAULT_CONFIGURATIONS_H
#define DEFAULT_CONFIGURATIONS_H

#include "VMDC.h"

/**
 * @brief DefaultConfiguration - Create a default configuration for a given task
 * @details Since removing the option of configuring studies from the app, the configurations, which previously had to be in the back end, can now be both fixed
 * and generated in the back end. The comments in this section serve also to store variations in the parameter values in case we need them in the future.ñ
 * @param study_code - One of the task codes
 * @return The default configuration if the task code is defined or an empty map otherwise.
 */

namespace ConfigConsts {

    static const qint32 BINDING_SIZE_SHORT = 32;
    static const qint32 NBACK_HOLD_TIME    = 400;
    static const qint32 NBACK_TIMEOUT      = 5000;
    static const qint32 NBACK_TRANSITION   = 1000;
    static const qint32 SPHERES_SLOW       = 60;
    static const qint32 SPHERES_STD        = 100;
    static const qint32 SPHERES_VS_MIN     = 50;
    static const qint32 SPHERES_VS_INITIAL = 80;
    static const qint32 SPHERES_VS_MAX     = 100;
}


inline QVariantMap DefaultConfiguration(const QString &task_code){

    QVariantMap config;

    if ((task_code == VMDC::Study::BINDING_BC_2_SHORT) || (task_code == VMDC::Study::BINDING_UC_2_SHORT)){

        config[VMDC::StudyParameter::TARGET_SIZE] = VMDC::BindingTargetSize::LARGE; // Small is no longer implemented.
        config[VMDC::StudyParameter::NUMBER_TARGETS] = 2;
        config[VMDC::StudyParameter::NUM_TRIALS] = ConfigConsts::BINDING_SIZE_SHORT;  // -1 here would use all trials.
        config[VMDC::StudyParameter::IS_3D_STUDY] = false;
    }
    else if ((task_code == VMDC::Study::BINDING_BC_3_SHORT) || (task_code == VMDC::Study::BINDING_UC_3_SHORT)){
        config[VMDC::StudyParameter::TARGET_SIZE] = VMDC::BindingTargetSize::LARGE; // Small is no longer implemented.
        config[VMDC::StudyParameter::NUMBER_TARGETS] = 3;
        config[VMDC::StudyParameter::NUM_TRIALS] = ConfigConsts::BINDING_SIZE_SHORT;  // -1 here would use all trials.
        config[VMDC::StudyParameter::IS_3D_STUDY] = false;
    }
    else if (task_code == VMDC::Study::NBACK_3){

        config[VMDC::StudyParameter::NUMBER_TARGETS] = 3;
        config[VMDC::StudyParameter::IS_3D_STUDY] = false;
        config[VMDC::StudyParameter::NBACK_HOLD_TIME] = ConfigConsts::NBACK_HOLD_TIME;
        config[VMDC::StudyParameter::NBACK_TIMEOUT] = ConfigConsts::NBACK_TIMEOUT;
        config[VMDC::StudyParameter::NBACK_TRANSITION] = ConfigConsts::NBACK_TRANSITION;
        config[VMDC::StudyParameter::NBACK_LIGHT_ALL] = false;

        // For reference:
        // The Legacy NBack RT used the following values:
        // Hold Time: 250, TimeOut 3000 and Transition 500

    }
    else if (task_code == VMDC::Study::NBACK_4){
        config[VMDC::StudyParameter::NUMBER_TARGETS] = 4;
        config[VMDC::StudyParameter::IS_3D_STUDY] = false;
        config[VMDC::StudyParameter::NBACK_HOLD_TIME] = ConfigConsts::NBACK_HOLD_TIME;
        config[VMDC::StudyParameter::NBACK_TIMEOUT] = ConfigConsts::NBACK_TIMEOUT;
        config[VMDC::StudyParameter::NBACK_TRANSITION] = ConfigConsts::NBACK_TRANSITION;
        config[VMDC::StudyParameter::NBACK_LIGHT_ALL] = false;
    }
    else if (task_code == VMDC::Study::GONOGO){
        config[VMDC::StudyParameter::IS_3D_STUDY] = false;
    }
    else if (task_code == VMDC::Study::MOVING_DOT) {
        config[VMDC::StudyParameter::IS_3D_STUDY] = false;
    }
    else if (task_code == VMDC::Study::SPHERES){
        config[VMDC::StudyParameter::HAND_TO_USE] = "both";
        config[VMDC::StudyParameter::IS_3D_STUDY] = true;
        config[VMDC::StudyParameter::MIN_SPEED]       = ConfigConsts::SPHERES_STD;
        config[VMDC::StudyParameter::INITIAL_SPEED]   = ConfigConsts::SPHERES_STD;
        config[VMDC::StudyParameter::MAX_SPEED]       = ConfigConsts::SPHERES_STD;
    }
    else if (task_code == VMDC::Study::SPHERES_VS){
        config[VMDC::StudyParameter::HAND_TO_USE]     = "both";
        config[VMDC::StudyParameter::IS_3D_STUDY]     = true;
        config[VMDC::StudyParameter::MIN_SPEED]       = ConfigConsts::SPHERES_VS_MIN;
        config[VMDC::StudyParameter::INITIAL_SPEED]   = ConfigConsts::SPHERES_VS_INITIAL;
        config[VMDC::StudyParameter::MAX_SPEED]       = ConfigConsts::SPHERES_VS_MAX;
    }

    return config;

}

#endif // DEFAULT_CONFIGURATIONS_H
