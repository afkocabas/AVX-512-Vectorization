#!/usr/bin/env bash
set -euo pipefail

CXX="g++"
BASE_FLAGS="-Wall -Wextra -O3 -malign-double -mcmodel=medium"
SCALAR_FLAGS="-fno-tree-vectorize -DUSE_AVX512=0"
VECTOR_FLAGS="-mavx512f -DUSE_AVX512=1" # or -march=native
BINARY_DIR="binaries"

usage() {
  echo "[ERROR]: No input file is provided" >&2
  echo "Usage: ./run.sh [file.cc] [scalar|vector|both]" >&2
  exit 1
}

compile() {
  local src="$1"
  local out="$2"
  shift 2
  mkdir -p "$BINARY_DIR"
  echo "Compiling $src -> $BINARY_DIR/$out"
  set -x
  $CXX $BASE_FLAGS "$@" "$src" -o "$BINARY_DIR/$out"
  set +x
}

run_bin() {
  echo "Running $1"
  echo "--------------------"
  "$1"
  echo
}

main() {
  [[ $# -ge 1 ]] || usage
  local src="$1"
  local mode="${2:-both}"
  local base="$(basename "$src" .cc)"

  if [[ "$mode" == "scalar" || "$mode" == "both" ]]; then
    compile "$src" "${base}_scalar" $SCALAR_FLAGS
    run_bin "./$BINARY_DIR/${base}_scalar"
  fi

  if [[ "$mode" == "vector" || "$mode" == "both" ]]; then
    compile "$src" "${base}_avx512" $VECTOR_FLAGS
    run_bin "./$BINARY_DIR/${base}_avx512"
  fi
}

main "$@"
