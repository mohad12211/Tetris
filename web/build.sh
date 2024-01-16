#!/bin/sh

emcc -o web/index.html src/*.c -Os -Wall -std=c99 -D_DEFAULT_SOURCE -Iweb/ web/libraylib.a  -s USE_GLFW=3 -s EXPORTED_RUNTIME_METHODS=ccall -sGL_ENABLE_GET_PROC_ADDRESS -DPLATFORM_WEB --shell-file web/minshell.html --preload-file resources/Music_1.ogg --preload-file resources/Music_2.ogg --preload-file resources/Music_3.ogg
git checkout gh-pages
cp web/index* .
git commit -am "Update"
git push
