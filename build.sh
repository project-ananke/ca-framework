#!/bin/bash

compile() {
	odin build source -collection:ext=./engine/ext -collection:engine=./engine -debug -out:build/ananke
}

run() {
	pushd build > /dev/null
	./ananke
	popd > /dev/null
}

mkdir -p build

if [[ "$1" == "run" ]]; then
	echo "-- Compiling Ananke"
	if compile; then 
		echo "-- Running Ananke"
		run
	fi
else
	echo "-- Compiling Ananke"
	compile
fi