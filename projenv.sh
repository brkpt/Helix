#!/bin/sh
#set -x
export PROJ_ROOT=$PWD
export CTAGS_FILE=tmp/ctags.out
export JAMPLUS_FILELIST=build/_workspaces_/vs2013/_targetinfo_/targetinfo._all_._all_.lua
export CTRLP_FILELIST=tmp/filelist
alias pd='cd ${PROJ_ROOT}'
function jd()
{
	pushd "${PROJ_ROOT}" > /dev/null
	scripts/jam.sh -c debug ${@}
	popd > /dev/null
}

function jr()
{
	pushd "${PROJ_ROOT}" > /dev/null
	scripts/jam.sh -c release ${@}
	popd > /dev/null
}

if [ ! -d tmp ]; then
	mkdir tmp
	scripts/generatefilelist.sh
	scripts/generatetags.sh
fi
gv


