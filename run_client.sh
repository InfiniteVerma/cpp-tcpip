sudo ./myclient 2>&1 | sed 's/\x1b\[[0-9;]*m//g' > client_log

