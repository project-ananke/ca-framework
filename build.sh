#!/bin/bash

compile() {
	odin build engine -collection:ext=./ext -collection:engine=./engine -debug -out:build/ananke
}

run() {
	pushd build > /dev/null
	./ananke
	popd > /dev/null
}

mkdir -p build

if [[ "$1" == "run" ]]; then
	compile
	run
else
	compile
fi