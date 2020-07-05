# 编译原理实验课实验报告

## 算法思路

### 一. 实现正则表达式转最小DFA接口

* 1. 正则表达式词法分析
* 2. 正则表达式转NFA
* 3. NFA转DFA
* 4. 最小化DFA

首先是数据结构方面。`State`作表示`DFA`和`NFA`的状态，用一个`int`字段表示id和`map<char,vector<State> >`表示状态的转移。

`NFA`的数据结构就是两个`State`，方便之后正则表达式到`NFA`的转换。

`DFA`就是一个`State`，只需要记录入口的`State`就够了

##### 1、2 正则转NFA

其中因为正则表达式比较简单，因此1，2步骤可以和在一起做，边读正则表达式变转化到`NFA`。思路就是ppt上的做法，每个正则表达式的部分拿出来转为`NFA`，多创建两个`State`节点`st`，`ed`。

* `E`和`a-z`

    创建两个节点之后在中间建立一条有向边即可,`st --ch--> ed`。

* `*+?`

    思路就是ppt上的，以`*`为例，就是创建两个节点`st`,`ed`之后，用`E`创建一条`st --E--> ed`边，然后对于`*`修饰的部分，也只需要得到这一部分的开头`inner_st`和结尾`inner_ed`两个节点，然后连上三条`E`的边，`st --E--> inner-st`,`ed --E--> inner_ed`, `inner_ed --E--> inner_st`。`+?`也类似，就是连接边的时候有一些区别。

* `|`

    读到`|`时，此时前半部分是已经解析成`NFA`的数据结构，后半部分是字符串，则调用正则转NFA的接口，得到后半部分的`NFA`数据结构，然后连上四条`E`的边即可

* `(`

    读到`(`时，直接找到与之匹配的`)`，然后取出其中的字符串调用转换接口函数即可。

* 连接

    当对新读到的字符串解析后，需要将新的NFA部分连接上原有的部分，是现实直接在原有部分的`ed`连上一条`E`边到新的`st`，然后将原有的`ed`更新为新的部分的`ed`。

* 其他

    还有具体实现时需要注意的一点就是，因为是连接时连上了就不会记录中间`State`信息了。因此例如`ba*`，假如读到`a`直接就转化，而之后再读到`*`的话就不好处理，因此对于单个字符以及`(xxx)`，在读取是都会先判断后一个字符是不是`*+?`，再进行处理。

    在转换完成之后，需要记录一个State为接受状态，实现时是利用State的id字段记录的，如果为-1，则表示为接受状态，并且转为NFA之后一定只有最后的那个State才需要标记为接受状态。

##### 3 NFA转DFA

实现就是与之前的实验课的NFA类似，实现`move`以及`Closure`，得到`DFA`的每一个`State`。

实现时，`NFA`转为`DFA`，因为此时可能会有多个接受状态，判断一个`DAF`状态对应的NFA状态集合中是否存在id为-1的状态，对于每一个`State`，假如id小于0，则表示为`ACCEPT`状态。

##### 4 最小化DFA

这一块实现时比较麻烦。

主要即使分为两步，第一步，对所有的State做划分，第二步，根据划分出来的集合生成对应的`DFA`。

数据结构方面，在`DFA`中用一个`vector`记录了所有的State节点。采用一个从`State`到`int`的`State2int map`映射，记录每一个`State`所在划分集合。还有就是一个`set<vector<State>>`,用于记录现在划分的状态，在之后查询输入字符之后的状态的集合编号时要用到。

首先是要划分为接受状态集合和非接受状态集合。这一步就是根据id的正负做出划分。然后就是对于现有划分的每一个vector做判断，判断是否会被划分到同一个类别。判断有依据就是接受了输入之后到的状态的集合编号，因此采用`map<char,int>`作为每一个`State`的划分依据，然后采用`map<map<char,int>,vector<State> >`作为划分的结果，同时，对于划分的结果，对其中每一个`vector`，更新这个`vector`中的`State`在`State2int`中的编号。就这样一直划分下去直到没有变化。

然后就是生成最小`DFA`了，就是遍历`set<vector<State>>`中的每一个`vector`，对于每一个`vector`，都采用第一个`State`作为生成边的依据即可。



#### 二. 比较两个DFA

调用上述接口，生成两个最小DFA，

#### 比较DFA通过判断一个DFA是否为另一个的子集实现

