# report6

### What is Uniprogramming & What is the shortcoming of Uniprogramming？
- ***what is uniprogramming?*** uniprogramming 只允许一次性运行一个用户程序，并把他放在RAM中，如果他想跑另外一个程序的话需要将该程序的内存覆盖掉。在一些老的计算机上采取的是这种
策略。
- ***What is the shortcoming of Uniprogramming***？uniprogramming 只能一次运行单个用户程序，这显然不能满足绝大部分计算机的需求。

### What is Multiprogramming？& What is the shortcoming of Multiprogramming？
- ***What is Multiprogramming?*** Muliprogramming 允许多个用户程序运行在RAM中，计算机可以同时运行这些程序(并发),cpu通过context switch分时执行这些程序。
- ***What is the shortcoming of Multiprogramming?*** Multiprogramming对RAM的容量要求较高，对cpu的执行效率要求较高，同时需要算法保护RAM，对整体的
硬件要求较高。

### What is the segmentation mechanism and its advantages & disadvantages?
- ***mechanism***：分段是非连续内存分配。他将逻辑上的原本连续的内存分成不同大小的进行储存。实现方式上软硬件结合，操作系统维护一个段表，段表包含了界限和基址，界限规定了该段的长度
基址为该段在物理内存中的开始地址，cpu通过段表进行地址的映射并且验证合法性，若不合法则陷入操作系统。
- ***advantages***：分段的好处是相对于连续地址分配来说。由于分段是非连续的，所以1.碎片问题得到改善。2.程序的内存本来是就是有区别的（可读，可写等），
分段在物理内存上将不同性质的数据分隔开而且每一段在逻辑上独立，更利于实现保护和共享。3.消除了内部碎片。
- ***disadvantages*** : 分段相比连续内存分配开销更大，实现更复杂。段表要占用额外内存。

### What is the paging mechanism and its advantages & disadvantages?
- ***mechanism***：分页也是一种非连续的内存分配策略。他跟分段的不同是分页是固定大小而分段不固定大小。分页在物理上将
内存分为大小一样的帧，在逻辑上将内存分为大小一样的页。也是通过软硬件结合的方式实现，操作系统维护一个页表。通过页表完成页到帧的映射。
- ***advantages***: 这里的好坏的比较对象是分段。1.分页相对于分段来说因为页的大小是固定的，可以通过相应的页分配策略消除掉外碎片和紧缩，
而分段不行。2.同时交换空间的单位是定长的。
- ***disadvantages***: 1.需要的额外内存大于分段 2.算法更加复杂 3.内部碎片


### How to reduce the occurrence of internal fragmentation when allocating memory? Briefly explain why this method works
- 消除内碎片可以通过在内存分配时候采用按需分配，用多少分配多少的策略进行消除。将内存分配的单元的颗粒度降低，这样分配的和实际需要的差值就会降到
最低，内碎片就会减少。


### How to reduce the occurrence of external fragmentation when allocating memory? Briefly explain why this method works.
- 外碎片消除方式需要结合实用场景进行选择相应的办法。因为是在分配时候的方法所以不考虑重定向方法。用最优适应和最差适应来实现。当我们的进程需要的内存都
差不多的时候，可以使用最差适应算法尽量避免细小外碎片。当进程颗粒度差距比较大的时候采取最优分配，可以避免大颗粒的进程无法分配。
