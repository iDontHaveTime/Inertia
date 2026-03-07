#!/usr/bin/env sh

EXCLUDE_DIRS="build"

find . -type f \( -name "*.cpp" -o -name "*.h" \) \
  ! -regex ".*\($EXCLUDE_DIRS\).*" \
  -exec clang-format -i {} \;