#!/usr/bin/env bash
# bake_gba_data.sh — bundle GAMEDATA into a ROM-linkable C source file.
#
# Usage: bake_gba_data.sh <GAMEDATA/KEEN_N dir> <output dir>
#
# For every file in the input directory this script produces:
#   - an extern symbol of its contents (via #embed if GCC >= 15 is
#     detected, else via objcopy-style hex arrays in the generated C).
#
# Output:
#   <output>/gba_data_gen.c  — definitions and the ck_gba_rom_entries[] table.
#
# The bake is a pure function of the input directory, so it reruns cheaply
# when the Makefile decides data-gen is stale.
set -euo pipefail

in_dir="${1:?gamedata dir}"
out_dir="${2:?output dir}"
repo_root="$(cd "$(dirname "$0")/.." && pwd)"

if [ ! -d "${in_dir}" ]; then
    echo "bake_gba_data.sh: input directory does not exist: ${in_dir}" >&2
    exit 1
fi

mkdir -p "${out_dir}"
out_c="${out_dir}/gba_data_gen.c"
staged_dir="${out_dir}/_staged_gamedata"

# Build the host LZEXE tool once and stage a decompressed GAMEDATA tree.
# The GBA has ~98 KiB of heap — far less than Keen 1's 51 KiB LZEXE'd
# exe needs to decompress at runtime (~200 KiB peak). Pre-decompressing
# KEEN*.EXE here lets us map it straight out of ROM with zero heap cost.
host_cc="${CC_HOST:-cc}"
host_tool="${out_dir}/gba_unlzexe_host"
if [ ! -x "${host_tool}" ] || [ "${repo_root}/scripts/gba_unlzexe_host.c" -nt "${host_tool}" ] \
   || [ "${repo_root}/src/third_party/cgenius/fileio/compression/Cunlzexe.c" -nt "${host_tool}" ]; then
    echo "bake_gba_data.sh: building host tool ${host_tool}"
    "${host_cc}" -O2 -std=gnu99 -Wno-parentheses \
        -I"${repo_root}/src" \
        "${repo_root}/scripts/gba_unlzexe_host.c" \
        "${repo_root}/src/third_party/cgenius/fileio/compression/Cunlzexe.c" \
        -o "${host_tool}"
fi

rm -rf "${staged_dir}"
mkdir -p "${staged_dir}"
for src_path in "${in_dir}"/*; do
    [ -f "${src_path}" ] || continue
    base=$(basename "${src_path}")
    upper=$(printf '%s' "${base}" | tr '[:lower:]' '[:upper:]')
    case "${upper}" in
        *.EXE)
            "${host_tool}" "${src_path}" "${staged_dir}/${base}"
            ;;
        *)
            cp "${src_path}" "${staged_dir}/${base}"
            ;;
    esac
done

# Build the host EGALATCH/EGASPRIT LZ decompressor. Keen 1's EGALATCH
# decompresses to ~120 KiB — bigger than the GBA heap. Pre-decompress on
# the host so the runtime can fread() (or better, map straight out of
# ROM) without allocating.
graphics_tool="${out_dir}/gba_decomp_graphics_host"
if [ ! -x "${graphics_tool}" ] \
   || [ "${repo_root}/scripts/gba_decomp_graphics_host.c" -nt "${graphics_tool}" ] \
   || [ "${repo_root}/src/third_party/cgenius/fileio/lz.c" -nt "${graphics_tool}" ]; then
    echo "bake_gba_data.sh: building host tool ${graphics_tool}"
    "${host_cc}" -O2 -std=gnu99 -Wno-parentheses \
        -I"${repo_root}/src" \
        "${repo_root}/scripts/gba_decomp_graphics_host.c" \
        "${repo_root}/src/third_party/cgenius/fileio/lz.c" \
        -o "${graphics_tool}"
fi

# Decompress graphics for each EGAHEAD.* extension we see in the stage.
for head_path in "${staged_dir}"/EGAHEAD.*; do
    [ -f "${head_path}" ] || continue
    ext="${head_path##*.}"
    "${graphics_tool}" "${staged_dir}" "${ext}"
done

# Build the host LEVEL/text preprocessor. CRLE and the text-file transform
# both produce output larger than (or equal to) their input, so doing them
# on the host means the GBA runtime can memcpy from ROM (levels) or point
# a pointer straight at ROM (text files) instead of malloc'ing scratch.
misc_tool="${out_dir}/gba_preprocess_misc_host"
if [ ! -x "${misc_tool}" ] \
   || [ "${repo_root}/scripts/gba_preprocess_misc_host.c" -nt "${misc_tool}" ]; then
    echo "bake_gba_data.sh: building host tool ${misc_tool}"
    "${host_cc}" -O2 -std=gnu99 -Wno-parentheses \
        "${repo_root}/scripts/gba_preprocess_misc_host.c" \
        -o "${misc_tool}"
fi

# Run the LEVEL/text preprocessor against each episode extension we see.
for head_path in "${staged_dir}"/EGAHEAD.*; do
    [ -f "${head_path}" ] || continue
    ext="${head_path##*.}"
    "${misc_tool}" "${staged_dir}" "${ext}"
done

# From here on, read from the staged (decompressed) tree.
in_dir="${staged_dir}"

# Collect files — sort so successive runs are reproducible.
# Use while-read instead of mapfile to stay compatible with macOS's bash 3.2.
files=()
while IFS= read -r line; do
    files+=("${line}")
done < <(cd "${in_dir}" && ls -1 | sort)

if [ ${#files[@]} -eq 0 ]; then
    echo "bake_gba_data.sh: no files in ${in_dir}" >&2
    exit 1
fi

{
    cat <<'HDR'
/*
 * gba_data_gen.c — auto-generated from data/GAMEDATA/KEEN1. Do not edit.
 *
 * Every GAMEDATA file is embedded as a byte array in a .rodata section
 * that the linker lays out in cart ROM. The table at the bottom maps
 * 8.3 DOS names to {pointer, size} so the FILE* shim can resolve opens
 * without a filesystem.
 */

#include <stddef.h>
#include <stdint.h>
#include "platform/gba_data.h"

HDR

    # Emit a blob per file.
    idx=0
    for f in "${files[@]}"; do
        sym="ck_gba_blob_${idx}"
        path="${in_dir}/${f}"
        size=$(wc -c < "${path}" | tr -d ' ')
        echo "/* ${f} (${size} bytes) */"
        echo "static const uint8_t ${sym}[] __attribute__((aligned(4))) = {"
        # xxd is handy: generates a C-ish hex dump.
        xxd -i < "${path}" | sed 's/  */    /g'
        echo "};"
        echo
        idx=$((idx + 1))
    done

    # Emit the lookup table.
    echo "const ck_gba_rom_entry ck_gba_rom_entries[] = {"
    idx=0
    for f in "${files[@]}"; do
        upper=$(printf '%s' "${f}" | tr '[:lower:]' '[:upper:]')
        size=$(wc -c < "${in_dir}/${f}" | tr -d ' ')
        printf '    { "%s", ck_gba_blob_%d, %su },\n' "${upper}" "${idx}" "${size}"
        idx=$((idx + 1))
    done
    echo "};"
    echo "const unsigned ck_gba_rom_entry_count = ${#files[@]};"
} > "${out_c}"

echo "bake_gba_data.sh: wrote ${out_c} (${#files[@]} entries)"
