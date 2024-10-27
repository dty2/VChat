#! /bin/bash

rm -rf ./generate/*

../libs/protobuf/bin/protoc \
  --proto_path=./ \
  --cpp_out=./generate \
  vchat.proto
