#!/usr/bin/env bash

#------------------------------------------------------------------------------
#
#	download.sh
#
#		otokodama ビルドに必要なファイルをまとめてダウンロード
#
#------------------------------------------------------------------------------

ROOT_DIR="${PWD}"
DOWNLOAD_DIR="${ROOT_DIR}/download"

# ディレクトリ作成
mkdir -p ${DOWNLOAD_DIR}
cd ${DOWNLOAD_DIR}

#-----------------------------------------------------------------------------
# elf2x68k のダウンロード
#-----------------------------------------------------------------------------

ELF2X68K_ARCHIVE=elf2x68k-Linux-20230318.tar.bz2

wget -nc https://github.com/yunkya2/elf2x68k/releases/download/20230318/${ELF2X68K_ARCHIVE}
tar jxvf ${ELF2X68K_ARCHIVE}


#-----------------------------------------------------------------------------
# xsp のダウンロード
#-----------------------------------------------------------------------------

XSP_COMMIT="965ea7f486896c592b6100b4e79aeb201091c888"
XSP_ARCHIVE=${XSP_COMMIT}.zip

wget -nc https://github.com/yosshin4004/x68k_xsp/archive/${XSP_ARCHIVE}
7z x -y ${XSP_ARCHIVE}


#-----------------------------------------------------------------------------
# APICG のダウンロード
#-----------------------------------------------------------------------------

APICG_ARCHIVE1=apg_212a.lzh
APICG_ARCHIVE2=APGL212A.Lzh
APICG_TEMP_DIR=apicg_tmp
APICG_DIR=apicg

wget -nc http://retropc.net/x68000/software/graphics/pic/apicg/${APICG_ARCHIVE1}
7z x -y -o"./${APICG_TEMP_DIR}" ${APICG_ARCHIVE1}
7z x -y -o"${APICG_DIR}" ./${APICG_TEMP_DIR}/${APICG_ARCHIVE2}


#------------------------------------------------------------------------------
# 正常終了
#------------------------------------------------------------------------------

echo ""
echo "-----------------------------------------------------------------------------"
echo "File download completed."
echo "-----------------------------------------------------------------------------"
echo ""
exit 0
