#!/usr/bin/env bash

SCRIPTPATH="$( cd "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

cp $SCRIPTPATH/../hw/de1_soc.rbf $SCRIPTPATH/../../../output/images/de1_soc.rbf
