#!/bin/bash

HEADER_SOURCES="header.c"
HEADER_EXEC="header"
HEADER_SIZE=

BUNDLE_FILE="testBundle"
BUNDLE_NAME="com.example.helloworld"
BUNDLE_SIZE="2188"

EXEC_FILE="$BUNDLE_FILE"_exe


# 1st compilation: generate header without header size info
gcc -o $HEADER_EXEC $HEADER_SOURCES -DFLATPAK_BUNDLE_SIZE=$BUNDLE_SIZE -DFLATPAK_BUNDLE_NAME=\"$BUNDLE_NAME\"
strip $HEADER_EXEC
HEADER_SIZE=$(stat -c %s $HEADER_EXEC)

# 2nd compilation: generate header with size info
gcc -o $HEADER_EXEC $HEADER_SOURCES -DFLATPAK_BUNDLE_SIZE=$BUNDLE_SIZE -DFLATPAK_BUNDLE_NAME=\"$BUNDLE_NAME\" -DINSTALLER_HEADER_SIZE=$HEADER_SIZE
strip $HEADER_EXEC

# Concatenate header and flatpak bundle to generate an self-installing executable
cat $HEADER_EXEC $BUNDLE_FILE > $EXEC_FILE
chmod +x $EXEC_FILE
rm $HEADER_EXEC

# Test if the executable works
./$EXEC_FILE
