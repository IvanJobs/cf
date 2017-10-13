# cf
Command Line Tool For Codeforces.

# 为什么要开发这个项目？
本人自认为对算法抱有极大的兴趣，但一直停在初级玩家的瓶颈之上。究其原因，有一部分是因为工作之后时间不够，另外也有一部分是自己的惰性导致，虽然如此，但内心一直抱有热情，时常参加Codeforces比赛。Codeforces是一个有时间限制的比赛，能够快一分钟，对比赛成绩的影响是可观的，所以如果有一个好用的CLI工具，能够减少我们在贴用例、敲编译命令、测试等过程中的时间，那么会给比赛带来极大的帮助。这里选择C++语言作为基础，开发一个好用的CLI，并且也是服务于使用C++参与比赛的选手。

# 特性列表
1. 用例管理：添加、查看、删除
2. 代码模版：默认代码模版 

# CLI 

cf help // show help messages.
cf new {contest_name} // new a contest.
cf case add // add a case
cf case ls // list test cases
cf case del // delete test cases
