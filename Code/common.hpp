#pragma once
#include <map>
#include <string>
#include <memory>
#include <vector>
#include <set>
#include <iostream>
#include <queue>
using namespace std;

#define DEBUG(x) cout << "fun:[" << __FUNCTION__ << "],line:[" << __LINE__ << "] " << #x << " : " << (x) << endl;

const string Chars = "abcdefghijklmnopqrstuvwxyz*+?|";
// 字符集 + ? * | a-z E
class State;

using StatePtr = shared_ptr<State>;

// 用于表示一个状态
class State{
    static int id;
public:
    int myid;
    // 在NFA中一个char对应多个StatePtr，但是在DFA中其实vector中只有一个元素
    map<char, vector<StatePtr> > StateMap;

    static void init()
    {
        id = 1;
    }

    State()
    {
        myid = id++;
    }

    State(int num)
    {
        myid = num;
    }

    void addedge(char ch, StatePtr nextState)
    {
        if(StateMap.find(ch) == StateMap.end()){
            StateMap[ch] = {nextState};
        }
        else{
            StateMap[ch].push_back(nextState);
        }
    }

    void display(string prefix, set<int>& visited)
    {
        prefix += "    ";
        if(visited.count(myid))
        {
            // cout << prefix << myid << "$"<< endl;
            return;
        }
        visited.insert(myid);
        cout << prefix << myid << " -> " << endl;
        for(auto kv : StateMap)
        {
            cout << prefix << kv.first << " : " ;
            for(auto s : kv.second)
                cout << s -> myid << " ";
            cout << endl;
        }
        for(auto kv : StateMap)
        {
            for(auto s : kv.second)
                s -> display(prefix, visited);
        }
    }
};

bool operator<=(StatePtr l, StatePtr r)
{
    set<pair<StatePtr, StatePtr>> visited;
    // 子集，因此rhs能接受的，this可以不接受，rhs不能接受的，this一定不能接受
    // 同时，this中能够接受的字符，rhs中也必须要能接受
    queue<pair<StatePtr, StatePtr> > q;
    q.push(make_pair(l, r));
    while(!q.empty())
    {
        auto val = q.front();
        visited.insert(val);
        q.pop();
        visited.insert(val);
        auto lhs = val.first;
        auto rhs = val.second;
        // lhs 接受， rhs不接受，则不<=
        if(lhs -> myid < 0 && rhs -> myid > 0) return false;
        for(auto kv : lhs -> StateMap)
        {
            if(rhs -> StateMap.find(kv.first) == rhs -> StateMap.end()){
                return false;
            }
            else if(visited.count(make_pair(kv.second[0], rhs -> StateMap[kv.first][0])) == 0){
                q.push(make_pair(kv.second[0], rhs -> StateMap[kv.first][0]));
            }
        }
    }
    return true;
}

int State::id = 1;


class NFA;
using NFAPtr = shared_ptr<NFA>;
// 用于表示正则表达式的一部分的NFA表示
class NFA{
public:
    StatePtr in; // 入口State
    StatePtr out; // 出口State

    // 每次创建NFA都会固定增加两个State
    NFA()
    {
        in = make_shared<State>();
        out = make_shared<State>();
    }

    NFA(char ch)
    {
        in = make_shared<State>();
        out = make_shared<State>();
        in -> addedge(ch, out);
    }

    void link(NFAPtr next)
    {
        out -> addedge('E', next -> in);
        out = next -> out;
    }

    void display()
    {
        set<int> visited;
        in -> display("", visited);
    }
};


class DFA;

using DFAPtr = shared_ptr<DFA>;

class DFA{
public:
    StatePtr st;
    vector<StatePtr> states; // 记录所有的节点，方便做最小化
    void display()
    {
        set<int> visited;
        st -> display("", visited);
    }
};

bool operator<=(DFAPtr lhs, DFAPtr rhs)
{
    return (lhs ->st <= rhs -> st);
}
// 注意这边的实现并不是 [ a<b = !(a>=b) ]这种
bool operator<(DFAPtr lhs, DFAPtr rhs)
{
    return lhs <= rhs && !(rhs <= lhs);
}

bool operator>(DFAPtr lhs, DFAPtr rhs)
{
    return rhs <= lhs && !(lhs <= rhs);
}

bool operator==(DFAPtr lhs, DFAPtr rhs)
{
    return (lhs <= rhs) && (rhs <= lhs);
}
