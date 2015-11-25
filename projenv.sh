#!/bin/sh
#set -x
export PROJ_ROOT=$PWD
export CTAGS_FILE=tmp/ctags.out
export JAMPLUS_FILELIST=build/_workspaces_/vs2013/_targetinfo_/targetinfo._all_._all_.lua
export CTRLP_FILELIST=tmp/filelist
alias jd='scripts/jam.sh -c debug'
alias jr='scripts/jam.sh -c release'
if [ ! -d tmp ]; then
	mkdir tmp
	scripts/generatefilelist.sh
	scripts/generatetags.sh
fi


