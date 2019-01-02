#!/bin/bash
#   Version 
#   Author: WildfootW
#   GitHub: github.com/WildfootW
#   Copyright (C) 2018 WildfootW All rights reserved.
#

# Absolute path to this script, e.g. /home/user/Pwngdb/install.sh
SCRIPT=$(readlink -f "$0")
# Absolute path this script is in, thus /home/user/Pwngdb
SCRIPTPATH=$(dirname "$SCRIPT")

my_adttest="../taskMgr"
ref_adttest="../ref/taskMgr-ref"
#dofiles="do1 do2 do3 do4 do5"
dofiles="mydo1"

echo "" > diff_result
for dofile in `echo $dofiles | tr ' ' '\n'`; do
    eval $my_adttest -f $dofile &> "autotest_out.$dofile"
    eval $ref_adttest -f $dofile &> "autotest_out.ref.$dofile"
    echo "=================================================" >> diff_result
    echo "                    $dofile" >> diff_result
    echo "=================================================" >> diff_result
    diff "autotest_out.$dofile" "autotest_out.ref.$dofile" >> diff_result
done

