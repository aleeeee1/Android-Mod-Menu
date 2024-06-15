#include <list>
#include <vector>
#include <string.h>
#include <pthread.h>
#include <thread>
#include <cstring>
#include <jni.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <dlfcn.h>
#include "Includes/Logger.h"
#include "Includes/obfuscate.h"
#include "Includes/Utils.h"
#include "KittyMemory/MemoryPatch.h"
#include "Menu/Setup.h"

//Target lib here
#define targetLibName OBFUSCATE("libil2cpp.so")

#include "Includes/Macros.h"

bool passThru = false,
     oldPassThru = passThru,

     patchPlayerSpeed = false,
     patchJumpHeight = false;

float playerSpeed = 10.0,
      jumpHeight = 20.0;

void (*set_ColliderEnable)(void *instance, bool value);
void (*old_get_WorldBodyCenterPosition)(void *instance);
void get_WorldBodyCenterPosition(void *instance) {
    if (instance != NULL && oldPassThru != passThru) {
        oldPassThru = passThru;
        set_ColliderEnable(instance, !passThru);
    }

    old_get_WorldBodyCenterPosition(instance);
}

float (*old_get_MinSpeed)(void *instance);
float get_MinSpeed(void *instance) {
    if (instance != NULL && patchPlayerSpeed) {
        return (float) playerSpeed;
    }
    return old_get_MinSpeed(instance);
}

float (*old_get_MaxSpeed)(void *instance);
float get_MaxSpeed(void *instance) {
    if (instance != NULL && patchPlayerSpeed) {
        return (float) playerSpeed;
    }
    return old_get_MaxSpeed(instance);
}

float (*old_get_JumpHeight)(void *instance);
float get_JumpHeight(void *instance) {
    if (instance != NULL && patchJumpHeight) {
        return (float) jumpHeight;
    }
    return old_get_JumpHeight(instance);
}


// we will run our hacks in a new thread so our while loop doesn't block process main thread
void *hack_thread(void *) {
    LOGI(OBFUSCATE("pthread created"));

    //Check if target lib is loaded
    do {
        sleep(1);
    } while (!isLibraryLoaded(targetLibName));

    //Anti-lib rename
    /*
    do {
        sleep(1);
    } while (!isLibraryLoaded("libYOURNAME.so"));*/

    LOGI(OBFUSCATE("%s has been loaded"), (const char *) targetLibName);

#if defined(__aarch64__) || true //To compile this code for arm64 lib only. Do not worry about greyed out highlighting code, it still works

    set_ColliderEnable = (void (*) (void *, bool)) getAbsoluteAddress("libil2cpp.so", 0x10C6C58);
    HOOK_LIB("libil2cpp.so", "0x10C68E4", get_WorldBodyCenterPosition, old_get_WorldBodyCenterPosition);

    HOOK_LIB("libil2cpp.so", "0x27B1C90", get_MinSpeed, old_get_MinSpeed);
    HOOK_LIB("libil2cpp.so", "0x27B1E40", get_MaxSpeed, old_get_MaxSpeed);
    HOOK_LIB("libil2cpp.so", "0x27B2284", get_JumpHeight, old_get_JumpHeight);

    LOGI(OBFUSCATE("Hooked functions"));
#else

#endif
    //Anti-leech
    /*if (!iconValid || !initValid || !settingsValid) {
        //Bad function to make it crash
        sleep(5);
        int *p = 0;
        *p = 0;
    }*/

    return NULL;
}

// Do not change or translate the first text unless you know what you are doing
// Assigning feature numbers is optional. Without it, it will automatically count for you, starting from 0
// Assigned feature numbers can be like any numbers 1,3,200,10... instead in order 0,1,2,3,4,5...
// ButtonLink, Category, RichTextView and RichWebView is not counted. They can't have feature number assigned
// Toggle, ButtonOnOff and Checkbox can be switched on by default, if you add True_. Example: CheckBox_True_The Check Box
// To learn HTML, go to this page: https://www.w3schools.com/

