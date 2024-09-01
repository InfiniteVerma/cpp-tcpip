CURR=$PWD
cd ../
clang-format -i include/*.h src/*.cpp tests/*.cpp
cd $CURR
