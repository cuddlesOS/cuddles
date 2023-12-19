#!/bin/bash
set -e
make -qC util/dump2cuddlekeys
doas loadkeys -C/dev/tty1 "$1"
dumpkeys -C/dev/tty1 | util/dump2cuddlekeys/dump2cuddlekeys > "fs/keymap/$1"
