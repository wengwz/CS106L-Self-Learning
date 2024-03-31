# KDTree
In this programming assignment, you will implement a k-dimension tree which is designed for efficient storage and query of multi-dimension data. For detailed description of KDTree and guidance on this assignment, you can refer to this [document](./doc/005_assignment_3_kdtree.pdf). To build and test your codes, you can refer to following commands:
```shell
mkdir -p build && cd build
cmake ..
make
./kd_tree_test
```
You can selectively enable testcases corresponding to the functionalities you have already implemented by changing macro definitions in the header of [kd_tree_test.cc](./kd_tree_test.cc).
