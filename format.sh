#!/usr/bin/env bash

find . -type f -name '*.h' -exec clang-format -i '{}' \;
find . -type f -name '*.cpp' -exec clang-format -i '{}' \;
