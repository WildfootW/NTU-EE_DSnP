# adtComp
## 資料結構的實作
### Dynamic Array
用一段連續的記憶體存放資料，並在空間不夠時自動擴展。
`_data`存放其開頭位址;`_size`紀錄目前有多少(個)資料存放在其中;`_capacity`儲存目前new了多少記憶體來放資料(個);`isSorted`在呼叫`sort()`後被設成`true`，並在`erase()`、`push_back()`等操作後被設為`false`。
一開始的`_capacity`為0，而每次`push_back()`但空間不夠時自動擴展`_size + 1`的空間(總共為`_size * 2 + 1`)。
`end()`會回傳最後一個有效資料的後一個位置。
`erase()`會直接把最後一個資料複製到要刪除的資料的位置，並把`_size` - 1。
`clear()`僅把`_size`設為0
`sort()`使用STL中的`sort()`
### Double link list
一串不連續的記憶體，資料存在每個`node`中，每個`node`都會指向前一個和後一個`node`。
`iterator`指向`node`，而`iterator`operator`+`的實作

### Binary search tree
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
