#!/bin/bash

## Check this guide: 
## https://cern.service-now.com/service-portal?id=kb_article&n=KB0009138

myUID=$(id -u)
echo "ID=${myUID}"
VNC_DISPLAY=$(( 1 + ( ${myUID} % 65535 ) ))
echo "VNC_DISPLAY=${VNC_DISPLAY}"
VNC_PORT=$(( ${VNC_DISPLAY} + 5900 ))
echo "VNC_PORT=${VNC_PORT}"

echo "Starting the VNC server..."

systemctl --user start  vncserver@${VNC_DISPLAY}.service
#export DISPLAY=localhost:${VNC_DISPLAY} 

echo "You should be loggin in with the following command (if you haven't already!)"
echo "ssh -L ${VNC_PORT}:localhost:${VNC_PORT} ${USER}@lxplus.cern.ch"
echo "You can then connect a VNC client to localhost:${VNC_PORT}"

echo "Other useful commands:"
echo "systemctl --user status"
echo "systemctl --user status vncserver@${VNC_DISPLAY}.service"
echo "journalctl --user --unit vncserver@${VNC_DISPLAY}.service"
echo "systemctl --user stop vncserver@${VNC_DISPLAY}.service"
