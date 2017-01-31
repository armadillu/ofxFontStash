#!/usr/bin/env bash
set -e

echo "Executing ci/install.sh"


export OF_ROOT=~/openFrameworks
export OF_ADDONS=$OF_ROOT/addons

ADDONS="armadillu/ofxTimeMeasurements"

cd $OF_ADDONS

for ADDON in $ADDONS
do
  echo "Cloning addon '$ADDON' to " `pwd`
  git clone --depth=1 --branch=$OF_BRANCH https://github.com/$ADDON.git
done

cd -