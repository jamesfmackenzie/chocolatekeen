#!/bin/sh
set -eu

if [ -z "${VITASDK:-}" ]; then
    echo "VITASDK is not set. Source your VitaSDK environment first."
    exit 1
fi

mkdir -p obj
make "$@"