一个DFA `d1`是另一个DFA `d2`的子集，则`d1`能接受的，`d2`一定接受，`d1`不能接受的，`d2`可以不能接受，同时 `d1`中能接受的字符，`d2`一定也要能接受。

思路就是判断`d1 <= d2`时，以`d1`为主导，遍历`d1`的状态，用上面的规则判断。

然后利用 `<=` 实现 `<`的逻辑，即 `d1 < d2` 需要 `d1 <= d2` 且 `!d2 <= d1`,`>=`同理。
`==`的实现就是 `d1 <= d2` 且 `d2 <= d1`，最后就是不等于了。


## 测试

#### 一. 基本测试

每个测试用例主要就是针对某几个符号进行测试。

* 单个符号

    `E E` `a a` `a E` `a b`测试最基本的正则表达式的正确性

* `*+?`

    两两配对测试，`a* a+`,`a? a+`,`a? a*`

* `|`

    对于`|`，对于形如`A | B`的正则表达式和`C`

    假设`B ! C`

    假如`A = C`，则`A|B > C`

    假如`A < C`，则`A|B ! C`

    假如`A > C`，则`A|B > C`

    假如`A ! C`，则`A|B ! C`

* 括号

    `(a) a` `(a)+ a` `(a)* a` `(a)? a`

* 连接

    `ab a`

然后做了一些复杂一点的正则表达式的测试

```
(a*b*)* E|(a|b)* =
(ab)* E|a(ba)*b =
a*(a|b)* (a|b)*a* =
(a|b)*a* b*(a|b)* =
b*(a|b)* (a|b)*b* =
(a|b)*b* (a|b)* =
(a|b)* a*(a|b)* =
(a|b)*abb (a|b)*|a+ab <
(a|b)*abb (a|b)*a*a|b !
(a|b)*a?ab (a|b)*a*a|b !
(a|b)*abb (a|b)*a?bb <
(a|b)*|a+ab (a|b)*a?ab >
(a|b)*|a+ab (a|b)*a*a|b >
(a|b)*|a+ab (((a|b)))*|a+ab =
```

#### 二. 复杂度测试

测试了长度1000和一个长度为2000的，为前一个正则|上另一个长度为1000的正则表达式，结果正确，基本也能够秒出结果。

