# Self-Learning Materials of CS106L

This repository collects and organizes learning materials of CS106L, a C++ programming course taught at Stanford University. The course delves into many useful and essential features of modern C++ and will help you understand C++ more in depth. In specific, this repo contains lecture slides, textbook, starter codes and testbenches of programming assignments.

```shell
.
├── assignments # documents and starter codes of programming assigments
│   ├── GapBuffer
│   ├── HashMap
│   └── KDTree
├── README.md
├── slides # lecture slides of Sp23 and Win21 semesters
│   ├── Spring2023
│   └── Winter2021
├── solutions # solution codes of three programming assignments
│   ├── GapBuffer
│   ├── HashMap
│   └── KDTree
└── textbooks # textbook for this course
    └── full_course_reader.pdf
```

# Course Overview
This course discusses the following topics of C++ programming:
- Type and Struct
- Uniform Initialization
- Reference
- Streams
- Containers
- Iterators
- Function Templates
- Lambda Function
- Template Class
- Const-Correctness
- Operator Overloading
- Special Member Function
- Move Semantics
- Resource Acquisition Is Initialization

For each topic, this repo includes two versions of lecture slides: one from Spring 2023 semester and the other comes from Winter 2022 semester. These two versions cover similar aspects of each topic but may illustrate them in different ways. And you can refer to both of them when learning a specific topic.

# Programing Assignments
Programming assignments are the most important and valuable part of this course, which guide you to create STL-compliant container classes from scratch. The implementation of these classes refers to most C++ grammars and features introduced in the lecture. By completing these assignments, you will better understand and master what taught in this class and improve your C++ programming skills.

This repo has collected programming assignments released from three different semesters of this course, including HashMap, KDTree and GapBuffer. Each assignment is accompanied by comprehensive materials, including documents, starter codes, benchmarks, build scripts, and solutions.

- [GapBuffer](./assignments/GapBuffer/): sequential container optimized for fast inserttion and deletion at any position;
- [KDTree](./assignments/KDTree/): k-dimension binary search tree for efficient storage and query of multi-dimension data;
- [HashMap](./assignments/HashMap): associative container similiar to the [unordered_map](https://en.cppreference.com/w/cpp/container/unordered_map) provided in STL;

The original versions of these programming assignments were built based on [QtCreator](https://www.qt.io/product/development-tools), which requires tedious and cumbersome process to setup development environment for self-learners. This repo has refactored the build and test process of each assignment using [CMake](https://cmake.org/) and [GoogleTest](https://github.com/google/googletest) for the convenience of development on Linux machines. And you only need to ensure that your Linux machine has installed CMake and C++ compiler before getting started with these assignments. To setup development environemnt on Ubuntu systems, you may refer to following commands:

```shell
sudo apt-get update
sudo apt-get install build-essential libssl-dev cmake -y
```

# Related Resources
All contents in this repo are sourced from online resources inclduing:
- CS106L Winter 2018: https://web.stanford.edu/class/archive/cs/cs106l/cs106l.1184/assignments.html
- CS106L Winter 2020: https://web.stanford.edu/class/archive/cs/cs106l/cs106l.1204/index.html
- CS106L Spring 2022: https://web.stanford.edu/class/archive/cs/cs106l/cs106l.1226/
- CS106L Spring 2023: https://web.stanford.edu/class/cs106l/
- Other GitHub Repos: https://github.com/PKUFlyingPig/CS106L