jobjectArray GetFeatureList(JNIEnv *env, jobject context) {
    jobjectArray ret;

    const char *features[] = {
            OBFUSCATE("Category_Misc"),
            OBFUSCATE("1_Toggle_Purchase for free"),
            OBFUSCATE("2_Toggle_Don't consume items"),
            OBFUSCATE("3_Toggle_GodMode"),

            OBFUSCATE("Category_Powers"),
            OBFUSCATE("15_Toggle_Infinite Magnet time"),
            OBFUSCATE("16_Toggle_Infinite Double Score time"),
            OBFUSCATE("17_Toggle_Infinite Jetpack time"),
            OBFUSCATE("18_Toggle_Infinite Super Sneaker time"),
            OBFUSCATE("19_Toggle_Infinite board time"),

            OBFUSCATE("Category_Movement"),
            OBFUSCATE("4_Toggle_Pass thru things"),
            OBFUSCATE("11_Toggle_Infinite jump"),
            OBFUSCATE("7_Toggle_No gravity"),
            OBFUSCATE("14_Toggle_Auto jump"),

            OBFUSCATE("Category_Player speed mod"),
            OBFUSCATE("5_Toggle_Enabled"),
            OBFUSCATE("6_SeekBar_Value_0_8000"),

            OBFUSCATE("Category_Jump height mod"),
            OBFUSCATE("9_Toggle_Enabled"),
            OBFUSCATE("10_SeekBar_Value_0_2000"),

            OBFUSCATE("Category_Camera"),
            OBFUSCATE("8_Toggle_Lock camera"),
            OBFUSCATE("12_Toggle_Camera follows you"),

    };

    int Total_Feature = (sizeof features / sizeof features[0]);
    ret = (jobjectArray)
            env->NewObjectArray(Total_Feature, env->FindClass(OBFUSCATE("java/lang/String")),
                                env->NewStringUTF(""));

    for (int i = 0; i < Total_Feature; i++)
        env->SetObjectArrayElement(ret, i, env->NewStringUTF(features[i]));

    return (ret);
}

void Changes(JNIEnv *env, jclass clazz, jobject obj,
                                        jint featNum, jstring featName, jint value,
                                        jboolean boolean, jstring str) {

    LOGD(OBFUSCATE("Feature name: %d - %s | Value: = %d | Bool: = %d | Text: = %s"), featNum,
         env->GetStringUTFChars(featName, 0), value,
         boolean, str != NULL ? env->GetStringUTFChars(str, 0) : "");

    //BE CAREFUL NOT TO ACCIDENTLY REMOVE break;

    switch (featNum) {
        case 1:
            PATCH_LIB_SWITCH("libil2cpp.so", "0x1797498", "00 00 80 D2 C0 03 5F D6", boolean);
            PATCH_LIB_SWITCH("libil2cpp.so", "0x22A37EC", "20 00 80 D2 C0 03 5F D6", boolean);
            PATCH_LIB_SWITCH("libil2cpp.so", "0x22A3828", "20 00 80 D2 C0 03 5F D6", boolean);
            break;

        case 2:
            PATCH_LIB_SWITCH("libil2cpp.so", "0x22A3410", "1F 20 03 D5 C0 03 5F D6", boolean);
            break;

        case 3:
            PATCH_LIB_SWITCH("libil2cpp.so", "0x10C96E8", "00 00 80 D2 C0 03 5F D6", boolean);
            PATCH_LIB_SWITCH("libil2cpp.so", "0x10C9BD8", "00 00 80 D2 C0 03 5F D6", boolean);
            PATCH_LIB_SWITCH("libil2cpp.so", "0x10C74E8", "1F 20 03 D5 C0 03 5F D6", boolean);
            PATCH_LIB_SWITCH("libil2cpp.so", "0x10C9AD4", "1F 20 03 D5 C0 03 5F D6", boolean);
            break;

        case 4:
            passThru = boolean;
            break;

        case 5:
            patchPlayerSpeed = boolean;
            break;
        case 6:
            playerSpeed = value;
            break;

        case 7:
            PATCH_LIB_SWITCH("libil2cpp.so", "0x10C95D0", "1F 20 03 D5 C0 03 5F D6", boolean);
            break;

        case 8:
            PATCH_LIB_SWITCH("libil2cpp.so", "0x2B71414", "1F 20 03 D5 C0 03 5F D6", boolean);

        case 9:
            patchJumpHeight = boolean;
            break;
        case 10:
            jumpHeight = value;
            break;

        case 11:
            PATCH_LIB_SWITCH("libil2cpp.so", "0x10C7028", "20 00 80 D2 C0 03 5F D6", boolean);
            break;

        case 12:
            PATCH_LIB_SWITCH("libil2cpp.so", "0x2B72E70", "20 00 80 D2 C0 03 5F D6", boolean);
            break;

        case 14:
            PATCH_LIB_SWITCH("libil2cpp.so", "0x10C5AF4", "20 00 80 D2 C0 03 5F D6", boolean);
            break;

        case 15:
            PATCH_LIB_SWITCH("libil2cpp.so", "0x10C2A04", "40 00 80 D2 C0 03 5F D6", boolean);
            break;
        case 16:
            PATCH_LIB_SWITCH("libil2cpp.so", "0x10C2924", "40 00 80 D2 C0 03 5F D6", boolean);
            break;
        case 17:
            PATCH_LIB_SWITCH("libil2cpp.so", "0x10C2994", "40 00 80 D2 C0 03 5F D6", boolean);
            break;
        case 18:
            PATCH_LIB_SWITCH("libil2cpp.so", "0x10C2B7C", "40 00 80 D2 C0 03 5F D6", boolean);
            break;
        case 19:
            PATCH_LIB_SWITCH("libil2cpp.so", "0x10C0E54", "40 00 80 D2 C0 03 5F D6", boolean);
            break;
    }
}

