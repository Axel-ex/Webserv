## Compilation

first do in the root of the project:

```cmake -B ./build```

it will create a lock file with info needed by Cmake to build the project. Then:

``` cmake --build ./build```

it will create the executables. our executable can be executed as so:

```./build/webserv```

When you write your code if you need to add file for the compilation you can add the name of your file in the CMakeLists.txt at the root of the directory.
If you want to write tests you can add them in test.cpp (there is a dummy exemple). to run test you need to build first and then execute:

``` ./build/test/webserv_test```

test folder as its own CMakeLists.txt and if you want to create more testfile you can do like explained above: add  the name of the file you created to the CMakeLists.txt of the test folder.

