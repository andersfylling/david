#! /bin/sh

loc=$( ( find ./engine/ -type f \( -name '*.h' -o -name "*.cpp" \) -print0 | xargs -0 cat ) | wc -l )
echo "LOC: $loc"
