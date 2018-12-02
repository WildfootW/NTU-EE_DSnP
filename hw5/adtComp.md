# adtComp
## 資料結構的實作
### Dynamic Array
用一段連續的記憶體存放資料，並在空間不夠時自動擴展。  
`_data`存放其開頭位址;`_size`紀錄目前有多少(個)資料存放在其中;`_capacity`儲存目前new了多少記憶體來放資料(個);`isSorted`在呼叫`sort()`後被設成`true`，並在`erase()`、`push_back()`等操作後被設為`false`。  
一開始的`_capacity`為0，而每次`push_back()`但空間不夠時自動擴展`_size + 1`的空間(總共為`_size * 2 + 1`)。  
`end()`會回傳最後一個有效資料的後一個位置。  
`erase()`會直接把最後一個資料複製到要刪除的資料的位置，並把`_size` - 1。  
`clear()`僅把`_size`設為0。  
`sort()`使用STL中的`sort()`。  

### Double link list
一串不連續的記憶體，資料存在每個`node`中，每個`node`都會指向前一個和後一個`node`。  
`_head`指向一個`dummy node`，這個`node`將這個list串成一個環，所以dummy的前一個為`back()`，而後一個則是`begin()`。  
`iterator`指向`node`，而`iterator`operator`+` `-`的實作則需要一個一個往前或往後。  
`sort()`因為不能直接指到指定的node，所以使用氣泡排序，並實作`swap_node()`以更改node的`_prev` `_next`交換node。  

### Binary search tree
一顆二元樹，左子端點永遠小於或等於父親，右子端點則永遠大於父親。  
`_size`儲存子樹端點數量(包括自己)，並在呼叫`erase()` `insert()`時維護。  
BSTree中`_maximum`為dummy node，將其當作最大的node，所以所有的資料都在其左子樹。  
找某點的次大端點(++iterator)：如果有右子端點，先向右子端點走一步再向左子端點走到底，如果沒有右子端點則向左父親走到底再向右父親走一步。如果是最大的端點則回傳`NULL`；次小端點比照辦理。  
`erase()`在有兩個子端點時，將successor(次大端點)的資料複製至要刪除的node，並刪除successor。  
`clear()`用PostOrder依序erase(node)，因為刪除的都是樹葉，可以省去複製資料的時間。  

## 實驗
```
adta -r 100000
usage
adtd -front 100000
usage

adta -r 100000
usage
adtd -back 100000
usage

adta -r 100000
usage

adtp
usage

adtp -r
usage

adts
usage

adtd -a
usage

q -f
```
### Dynamic Array
```
-- adta -r 100000
Period time used : 0.01 seconds
Total time used  : 0.01 seconds
Total memory used: 7.445 M Bytes
-- adtd -front 100000
Period time used : 0 seconds
Total time used  : 0.01 seconds
Total memory used: 7.445 M Bytes
-- adta -r 100000
Period time used : 0.01 seconds
Total time used  : 0.02 seconds
Total memory used: 7.445 M Bytes
-- adtd -back 100000
Period time used : 0 seconds
Total time used  : 0.02 seconds
Total memory used: 7.445 M Bytes
-- adta -r 100000
Period time used : 0 seconds
Total time used  : 0.02 seconds
Total memory used: 7.445 M Bytes
-- adtp
Period time used : 0.02 seconds
Total time used  : 0.04 seconds
Total memory used: 7.445 M Bytes
-- adtp -r
Period time used : 0 seconds
Total time used  : 0.04 seconds
Total memory used: 7.445 M Bytes
-- adts
Period time used : 0.02 seconds
Total time used  : 0.06 seconds
Total memory used: 7.445 M Bytes
-- adtd -a
Period time used : 0 seconds
Total time used  : 0.06 seconds
Total memory used: 7.445 M Bytes
```
### Double link list
```
-- adta -r 100000
Period time used : 0 seconds
Total time used  : 0 seconds
Total memory used: 7.473 M Bytes
-- adtd -front 100000
Period time used : 0 seconds
Total time used  : 0 seconds
Total memory used: 7.473 M Bytes
-- adta -r 100000
Period time used : 0.01 seconds
Total time used  : 0.01 seconds
Total memory used: 7.473 M Bytes
-- adtd -back 100000
Period time used : 0 seconds
Total time used  : 0.01 seconds
Total memory used: 7.473 M Bytes
-- adta -r 100000
Period time used : 0.01 seconds
Total time used  : 0.02 seconds
Total memory used: 7.473 M Bytes
-- adtp
Period time used : 0.02 seconds
Total time used  : 0.04 seconds
Total memory used: 7.473 M Bytes
-- adtp -r
Period time used : 0.02 seconds
Total time used  : 0.06 seconds
Total memory used: 7.473 M Bytes
-- adts
Period time used : 117 seconds
Total time used  : 117 seconds
Total memory used: 7.473 M Bytes
-- adtd -a
Period time used : 0.01 seconds
Total time used  : 117.1 seconds
Total memory used: 7.473 M Bytes
```
### Binary Search Tree
```
-- adta -r 100000
Period time used : 0.04 seconds
Total time used  : 0.04 seconds
Total memory used: 9.023 M Bytes
-- adtd -front 100000
Period time used : 0.01 seconds
Total time used  : 0.05 seconds
Total memory used: 9.023 M Bytes
-- adta -r 100000
Period time used : 0.08 seconds
Total time used  : 0.13 seconds
Total memory used: 9.023 M Bytes
-- adtd -back 100000
Period time used : 0.01 seconds
Total time used  : 0.14 seconds
Total memory used: 9.023 M Bytes
-- adta -r 100000
Period time used : 0.07 seconds
Total time used  : 0.21 seconds
Total memory used: 9.023 M Bytes
-- adtp
Period time used : 0.02 seconds
Total time used  : 0.23 seconds
Total memory used: 9.023 M Bytes
-- adtp -r
Period time used : 0.03 seconds
Total time used  : 0.26 seconds
Total memory used: 9.023 M Bytes
-- adts
Period time used : 0 seconds
Total time used  : 0.26 seconds
Total memory used: 9.023 M Bytes
-- adtd -a
Period time used : 0.01 seconds
Total time used  : 0.27 seconds
Total memory used: 9.023 M Bytes
```
