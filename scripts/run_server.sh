CURR=$PWD
cd ../
sudo ./myserver 2>&1 | sed 's/\x1b\[[0-9;]*m//g' |& tee server_log
cd $CURR
