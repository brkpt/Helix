#!/bin/sh
#set -x
echo "Generating ctags"
echo "DeferredShader..."
"${CTAGS_EXE}" -L ${CTRLP_FILELIST} -o ${CTAGS_FILE}

echo "DirectX (${DXSDK_DIR/\\/\/}include)"
find "${DXSDK_DIR//\\/\/}include" -type f | ${CTAGS_EXE} -L - -o ${CTAGS_FILE} --append=yes

