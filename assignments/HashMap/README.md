# HashMap
In this programming task, you will implement an associative container class which has similar functionalities as [unordered_map](https://en.cppreference.com/w/cpp/container/unordered_map) provided in the STL. For detailed description of HashMap and guidance on this assignment, you can refer to this [document](./doc/HashMap_doc.pdf). As described in the document, for the original version of this assignment, you are given an almost complete implementation of HashMap and only need to do some modifications or add few functionalities. But it's highly recommended that you read and understand HashMap implementations provided in the original [starter codes](./doc/HashMap_Starter/) and then implemnt each function from scratch in the empty source files under current directory.

To build and test your implementation, you can refer to the following commands. You can selectively enable testcases corresponding to the functionalities you have already implemented by changing macro definitions in the [test_settings.h](./test_settings.h).
```shell
mkdir -p build && cd build
cmake ..
make
# functionality test
./hashmap_test
# performance test
./hashmap_perf
```