`ba+sdg*h(a+s?d|g*h((a|b)*|a+ab(((ba+sdg*h(a+s?d|g*h((a|b)*|a+ab(((a|b)))*|a+ab)+)agh?e*agba+s(d(a|b)*|a+ab(((a|b)))*|a+ab)*h(a+s?d|g*ho+)agh?e*agoa(hogh|o?ho)?g?aba+sdg*h(a+s?d|g*h((a|b)*|a+ab(((a|b)))*|a+ab)+)agh?e*agba+s(d(a|b)*|a+ab(((a|b)))*|a+ab)*h(a+s?d|g*ho+)agh?e*agoa(hogh|o?ho)?g?ahba+sdg*h(a+s?d|g*h((a|b)*|a+ab(((a|b)))*|a+ab)+)agh?e*agba+s(d(a|b)*|a+ab(((a|b)))*|a+ab)*h(a+s?d|g*ho+)agh?e*a(ba+sdg*h(a+s?d|g*h((a|b)*|a+ab(((a|b)))*|a+ab)+)agh?e*agba+s(d(a|b)*|a+ab(((a|b)))*|a+ab)*h(a+s?d|g*ho+)agh?e*agoa(hogh|o?ho)?g?ahoa(hoba+sdg*h(a+s?d|g*ho+)agh?e*agoa(hogh|o?ho)?g?ahgh|o?ho)?g?ah)oa(hogh|o?ho)?g?ahoa(hoba+sdg*h(a+s?d|g*ho+)agh?e*agoa(hogh|o?ho)?g?ahgh|o?ho)?g?aha(hoba+sdg*h(a+s?d|g*ho+)agh?e*agoa(hogh|o?ho)?g?ahgh|o?ho)?g?ahoa(hoba+sdg*h(a+s?d|g*ho+)agh?e*agoa(hogh|o?ho)?g?ahgh|o?ho)?g?ah|b)))*|a+ab)+)agh?e*agba+s(d(a|b)*|a+ab(((a|b)))*|a+ab)*h(a+s?d|g*ho+)agh?e*agoa(hogh|o?ho)?g?ahoa(hoba+sdg*h(a+s?d|g*ho+)agh?e*agoa(hogh|o?ho)?g?ahgh|o?ho)?g?ah`  和 `hf*a(u?g+i|fo*ay+oahf*a(u?g+i|fo*ay+oa(hf*a(u?g+i|fo*ay+oaw*)eh?s+eabgo*(ia+f?ggzby*ga)+w?o((a|b)*abb(a|b)*a*a|b)?(aff(a|b)*abb(a|b)*a*a|bff)*afh*)*)eh?s+e(hf*a(u?g+i|fo*ay+oahf*a(u?g+i|fo*ay+oa(hf*a(u?g+i|fo*ay+oaw*)eh?s+eabgo*(ia+f?ggzby*ga)+w?o((a|b)*abb(a|b)*a*a|b)?(aff(a|b)*abb(a|b)*a*a|bff)*afh*)*)eh?s+ehf*a(u?g+i|fo*ay+oahf*a(u?g+i|fo*ay+oa(hf*a(u?g+i|fo*ay+oaw*)eh?s+eabgo*(ia+f?ggzby*ga)+w?o((a|b)*abb(a|b)*a*a|b)?(aff(a|b)*abb(hf*a(u?g+i|fo*ay+oahf*a(u?g+i|fo*ay+oa(hf*a(u?g+i|fo*ay+oaw*)eh?s+eabgo*(ia+f?ggzby*ga)+w?o((a|b)*abb(a|b)*a*a|b)?(aff(a|b)*abb(a|b)*a*a|bff)*afh*)*)eh?s+eabgo*(ia+f?ggzby*ga)+w?oafh*w*)eh?s+eabgo*(ia+f?ggzby*ga)+w?oafh*|b)*a*a|bff)*afh*)*)eh?s+eabgo*(ia+f?ggzby*ga)+w?oafh*w*)eh?s+eabgo*(ia+f?ggzby*ga)+w?oafh*bgo*(ia+f?ggzby*ga)+w?oafh*w*)eh?s+eabgo*(ia+f?ggzby*ga)+w?oafh*)?bgo*(ia+f?ggzby*ga)+w?oafh*w*)eh?s+eabgo*(ia+f?ggzby*ga)+w?oafh*|ba+sdg*h(a+s?d|g*h((a|b)*|a+ab(((ba+sdg*h(a+s?d|g*h((a|b)*|a+ab(((a|b)))*|a+ab)+)agh?e*agba+s(d(a|b)*|a+ab(((a|b)))*|a+ab)*h(a+s?d|g*ho+)agh?e*agoa(hogh|o?ho)?g?aba+sdg*h(a+s?d|g*h((a|b)*|a+ab(((a|b)))*|a+ab)+)agh?e*agba+s(d(a|b)*|a+ab(((a|b)))*|a+ab)*h(a+s?d|g*ho+)agh?e*agoa(hogh|o?ho)?g?ahba+sdg*h(a+s?d|g*h((a|b)*|a+ab(((a|b)))*|a+ab)+)agh?e*agba+s(d(a|b)*|a+ab(((a|b)))*|a+ab)*h(a+s?d|g*ho+)agh?e*a(ba+sdg*h(a+s?d|g*h((a|b)*|a+ab(((a|b)))*|a+ab)+)agh?e*agba+s(d(a|b)*|a+ab(((a|b)))*|a+ab)*h(a+s?d|g*ho+)agh?e*agoa(hogh|o?ho)?g?ahoa(hoba+sdg*h(a+s?d|g*ho+)agh?e*agoa(hogh|o?ho)?g?ahgh|o?ho)?g?ah)oa(hogh|o?ho)?g?ahoa(hoba+sdg*h(a+s?d|g*ho+)agh?e*agoa(hogh|o?ho)?g?ahgh|o?ho)?g?aha(hoba+sdg*h(a+s?d|g*ho+)agh?e*agoa(hogh|o?ho)?g?ahgh|o?ho)?g?ahoa(hoba+sdg*h(a+s?d|g*ho+)agh?e*agoa(hogh|o?ho)?g?ahgh|o?ho)?g?ah|b)))*|a+ab)+)agh?e*agba+s(d(a|b)*|a+ab(((a|b)))*|a+ab)*h(a+s?d|g*ho+)agh?e*agoa(hogh|o?ho)?g?ahoa(hoba+sdg*h(a+s?d|g*ho+)agh?e*agoa(hogh|o?ho)?g?ahgh|o?ho)?g?ah`