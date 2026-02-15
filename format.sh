#!/bin/sh
clang-format --verbose -i `find lib src include '(' -name "*.c" -or -name "*.h" ')'`
