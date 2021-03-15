rm -rf build
mkdir -p build
cd build && cmake ../ && make
echo "Build done"
cd ..