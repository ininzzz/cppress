## 定时器

### 功能

- Add: 增加一个结点并设置该结点在超时的时候调用的回调函数
- Update: 更新结点的超时时间(加一个周期)
- Handle: 删掉超时的结点并调用回调函数

### 数据结构

- 采用的数据结构为优先队列，因为不想手写堆所以直接用`std::priority_queue`，缺点在于这种数据结构无法对其内部的元素做随机访问，哪怕用`std::set`替换，也只能保证O(logn)的访问，如果遇到多次更新，每次O(logn)是很浪费时间的，这里采取另一种性能更高的办法
- 增加一个数组记录更新后的时间，在handle的时候先看看这个结点是否在数组中被更新了，如果被更新且时间大于当前时间，则重新把这个元素扔到堆中，否则就从堆中删除结点并做对应的callback，这样均摊下来优于每次事件触发就更新结点，也避免了使用`std::set`，为了降低内存的消耗，数组的实现采用`std::vector`

### 其他方案和改进

- 时间轮，上面的做法可以看出，需要重新将元素扔到堆中依然需要O(logn)的时间，只是这样的操作次数被减少了，时间轮采用哈希的思想可以把时间优化到O(1)，但是时间轮的设计比较麻烦，后面有时间可以实现一下
- muduo网络库采用timerfd将定时操作放到事件循环中，但是timerfd我用下来好像依然每次循环都会触发读，只不过在read的时候会阻塞？这样好像依然会出现busy-loop，后续再研究一下
- 另外我对`std::chrono`也不是很熟悉，只是看了几个函数和类型，如果有更优秀的方法也会重写一下这个定时器