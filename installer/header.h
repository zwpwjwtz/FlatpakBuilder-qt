#ifndef HEADER_H
#define HEADER_H

/*
 * This header file is a template, which holds the necessary
 * information for installer to extract flatpak bundle from
 * its payload, and to verify its integrity.
 * All values should be passed to compiler during the final
 * compilation of the installer.
 */

/* Installer header information */
#ifdef INSTALLER_HEADER_SIZE
#define FPBUNDLE_INSTALLER_HEADER_SIZE INSTALLER_HEADER_SIZE
#else
#define FPBUNDLE_INSTALLER_HEADER_SIZE 65536
#endif


/* Installer payload information */
#ifdef FLATPAK_BUNDLE_NAME
#define FPBUNDLE_INSTALLER_PAYLOAD_NAME FLATPAK_BUNDLE_NAME
#else
#define FPBUNDLE_INSTALLER_PAYLOAD_NAME "bundle"
#endif

#ifdef FLATPAK_BUNDLE_SIZE
#define FPBUNDLE_INSTALLER_PAYLOAD_SIZE FLATPAK_BUNDLE_SIZE
#else
#define FPBUNDLE_INSTALLER_PAYLOAD_SIZE 0
#endif

#ifdef FLATPAK_BUNDLE_CHECKSUM_SHA256
#define FPBUNDLE_INSTALLER_PAYLOAD_SHA256SUM FLATPAK_BUNDLE_SHA256
#else
#define FPBUNDLE_INSTALLER_PAYLOAD_SHA256SUM ""
#endif

#endif // HEADER_H
