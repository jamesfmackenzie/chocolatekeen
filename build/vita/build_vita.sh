#!/bin/sh
set -eu

if [ -z "${VITASDK:-}" ]; then
    echo "VITASDK is not set. Source your VitaSDK environment first."
    exit 1
fi

mkdir -p obj

has_target=0
for arg in "$@"; do
    case "$arg" in
        -*|*=*)
            ;;
        *)
            has_target=1
            break
            ;;
    esac
done

if [ "$has_target" -eq 0 ]; then
    set -- "$@" vpk
fi

make "$@"
