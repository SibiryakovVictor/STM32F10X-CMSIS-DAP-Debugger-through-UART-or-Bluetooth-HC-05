#!/bin/bash

COMMIT_INFO=$(git log -1 --format="%H %d %an, %aD")
DIR="$( cd "$(dirname "$0")" ; pwd -P )"

# только в строках, начинающихся на #define подставить содержиемое COMMIT_INFO. | для разделителей, потому что внутри COMMIT_INFO есть слеши
sed -i -e "/#define/   s|COMMIT_DATA.*|COMMIT_DATA \"$COMMIT_INFO\"|" "$DIR/retarget.cpp"
# восстановить виндовые концы строк
unix2dos "$DIR/retarget.cpp"



