CURR=$PWD
cd ../
sudo DEBUGINFOD_URLS="https://debuginfod.archlinux.org" G_SLICE=always-malloc valgrind --leak-check=full ./myserver
cd $CURR
