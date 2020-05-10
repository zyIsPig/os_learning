
# report7

## Briefly describe the FIFO page-replacement algorithm and analyze its algorithm complexity

- 当发生缺页的时候且页表还没有装满的时候，直接加入fifo中。当页表满了，将最先加入fifo的entity换成新的entity，先进先出。该算法遍历了一遍输入（n），
同时我们可以使用hashmap在O(1)的时间查找是否命中。所以时间复杂度为O(N)。

## Briefly describe the MIN page-replacement algorithm and analyze its algorithm complexity

- 该算法为命中率更高的算法，但是它需要知道未来的输入信息。每次页表满了且发生缺页时，我们查找之后还未输入的所有entity，找到未来最晚被使用到的entity并将它
和新来的节点置换，需要注意还要考虑现在已经cache但是未来不会出现的entity优先度更高。我们同样通过hashmap来进行快速查找，每次发生缺页的时候我们需要遍历剩余节点和
已经被cache的节点，需要O(N)的时间，再加上一共有N个输入，所以时间复杂度为O(N*N)

## Briefly describe the LRU page-replacement algorithm and analyze its algorithm complexity
- LRU为最近最少使用，我通过双向链表来维护LRU的页表，我想要达到的效果是删除节点，增加节点，查找节点的时间复杂度都为O(1)。若lru list还有剩余空间，
则向该链表的头部插入新来的节点。若发生了缺页，我们移除掉list的最后节点（最近最少使用的节点），并且向头部插入新来的节点。若cache hit，则将命中的节点放到
list的头部。使用hashmap可以达到O(1)的时间查找节点。时间复杂度：因为插入，删除，查找都是O(1)，所以总时间复杂度为O(N);

## Briefly describe the clock algorithm and analyze its algorithm complexity
- 时钟算法利用了环形链表。当该链表被放满后，若发生缺页，则遍历该链表，找到第一个生命周期为0的节点，将该节点换出。生命周期只有0和1，当查找遍历时，若当前节点
的生命周期为1则把它变为0。假设当该链表的生命周期全部为1（刚好插入满）,此时若发生缺页，将会花费O(K)去比遍历该链表,遍历结束之后所有节点的生命周期都会变为
0，那么之后的K-1次缺页都仅仅使用O(1)时间查找，也就是说每一个缺页的平均查找时间为O(1)。所以总共我们仅需要O(N)的时间复杂度。

## Briefly describe the second-chance algorithm and analyze its algorithm complexity
- 该算法结合了lru和fifo，前半部分时fifo，后半部分时lru。在插入节点的时候，优先向fifo中插入元素，当fifo满了之后，fifo会溢出到lru中。当lru cache hit时，将命中的节点从lru中删除并
重新插入到fifo中。时间复杂度：由于fifo插入 删除和lru插入删除以及fifo和lru之间的移动都是O（1），所以总共的时间还是O(N)

