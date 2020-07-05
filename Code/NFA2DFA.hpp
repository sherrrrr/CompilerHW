#include "common.hpp"


class NFA2DFA{
public:
    static DFAPtr translate(NFAPtr nfa)
    {
        State::init();
        return convert(nfa);
    }
private:
    // 接受一个输入之后的状态
    static set<StatePtr> move(char ch, set<StatePtr>& origin)
    {
        set<StatePtr> res;
        for(auto node : origin)
        {
            if(node -> StateMap.find(ch) == node -> StateMap.end()) continue;
            for(auto next : node -> StateMap[ch])
                res.insert(next);
        }
        return res;
    }

    // 得到闭包
    static set<StatePtr> getClosure(set<StatePtr>& origin)
    {
        set<StatePtr> res(origin);
        queue<StatePtr> q;
        for(auto i : origin){
            q.push(i);
        }
        while(!q.empty()){
            StatePtr node = q.front();
            q.pop();
            if(node -> StateMap.find('E') == node -> StateMap.end()) continue;
            for(auto next : node -> StateMap['E']){
                if(res.count(next) == 0){
                    q.push(next);
                    res.insert(next);
                }
            }
        }
        return res;
    }

    // 假如还不存在NFA集合对应的DFA节点，则创建节点，并根据集合内是否有接受状态
    // 改变DFA节点的接受状态
    static void addNewDFAState(map<set<StatePtr>, StatePtr>& m, set<StatePtr>& s)
    {
        m[s] = make_shared<State>();
        for(auto state : s)
            if(state -> myid == -1)
            {
                m[s] -> myid *= -1;
            }
    }

    static DFAPtr convert(NFAPtr nfa)
    {
        // NFA状态集合到DFA状态的映射
        map<set<StatePtr>, StatePtr> m;

        // 初始化最初的DFA节点
        set<StatePtr> in = {nfa -> in};
        set<StatePtr> st = getClosure(in);
        DFAPtr res = make_shared<DFA>();
        addNewDFAState(m, st);
        res -> st = m[st];
        res -> states.push_back(res -> st);
        // 更新映射，得到DFA
        queue < set<StatePtr> > q;
        q.push(st);
        while(!q.empty())
        {
            set<StatePtr> now = q.front();
            q.pop();
            for(auto ch : DFAChars)
            {
                set<StatePtr> newset = move(ch, now);
                newset = getClosure(newset);
                if(newset.size() == 0)  continue;
                if(m.find(newset) == m.end()){
                    addNewDFAState(m, newset);
                    q.push(newset);
                    res -> states.push_back(m[newset]);
                }
                // DFA 的节点连接
                m[now] ->addedge(ch, m[newset]);
            }
        }
        return res;
    }

};