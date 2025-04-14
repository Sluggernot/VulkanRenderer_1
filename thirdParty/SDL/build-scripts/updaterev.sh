#!/bin/sh
#
# Generate a header file with the current source revision

outdir=`pwd`
cd `dirname $0`
srcdir=..
header=$outdir/include/SDL_revision.h
dist=
vendor=

while [ "$#" -gt 0 ]; do
    case "$1" in
        (--dist)
            dist=yes
            shift
            ;;
        (--vendor)
            vendor="$2"
            shift 2
            ;;
        (*)
            echo "$0: Unknown option: $1" >&2
            exit 2
            ;;
    esac
done

rev=`sh showrev.sh 2>/dev/null`
if [ "$rev" != "" ]; then
    if [ -n "$dist" ]; then
        echo "$rev" > "$outdir/VERSION.txt"
    fi
    echo "/* Generated by updaterev.sh, do not edit */" >"$header.new"
    if [ -n "$vendor" ]; then
        echo "#define SDL_VENDOR_INFO \"$vendor\"" >>"$header.new"
    fi
    echo "#ifdef SDL_VENDOR_INFO" >>"$header.new"
    echo "#define SDL_REVISION \"SDL-$rev (\" SDL_VENDOR_INFO \")\"" >>"$header.new"
    echo "#else" >>"$header.new"
    echo "#define SDL_REVISION \"SDL-$rev\"" >>"$header.new"
    echo "#endif" >>"$header.new"
    echo "#define SDL_REVISION_NUMBER 0" >>"$header.new"
    if diff $header $header.new >/dev/null 2>&1; then
        rm "$header.new"
    else
        mv "$header.new" "$header"
    fi
fi
