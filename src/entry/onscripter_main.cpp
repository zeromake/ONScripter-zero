/* -*- C++ -*-
 *
 *  onscripter_main.cpp -- main function of ONScripter
 *
 *  Copyright (c) 2001-2018 Ogapee. All rights reserved.
 *            (C) 2014-2019 jh10001 <jh10001@live.cn>
 *
 *  ogapee@aqua.dti2.ne.jp
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdlib.h>

#include <infra/filesystem.hpp>

#include "ONScripter.h"
#include "gbk2utf16.h"
#include "private/utils.h"
#include "sjis2utf16.h"
#include "version.h"

ONScripter ons;
Coding2UTF16 *coding2utf16 = NULL;

#ifdef _WIN32
#include <direct.h>

#define chdir _chdir
#else
#include <unistd.h>
#endif

#ifdef ANDROID
#include <unistd.h>
#endif

#ifdef WINRT
#include "ScriptSelector.h"
#endif

#ifdef ONSCRIPTER_EXTEND_INIT
#include "onscripter_init.h"
#endif

void optionHelp() {
    utils::printInfo("Usage: onscripter [option ...]\n");
    utils::printInfo("      --cdaudio\t\tuse CD audio if available\n");
    utils::printInfo("      --cdnumber no\tchoose the CD-ROM drive number\n");
    utils::printInfo("  -f, --font file\tset a TTF font file\n");
    utils::printInfo("      --registry file\tset a registry file\n");
    utils::printInfo("      --dll file\tset a dll file\n");
    utils::printInfo("  -r, --root path\tset the root path to the archives\n");
    utils::printInfo("      --fullscreen\tstart in fullscreen mode\n");
    utils::printInfo("      --window\t\tstart in windowed mode\n");
    utils::printInfo(
        "      --force-button-shortcut\tignore useescspc and getenter "
        "command\n");
    utils::printInfo(
        "      --enable-wheeldown-advance\tadvance the text on mouse wheel "
        "down\n");
    utils::printInfo(
        "      --rescale \tdo rescale the images in the archives\n");
    utils::printInfo(
        "      --disable-rescale\tdo not rescale the images in the archives\n");
    utils::printInfo(
        "      --render-font-outline\trender the outline of a text instead of "
        "casting a shadow\n");
    utils::printInfo(
        "      --edit\t\tenable online modification of the volume and "
        "variables when 'z' is pressed\n");
    utils::printInfo(
        "      --key-exe file\tset a file (*.EXE) that includes a key table\n");
    utils::printInfo("      --enc:sjis|gbk\tuse sjis|gbk coding script\n");
    utils::printInfo("      --debug:1\t\tprint debug info\n");
    utils::printInfo("      --fontcache\tcache default font\n");
    utils::printInfo("  -h, --help\t\tshow this help and exit\n");
    utils::printInfo(
        "  -v, --version\t\tshow the version information and exit\n");
    exit(0);
}

void optionVersion() {
    utils::printInfo("Copyright:\n");
    utils::printInfo("  (C) 2001-2018 Ogapee.\n");
    utils::printInfo("  (C) 2014-2018 jh10001<jh10001@live.cn>.\n");
    utils::printInfo("  (C) 2022-2023 zeromake<a390720046@gmail.com>.\n\n");
    utils::printInfo("Origin Written by Ogapee <ogapee@aqua.dti2.ne.jp>\n\n");
    utils::printInfo(
        "This is free software; see the source for copying conditions.\n");
    int count = SDL_GetNumRenderDrivers();
    std::string renderDrivers = "";
    SDL_RendererInfo info;
    for (int i = 0; i < count; i++) {
        SDL_GetRenderDriverInfo(i, &info);
        renderDrivers += info.name;
        renderDrivers += " ";
    }
    utils::printInfo("\nSuppert:\n");
    utils::printInfo("  Render => %s\n", renderDrivers.c_str());
    count = SDL_GetNumAudioDrivers();
    std::string audioDrivers = "";
    for (int i = 0; i < count; i++) {
        auto audioDriverName = SDL_GetAudioDriver(i);
        audioDrivers += audioDriverName;
        audioDrivers += " ";
    }
    utils::printInfo("  Audio  => %s\n\n", audioDrivers.c_str());

    exit(0);
}

#ifdef ANDROID
extern "C" {
#include <android/log.h>
#include <errno.h>
#include <jni.h>
static jobject JavaONScripter = NULL;
static jmethodID JavaPlayVideo = NULL;
static jint JNI_VERSION = JNI_VERSION_1_4;
static JavaVM *jniVM = NULL;
static jmethodID JavaGetFD = NULL;
static jmethodID JavaMkdir = NULL;

JNIEXPORT jint JNI_OnLoad(JavaVM *jvm, void *reserved) {
    jniVM = jvm;
    return JNI_VERSION;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved) { jniVM = vm; };

JNIEnv *GetJniEnv() {
    JNIEnv *m_pJniEnv;
    int nEnvStat =
        jniVM->GetEnv(reinterpret_cast<void **>(&m_pJniEnv), JNI_VERSION);
    if (nEnvStat == JNI_EDETACHED) {
        JavaVMAttachArgs args;
        args.version = JNI_VERSION;
        if (jniVM->AttachCurrentThread(&m_pJniEnv, &args) != 0) {
            return nullptr;
        }
        thread_local struct DetachJniOnExit {
            ~DetachJniOnExit() { jniVM->DetachCurrentThread(); }
        } detachJniOnExit;
    } else if (nEnvStat == JNI_EVERSION) {
        return nullptr;
    }
    return m_pJniEnv;
}

#ifndef SDL_JAVA_PACKAGE_PATH
#error You have to define SDL_JAVA_PACKAGE_PATH to your package path with dots replaced with underscores, for example "com_example_SanAngeles"
#endif
#define JAVA_EXPORT_NAME2(name, package) Java_##package##_##name
#define JAVA_EXPORT_NAME1(name, package) JAVA_EXPORT_NAME2(name, package)
#define JAVA_EXPORT_NAME(name) JAVA_EXPORT_NAME1(name, SDL_JAVA_PACKAGE_PATH)

JNIEXPORT jint JNICALL
JAVA_EXPORT_NAME(ONScripter_nativeGetWidth)(JNIEnv *env, jobject thiz) {
    return ons.getWidth();
}

JNIEXPORT jint JNICALL
JAVA_EXPORT_NAME(ONScripter_nativeGetHeight)(JNIEnv *env, jobject thiz) {
    return ons.getHeight();
}

JNIEXPORT jint JNICALL JAVA_EXPORT_NAME(ONScripter_nativeInitJavaCallbacks)(
    JNIEnv *env, jobject thiz) {
    JavaONScripter = env->NewGlobalRef(thiz);
    jclass JavaONScripterClass = env->GetObjectClass(JavaONScripter);
    JavaPlayVideo = env->GetMethodID(JavaONScripterClass, "playVideo", "([C)V");
    return 0;
}

void playVideoAndroid(const char *filename) {
    JNIEnv *jniEnv = GetJniEnv();
    if (!jniEnv) {
        utils::printError(
            "ONScripter::playVideoAndroid: Java VM AttachCurrentThread() "
            "failed");
        return;
    }

    jchar *jc = new jchar[strlen(filename)];
    for (int i = 0; i < strlen(filename); i++) jc[i] = filename[i];
    jcharArray jca = jniEnv->NewCharArray(strlen(filename));
    jniEnv->SetCharArrayRegion(jca, 0, strlen(filename), jc);
    jniEnv->CallVoidMethod(JavaONScripter, JavaPlayVideo, jca);
    jniEnv->DeleteLocalRef(jca);
    delete[] jc;
}
}

#include <dlfcn.h>
#include <unwind.h>

#include <iomanip>
#include <iostream>

struct BacktraceState {
    void **current;
    void **end;
};

static _Unwind_Reason_Code unwindCallback(struct _Unwind_Context *context,
                                          void *arg) {
    BacktraceState *state = static_cast<BacktraceState *>(arg);
    uintptr_t pc = _Unwind_GetIP(context);
    if (pc) {
        if (state->current == state->end) {
            return _URC_END_OF_STACK;
        } else {
            *state->current++ = reinterpret_cast<void *>(pc);
        }
    }
    return _URC_NO_REASON;
}

void dumpBacktrace(std::ostream &os, void **buffer, size_t max) {
    BacktraceState state = {buffer, buffer + max};
    _Unwind_Backtrace(unwindCallback, &state);
    size_t count = state.current - buffer;
    for (size_t idx = 0; idx < count; ++idx) {
        const void *addr = buffer[idx];
        const char *symbol = "";
        Dl_info info;
        if (dladdr(addr, &info) && info.dli_sname) {
            symbol = info.dli_sname;
        }
        os << "  #" << std::setw(2) << idx << ": " << addr << "  " << symbol
           << "\n";
    }
}
#endif

void parseOption(int argc, char *argv[]) {
    while (argc > 0) {
        if (argv[0][0] == '-') {
            if (!strcmp(argv[0] + 1, "h") || !strcmp(argv[0] + 1, "-help")) {
                optionHelp();
            } else if (!strcmp(argv[0] + 1, "v") ||
                       !strcmp(argv[0] + 1, "-version")) {
                optionVersion();
            } else if (!strcmp(argv[0] + 1, "-cdaudio")) {
                ons.enableCDAudio();
            } else if (!strcmp(argv[0] + 1, "-cdnumber")) {
                argc--;
                argv++;
                ons.setCDNumber(atoi(argv[0]));
            } else if (!strcmp(argv[0] + 1, "f") ||
                       !strcmp(argv[0] + 1, "-font")) {
                argc--;
                argv++;
                ons.setFontFile(argv[0]);
            } else if (!strcmp(argv[0] + 1, "-registry")) {
                argc--;
                argv++;
                ons.setRegistryFile(argv[0]);
            } else if (!strcmp(argv[0] + 1, "-dll")) {
                argc--;
                argv++;
                ons.setDLLFile(argv[0]);
            } else if (!strcmp(argv[0] + 1, "r") ||
                       !strcmp(argv[0] + 1, "-root")) {
                argc--;
                argv++;
                auto root = std::fs::absolute(argv[0]).string();
                ons.setArchivePath(root.data());
            } else if (!strcmp(argv[0] + 1, "-fullscreen")) {
                ons.setFullscreenMode();
            } else if (!strcmp(argv[0] + 1, "-window")) {
                ons.setWindowMode();
            } else if (!strcmp(argv[0] + 1, "-width")) {
                argc--;
                argv++;
                ons.setWindowWidth(atoi(argv[0]));
            } else if (!strcmp(argv[0] + 1, "-height")) {
                argc--;
                argv++;
                ons.setWindowHeight(atoi(argv[0]));
            } else if (!strcmp(argv[0] + 1, "-sharpness")) {
                argc--;
                argv++;
                ons.setSharpness(atof(argv[0]));
            } else if (!strcmp(argv[0] + 1, "-force-button-shortcut")) {
                ons.enableButtonShortCut();
            } else if (!strcmp(argv[0] + 1, "-enable-wheeldown-advance")) {
                ons.enableWheelDownAdvance();
            } else if (!strcmp(argv[0] + 1, "-disable-rescale")) {
                ons.disableRescale();
            } else if (!strcmp(argv[0] + 1, "-render-font-outline")) {
            } else if (!strcmp(argv[0] + 1, "-edit")) {
                ons.enableEdit();
            } else if (!strcmp(argv[0] + 1, "-key-exe")) {
                argc--;
                argv++;
                ons.setKeyEXE(argv[0]);
            } else if (!strcmp(argv[0] + 1, "-enc:sjis")) {
                if (coding2utf16 == NULL) coding2utf16 = new SJIS2UTF16();
            } else if (!strcmp(argv[0] + 1, "-enc:gbk")) {
                if (coding2utf16 == NULL) coding2utf16 = new GBK2UTF16();
            } else if (!strcmp(argv[0] + 1, "-rescale")) {
                argc--;
                argv++;
                int screen_ratio1 = atof(argv[0]) * 100;
                int screen_ratio2 = 100;
                ons.setRescale(screen_ratio1, screen_ratio2);
            } else if (!strcmp(argv[0] + 1, "-debug:1")) {
                ons.setDebugLevel(1);
            } else if (!strcmp(argv[0] + 1, "-fontcache")) {
                ons.setFontCache();
            } else if (!strcmp(argv[0] + 1, "-no-vsync")) {
                ons.setVsyncOff();
            } else if (!strcmp(argv[0] + 1, "-scale-window")) {
                // 强制缩放渲染到窗口大小，与 rescale 选项同时用时 rescale
                // 生效，scale-window 失效
                ons.setScaleToWindow();
            } else if (!strcmp(argv[0] + 1, "-font-config")) {
                argc--;
                argv++;
                ons.setFontConfig(argv[0]);
            }
#if defined(ANDROID)
            else if (!strcmp(argv[0] + 1, "-save-dir")) {
                argc--;
                argv++;
                ons.setSaveDir(argv[0]);
            }
#endif
            else {
                utils::printInfo(" unknown option %s\n", argv[0]);
            }
        } else {
            auto rootPath = std::fs::absolute(argv[0]);
            if (std::fs::is_directory(rootPath)) {
                auto root = rootPath.string();
                ons.setArchivePath(root.data());
            } else {
                optionHelp();
            }
        }
        argc--;
        argv++;
    }
}

#if 0

struct sigaction old_handlers[18];
#define CATCHSIG(X) sigaction(X, &handler, &old_handlers[X])

void android_sigaction(int signal, siginfo_t *info, void *reserved) {
    // utils::printInfo("sigaction: %d", signal);
    // JNIEnv * jniEnv = NULL;
    // gJvm->AttachCurrentThread(&jniEnv, NULL);
    // if (!jniEnv) {  
    //     return;
    // }
    // utils::printInfo("onNativeCrashed");
    // jniEnv->CallStaticVoidMethod(JavaONScripterClass, JavaOnNativeCrashed);
    // utils::printInfo("onNativeCrashed done");
    old_handlers[signal].sa_sigaction(signal, info, reserved);
} 

void InitCrashReport(){
    utils::printInfo("InitCrashReport");
    // Try to catch crashes...
    struct sigaction handler;
    memset(&handler, 0, sizeof(struct sigaction));
    handler.sa_sigaction = android_sigaction;
    handler.sa_flags = SA_RESETHAND;

    CATCHSIG(SIGILL);
    CATCHSIG(SIGABRT);
    CATCHSIG(SIGBUS);
    CATCHSIG(SIGFPE);
    CATCHSIG(SIGSEGV);
    CATCHSIG(SIGSTKFLT);
    CATCHSIG(SIGPIPE);
}
#endif

#ifndef ONSCRIPTER_MAIN_EXTREN
#define ONSCRIPTER_MAIN_EXTREN
#endif

#ifdef ONSCRIPTER_UNDEF_MAIN
#undef main
#endif
#ifdef ONSCRIPTER_MAIN_RENAME
#undef main
#define main ONSCRIPTER_MAIN_RENAME
#endif

// #if defined(ANDROID)
// #include <sstream>

// int SDL_main(int argc, char *argv[]) {
// const size_t max = 30;
// void* buffer[max];
// std::ostringstream oss;
// dumpBacktrace(oss, buffer, max);
// utils::printInfo("dumpBacktrace: %s", oss.str().c_str());
// #else
// #undef main
ONSCRIPTER_MAIN_EXTREN int main(int argc, char *argv[]) {
    // #endif
    utils::printInfo("Version:\n");
    utils::printInfo("  ONScripter-zero\t%s\n", ONS_ZERO_VERSION);
    utils::printInfo("  ONScripter-Jh\t\t%s\n", ONS_JH_VERSION);
    utils::printInfo("  ONScripter\t\t%s\n", ONS_VERSION);
    utils::printInfo("  NScriper\t\t%.2f\n", (float)NSC_VERSION / 100);

#if defined(PSP)
    ons.disableRescale();
    ons.enableButtonShortCut();
    SetupCallbacks();
#elif defined(WINRT)
    {
        ScriptSelector ss;
        ons.setArchivePath(ss.selectedPath.c_str());
    }
    ons.disableRescale();
#elif defined(ANDROID)
    ons.enableButtonShortCut();
#endif

#ifdef ONSCRIPTER_EXTEND_INIT
    onscripter_init(&ons, argc, argv);
#endif

    // ----------------------------------------
    // Parse options
    argv++;
    parseOption(argc - 1, argv);
    const char *argfilename = "ons_args";
    FILE *fp = NULL;
    if (ons.getArchivePath()) {
        size_t len = strlen(ons.getArchivePath()) + strlen(argfilename) + 1;
        char *full_path = new char[len]{0};
        snprintf(full_path, len, "%s%s", ons.getArchivePath(), argfilename);
        fp = fopen(full_path, "r");
        delete[] full_path;
    } else
        fp = fopen(argfilename, "r");
    if (fp) {
        char **args = new char *[16];
        int argn = 0;
        args[argn] = new char[64]{0};
        while (argn < 16 && (fscanf(fp, "%s", args[argn]) > 0)) {
            ++argn;
            if (argn < 16) args[argn] = new char[64]{0};
        }
        parseOption(argn, args);
        for (int i = 0; i < argn; ++i) delete[] args[i];
        delete[] args;
    }

    if (coding2utf16 == NULL) coding2utf16 = new GBK2UTF16();

    // ----------------------------------------
    // Run ONScripter
    if (ons.openScript()) exit(-1);
    if (ons.init()) exit(-1);
    ons.executeLabel();
    exit(0);
}
