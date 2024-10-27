# 开发环境搭建
## glog
```
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=[dir] ..
make -j8
make install
```

## boost.asio
```
mkdir $target_dir/boost_1_85_0
cd $install_dir/boost_1_85_0
./bootstrap.sh
./b2 install --prefix=[dir]
```

## gRPC
```
export GRPC_INSTALL_DIR=[dir]
mkdir [dir]
export PATH="$GRPC_INSTALL_DIR/bin:$PATH"
git clone --recurse-submodules -b v1.66.0 --depth 1 --shallow-submodules https://github.com/grpc/grpc
cd grpc
mkdir -p cmake/build
pushd cmake/build
cmake -DgRPC_INSTALL=ON \
    -DgRPC_BUILD_TESTS=OFF \
    -DCMAKE_INSTALL_PREFIX=$MY_INSTALL_DIR \
    ../..
make -j 8
make install
popd
```
