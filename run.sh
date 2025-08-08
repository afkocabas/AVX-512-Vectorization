CFLAGS=" -Wall -Wextra -malign-double -O3 -mavx512f"
BINARY_DIR="binaries"
CXX="g++"

function usage() {
  echo "[ERROR]: No input file is provided" >&2
  echo "Usage: ./run.sh [file.cc]"
  exit 1
}

function compile_and_run {
  echo ${1}
  set -x
  [[ -d ${BINARY_DIR} ]] || mkdir ${BINARY_DIR}
  $CXX $CFLAGS $1 -o ./${BINARY_DIR}/$(basename $1 .cc)
  ./${BINARY_DIR}/$(basename $1 .cc)
}

function main {
  if [[ $# -lt 1 ]]; then
    usage
    exit 1
  fi
  compile_and_run ${1}
}

main "$@"