__attribute__((constructor))
void lib_main() {
    // Create a new thread so it does not block the main thread, means the game would not freeze
    pthread_t ptid;
    pthread_create(&ptid, NULL, hack_thread, NULL);
}

int RegisterMenu(JNIEnv *env) {
    JNINativeMethod methods[] = {
            {OBFUSCATE("Icon"), OBFUSCATE("()Ljava/lang/String;"), reinterpret_cast<void *>(Icon)},
            {OBFUSCATE("IconWebViewData"),  OBFUSCATE("()Ljava/lang/String;"), reinterpret_cast<void *>(IconWebViewData)},
            {OBFUSCATE("IsGameLibLoaded"),  OBFUSCATE("()Z"), reinterpret_cast<void *>(isGameLibLoaded)},
            {OBFUSCATE("Init"),  OBFUSCATE("(Landroid/content/Context;Landroid/widget/TextView;Landroid/widget/TextView;)V"), reinterpret_cast<void *>(Init)},
            {OBFUSCATE("SettingsList"),  OBFUSCATE("()[Ljava/lang/String;"), reinterpret_cast<void *>(SettingsList)},
            {OBFUSCATE("GetFeatureList"),  OBFUSCATE("()[Ljava/lang/String;"), reinterpret_cast<void *>(GetFeatureList)},
    };

    jclass clazz = env->FindClass(OBFUSCATE("com/android/support/Menu"));
    if (!clazz)
        return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return JNI_ERR;
    return JNI_OK;
}

int RegisterPreferences(JNIEnv *env) {
    JNINativeMethod methods[] = {
            {OBFUSCATE("Changes"), OBFUSCATE("(Landroid/content/Context;ILjava/lang/String;IZLjava/lang/String;)V"), reinterpret_cast<void *>(Changes)},
    };
    jclass clazz = env->FindClass(OBFUSCATE("com/android/support/Preferences"));
    if (!clazz)
        return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return JNI_ERR;
    return JNI_OK;
}

int RegisterMain(JNIEnv *env) {
    JNINativeMethod methods[] = {
            {OBFUSCATE("CheckOverlayPermission"), OBFUSCATE("(Landroid/content/Context;)V"), reinterpret_cast<void *>(CheckOverlayPermission)},
    };
    jclass clazz = env->FindClass(OBFUSCATE("com/android/support/Main"));
    if (!clazz)
        return JNI_ERR;
    if (env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0])) != 0)
        return JNI_ERR;

    return JNI_OK;
}

extern "C"
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);
    if (RegisterMenu(env) != 0)
        return JNI_ERR;
    if (RegisterPreferences(env) != 0)
        return JNI_ERR;
    if (RegisterMain(env) != 0)
        return JNI_ERR;
    return JNI_VERSION_1_6;
}
