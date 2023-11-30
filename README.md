# shared_memory

Python的mmap似乎不太稳定，有的时候会出现写入不成功的情况（可能是代码问题？），使用C暂时还没有遇到过这样的情况。

因此，将C编译成共享库，在Python中使用。
