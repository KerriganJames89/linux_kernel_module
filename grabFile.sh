#!/usr/bin/env bash

# ################################################
# Grab file from linrpog server to local machine
# ################################################
# -s run with sudo | -r run without sudo
# -----------------------------------------------
# $2 - username
# $3 - file location on linprog server
# $4 - destination on local machine
# -----------------------------------------------
# To let grabFile.sh run as an executable:
# chmod +x grabFile.sh
# ################################################

if [[ $1 == "-s" ]]; then
    # grab file as super user
    sudo scp $2@linprog.cs.fsu.edu:~$3 $4
elif [[ $1 == "-r" ]]; then  
    # without sudo pesmission
    scp $2@linprog.cs.fsu.edu:~$3 $4
fi