#ifndef MANIFEST_FORMAT_H
#define MANIFEST_FORMAT_H

/*
 * Manifest structure defined according to
 * Flatpak version 0.10.2
 * Please refer to Flatpak-builder manual for
 * more details and help support more parameters
 */

// Application property tags
#define FLATPAK_MANIFEST_PROP_APPID "app-id"
#define FLATPAK_MANIFEST_PROP_BRANCH "branch"
#define FLATPAK_MANIFEST_PROP_RTNAME "runtime"
#define FLATPAK_MANIFEST_PROP_RTVER "runtime-version"
#define FLATPAK_MANIFEST_PROP_SDKNAME "sdk"
#define FLATPAK_MANIFEST_PROP_COMMAND "command"
#define FLATPAK_MANIFEST_PROP_SEPLCL "separate-locales"
#define FLATPAK_MANIFEST_PROP_TAGS "tags"
#define FLATPAK_MANIFEST_PROP_FNARGS "finish-args"

// Application
#define FLATPAK_MANIFEST_FINARGS_SHARE_IPC "--share=ipc"
#define FLATPAK_MANIFEST_FINARGS_SHARE_NET "--share=network"
#define FLATPAK_MANIFEST_FINARGS_SOCKET_X11 "--socket=x11"
#define FLATPAK_MANIFEST_FINARGS_SOCKET_AUDIO "--socket=pulseaudio"
#define FLATPAK_MANIFEST_FINARGS_SOCKET_DBUS "--socket=system-bus"
#define FLATPAK_MANIFEST_FINARGS_DEVICE_DRI "--device=dri"
#define FLATPAK_MANIFEST_FINARGS_FS_HOST "--filesystem=host"
#define FLATPAK_MANIFEST_FINARGS_FS_HOME "--filesystem=home"
#define FLATPAK_MANIFEST_FINARGS_FS_READONLY ":ro"
#define FLATPAK_MANIFEST_FINARGS_OWN_NAME "--own-name="

// Application build option tags
#define FLATPAK_MANIFEST_PROP_BUILDOPT "build-options"
#define FLATPAK_MANIFEST_PROP_BUILD_CFLAG "cflags"
#define FLATPAK_MANIFEST_PROP_BUILD_CXXFLAG "cxxflags"
#define FLATPAK_MANIFEST_PROP_BUILD_CPPFLAG "cPPflags"
#define FLATPAK_MANIFEST_PROP_BUILD_LDFLAG "ldflags"
#define FLATPAK_MANIFEST_PROP_BUILD_PREFIX "prefix"
#define FLATPAK_MANIFEST_PROP_BUILD_CONFARG "config-opts"
#define FLATPAK_MANIFEST_PROP_BUILD_MAKEARG "make-args"
#define FLATPAK_MANIFEST_PROP_BUILD_MINSARG "make-install-args"
#define FLATPAK_MANIFEST_PROP_BUILD_STRIP "strip"
#define FLATPAK_MANIFEST_PROP_BUILD_NDBGINF "no-debuginfo"

// Module property tags
#define FLATPAK_MANIFEST_PROP_MODULES "modules"
#define FLATPAK_MANIFEST_PROP_MODULES_NAME "name"
#define FLATPAK_MANIFEST_PROP_MODULES_NAUTOGEN "no-autogen"
#define FLATPAK_MANIFEST_PROP_MODULES_NPARAMK "no-parallel-make"
#define FLATPAK_MANIFEST_PROP_MODULES_NMKINST "no-make-install"
#define FLATPAK_MANIFEST_PROP_MODULES_BUILDSYS "buildsystem"
#define FLATPAK_MANIFEST_PROP_MODULES_BUILDDIR "builddir"
#define FLATPAK_MANIFEST_PROP_MODULES_SUBDIR "subdir"
#define FLATPAK_MANIFEST_PROP_MODULES_BUILDCMD "build-commands"
#define FLATPAK_MANIFEST_PROP_MODULES_INSTRULE "install-rule"
#define FLATPAK_MANIFEST_PROP_MODULES_POSTINST "post-install"
#define FLATPAK_MANIFEST_PROP_MODULES_CLEANUP "cleanup"
#define FLATPAK_MANIFEST_PROP_MODULES_SOURCES "sources"

// Source property tags
#define FLATPAK_MANIFEST_PROP_SOURCES_TYPE "type"
#define FLATPAK_MANIFEST_PROP_SOURCES_URL "url"
#define FLATPAK_MANIFEST_PROP_SOURCES_BRANCH "branch"
#define FLATPAK_MANIFEST_PROP_SOURCES_CMDS "commands"
#define FLATPAK_MANIFEST_PROP_SOURCES_DESTFILE "dest-filename"

// Source type
#define FLATPAK_MANIFEST_SOURCE_TYPE_ARCHIVE "archive"
#define FLATPAK_MANIFEST_SOURCE_TYPE_GIT "git"
#define FLATPAK_MANIFEST_SOURCE_TYPE_BZR "bzr"
#define FLATPAK_MANIFEST_SOURCE_TYPE_FILE "file"
#define FLATPAK_MANIFEST_SOURCE_TYPE_SCRIPT "script"
#define FLATPAK_MANIFEST_SOURCE_TYPE_SHELL "shell"
#define FLATPAK_MANIFEST_SOURCE_TYPE_PATCH "patch"

#endif // MANIFEST_FORMAT_H
