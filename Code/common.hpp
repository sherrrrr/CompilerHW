#pragma once
#include <map>
#include <string>
#include <memory>
#include <vector>
#include <set> 
#include <iostream>
#include <queue>
using namespace std;

#define DEBUG(x) cout << "fun:[" << __FUNCTION__ << "],line:[" << __LINE__ << "] " << #x << " : " << x << endl;

const string DFAChars = "abcdefghijklmnopqrstuvwxyz*+?|";
// 字符集 + ? * | a-z E
class State;

using StatePtr = shared_ptr<State>;

// 用于表示一个状态
class State{
    static int id;
public:
    int myid;
    map<char, vector<StatePtr> > StateMap; 

    static void init()
    {
        id = 1;
    }

    State()
    {
        myid = id++;
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

    bool operator<=(StatePtr another)
    {
        // TODO 子集，因此another能接受的，this可以不接受，another不能接受的，this一定不能接受
    }
};

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
    
    bool operator<=(DFAPtr another)
    {
        // TODO
    }
};
