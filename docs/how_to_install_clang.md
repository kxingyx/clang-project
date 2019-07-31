***本系列文章的运行环境基于CentOS 6.3 x86_64，gcc 5.2.0，cmake version 3.15.0， glibc 2.23.2，后文不再赘述。***



## LLVM&clang简介
[LLVM](http://llvm.org/)是一组编译工具链集合，值得注意的是，LLVM并不是谁的缩写，其中的VM与virtual machine也没有关系。如官网所述，LLVM就是这个项目的名称，没有其他含义。所以，凡是看到中文资料里面声称 LLVM 全称是 Low Level Virtual Machine的， 应当立刻停止阅读，以防被毒害！！
clang（读音/klaNG/，音同*可浪*）是[LLVM](http://llvm.org/)的一个子项目(sub-project)。 它是一个[C系语言](https://en.wikipedia.org/wiki/List_of_C-family_programming_languages)的[编译前端](https://en.wikipedia.org/wiki/Compiler#Front_end)，同时clang还提供了以下三种面向开发者的扩展方式：
- C Library libclang ，libclang是一套稳定的(stable)但对AST控制不够全面的C Libray
- C++ Library libtooling，libtooling是一套对AST控制全面但却不够稳定的C++ Library，不稳定指的是无前后兼容保证，接口经常变化（包括函数名称、函数参数列表）
- plugin， C++接口，主要针对单次编译的单个文件，比如可用于实现一个自定义风格的代码检查工具

不同的业务目标，可以在以上三种做出选择，本系列最终目的是完成中大型项目的代码索引，所以选择基于libtooling进行开发。

## 安装
### 源码下载
由于libtooling的不稳定因素，我们决定采用最新的[LLVM8.0-release](http://releases.llvm.org/download.html#8.0.0)版本。
分别下载[LLVM源码](http://releases.llvm.org/8.0.0/llvm-8.0.0.src.tar.xz)、[clang源码](http://releases.llvm.org/8.0.0/cfe-8.0.0.src.tar.xz)、[clang-tools-extra源码](http://releases.llvm.org/8.0.0/clang-tools-extra-8.0.0.src.tar.xz)并解压。由于LLVM各个工程共享了很多CMake配置，因此需要调整他们的位置，
假设LLVM、clang、clang-tools-extra源码解压后被放置在 LLVM_SRC_ROOT, CLANG_SRC_ROOT,CLANG_TOOLS_EXTRA_ROOT，那么需要进行如下的目录调整
```
#set these 3 env
export LLVM_SRC_ROOT=/path/to/your/llvm
export CLANG_SRC_ROOT=/path/to/your/cfe #clang-src
export CLANG_TOOLS_EXTRA_ROOT=/path/to/your/clang-tools-extra

#will use 3 env defined above
mv ${CLANG_SRC_ROOT} ${LLVM_SRC_ROOT}/tools/clang
mv ${CLANG_TOOLS_EXTRA_ROOT} ${LLVM_SRC_ROOT}/tools/clang/tools/extra
```
使用`ln -s TARGET LINK`的方式建立两个软链也可达到类似的收益。

> 除了上面的方式，[官方](https://clang.llvm.org/get_started.html)提供了cmake 宏 LLVM_ENABLE_PROJECTS，可以不将clang等源码目录移动到llvm下:
> 1. 将clang解压至 `${LLVM_SRC_ROOT}/..`，即llvm-src的平级目录, 命名为clang
> 2. 将extra-tool也解压到llvm平级目录，命名为clang-tools-extra
> 3. 在${LLVM_SRC_ROOT}/build下，`cmake -DCMAKE_INSTALL_PREFIX=/your/path/to/llvm/  -DLLVM_ENABLE_PROJECTS=clang -G "Unix Makefiles" ..`, 这时， LLVM_TOOL_CLANG_TOOLS_EXTRA_BUILD 会被设置成ON，继续编译: ` make clang && make clangd`, 如需安装，则另外调用`make install`。 


### 编译安装
### 先决条件
>最新的LLVM需要CMake. Version 3.4.3，下文假设合适版本的cmake已经正确安装，此外编译LLVM，最好磁盘剩余80G以上空间。


编译前空间:
> $ df -h  
Filesystem      Size  Used Avail Use% Mounted on  
/dev/vda1        20G  5.0G   14G  28% /  
tmpfs           7.8G   11M  7.8G   1% /dev/shm  
/dev/vdb        233G  8.6G  212G   4% /home


编译后剩余空间:
> $ df -h  
Filesystem      Size  Used Avail Use% Mounted on  
/dev/vda1        20G  5.0G   14G  28% /  
tmpfs           7.8G   11M  7.8G   1% /dev/shm  
/dev/vdb        233G   82G  139G  38% /home  


粗略的空间估算,  由于进行的是LLVM全系列编译，硬盘消耗约80G, 如没有如此多的空间，可根据需要只编译某一部分，比如 在cmake build时，仅执行`make clang`。

假设CMake和CXX(比如g++)是工作的，直接按照官方文档编译即可, 编译比较耗时，为了加速，可以设置-j并发参数,参考如下build命令的注释内容，即使加速，也需要小时级别耗时（视机器配置和负荷有所不同）。
```bash
cd ${LLVM_SRC_ROOT}
mkdir -p build
cd build
cmake ${LLVM_SRC_ROOT}
CPU_NUM=`getconf _NPROCESSORS_ONLN`
cmake --build . # to speed up ,  -j <num> is recommended, say : cmake --build . -j  ${CPU_NUM} || cmake --build .
#if compile successfully, install
cmake -DCMAKE_INSTALL_PREFIX=/path/where/llvm/installed  -P cmake_install.cmake
```
如以上均正确返回，再double check一下，环境变量等是否正确
>$ clang++ --version
clang version 8.0.0 (tags/RELEASE_800/final)
Target: x86_64-unknown-linux-gnu
Thread model: posix
InstalledDir: /home/M23/local/llvm/bin

检查一下LLVM的某项配置：
>$ llvm-config --cxxflags
-I/home/M23/local/llvm/include -std=c++11  -fno-exceptions -fno-rtti -D_GNU_SOURCE -D_DEBUG -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS

在上面的步骤中，我们还下载了clang-tools-extra， 如果需要编译clangd等extra-tool， 则需要在cmake调用中增加 `-DLLVM_TOOL_CLANG_TOOLS_EXTRA_BUILD=ON`, 然后 make clangd即可。


至此，我们成功安装了clang开发所需的基础环境。


