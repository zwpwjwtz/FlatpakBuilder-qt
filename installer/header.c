#include "header.h"
#include "stdbool.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "fcntl.h"
#include "unistd.h"
#include "sys/mman.h"
#include <sys/wait.h>

#define FPBUNDLE_INSTALLER_BUNDLE_PREFIX ".fpinstaller_"

#define FPBUNDLE_INSTALLER_FLATPAK_EXECPATH "/usr/bin/flatpak"
#define FPBUNDLE_INSTALLER_FLATPAK_COMMAND "install"
#define FPBUNDLE_INSTALLER_FLATPAK_OPTION_BUNDLE "--bundle"
#define FPBUNDLE_INSTALLER_FLATPAK_OPTION_SYSTEM "--system"
#define FPBUNDLE_INSTALLER_FLATPAK_OPTION_USER "--user"


long filesize(const char* fileName)
{
    long int size;
    FILE* file = fopen(fileName, "rb");
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fclose(file);
    return size;
}

bool file_put_content(const char* fileName, void* buffer, int length)
{
    FILE* file = fopen(fileName, "wb");
    if (!file)
    {
        printf("Failed to open \"%s\" for writing, aborting.\n");
        return false;
    }
    int writeLength = fwrite(buffer, 1, length, file);
    fclose(file);
    return (length == writeLength);
}

void messagebox_default(const char* title, const char* text)
{
    char command[256];
    snprintf(command, 256, "xmessage -center \"%s\n\n%s\"", title, text);
    system(command);
}

void messagebox_information(const char* title, const char* text)
{
    if (!title)
        title = "Information";
    messagebox_default(title, text);
}

void messagebox_critical(const char* title, const char* text)
{
    if (!title)
        title = "Critical Error";
    messagebox_default(title, text);
}

bool flatpak_install_bundle(const char* fileName,
                            const char* appName,
                            bool systemScope)
{
    if (!fileName || !appName)
        return false;

    const char* scopeOption;
    if (systemScope)
        scopeOption = FPBUNDLE_INSTALLER_FLATPAK_OPTION_SYSTEM;
    else
        scopeOption = FPBUNDLE_INSTALLER_FLATPAK_OPTION_USER;

    int status;
    char messageText[256];
    pid_t pid = fork();
    if (pid == 0)
    {
        // This is the child process.  Execute the shell command.
        printf("Starting flatpak to install bundle file...\n");
        execl(FPBUNDLE_INSTALLER_FLATPAK_EXECPATH,
              FPBUNDLE_INSTALLER_FLATPAK_EXECPATH,
              FPBUNDLE_INSTALLER_FLATPAK_COMMAND,
              FPBUNDLE_INSTALLER_FLATPAK_OPTION_BUNDLE,
              scopeOption,
              fileName,
              NULL);
        _exit(EXIT_SUCCESS);
    }
    else if (pid < 0)
    {
        // The fork failed.  Report failure.
        messagebox_critical(NULL,
                    "Cannot start flatpak to install bundle file, aborting.");
        return false;
    }
    else
    {
        // This is the parent process.  Wait for the child to complete.
        if (waitpid(pid, &status, 0) != pid)
        {
            sprintf(messageText,
                    "Flatpak sub-process failed, exit status: %d.",
                    status);
            messagebox_critical(NULL, messageText);
            return false;
        }
        else
        {
            sprintf(messageText,
                    "The package %s has been installed. "
                    "Use command \n\tflatpak run %s \n"
                    "to start the application.",
                    appName, appName);
            messagebox_information("Installation succeed.", messageText);
            return true;
        }
    }
}

int main(int argc, char *argv[])
{
    // Try to locate myself
    const char* execFileName = argv[0];
    int execFile = open(execFileName, O_RDONLY);
    if (!execFile)
    {
        messagebox_critical(NULL,
                            "Failed to read content of myself, aborting.");
        return 1;
    }

    // Extract payload from my self
    // Use memory mapping to avoid extra memory allocation
    long execFileLength = filesize(execFileName);
    void* execImage = mmap(0, execFileLength,
                           PROT_READ, MAP_SHARED,
                           execFile, 0);
    if (execImage == MAP_FAILED)
    {
        messagebox_critical(NULL, "Failed to map myself to memory, aborting.");
        close(execFile);
        return 1;
    }
    void* payload = execImage + FPBUNDLE_INSTALLER_HEADER_SIZE;

    // Write payload to a standalone bundle file
    char* bundleFileName = malloc(strlen(FPBUNDLE_INSTALLER_BUNDLE_PREFIX) +
                                  strlen(FPBUNDLE_INSTALLER_PAYLOAD_NAME) + 1);
    strcpy(bundleFileName, FPBUNDLE_INSTALLER_BUNDLE_PREFIX);
    strcat(bundleFileName, FPBUNDLE_INSTALLER_PAYLOAD_NAME);
    file_put_content(bundleFileName, payload, FPBUNDLE_INSTALLER_PAYLOAD_SIZE);
    munmap(execImage, execFileLength);
    close(execFile);

    // Install the bundle file with flatpak command
    if (!flatpak_install_bundle(bundleFileName,
                                FPBUNDLE_INSTALLER_PAYLOAD_NAME,
                                true))
    {
        messagebox_critical(NULL, "Failed to install bundle file.");
    }
    remove(bundleFileName);

    return 0;
}
