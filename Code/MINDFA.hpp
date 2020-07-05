#include "common.hpp"

class MINDFA{
public:
    static int groupId;
    static map<StatePtr, int> groupIdMap;
    static DFAPtr minimize(DFAPtr origin)
    {
        init();
        return convert(origin);
    }
private:
    static void init()
    {
        groupId = 1;
        groupIdMap.clear();
    }

    // 初始化分组，以是否为接受状态做划分
    static set<vector<StatePtr> > initSet(vector<StatePtr>& states)
    {
        vector<StatePtr> accepts;
        vector<StatePtr> unaccepts;
        for(auto p : states){
            if(p -> myid < 0){
                groupIdMap[p] = 233;
                accepts.push_back(p);
            }
            else{
                groupIdMap[p] = 233;
                unaccepts.push_back(p);
            }
        }
        set<vector<StatePtr> > res = {accepts, unaccepts};
        UpdateGroupIdMap(res);
        return res;
    }

    // 划分之后更新每一个State对应的组号id
    static void UpdateGroupIdMap(set<vector<StatePtr> >& groupSet)
    {
        for(auto sset : groupSet)
        {
            ++groupId;
            for(auto s : sset) groupIdMap[s] = groupId;
        }
    }

    // 对于每一个group内的State，以他们接受一个字符之后进入的下一个State的
    // 组号作为划分的key
    static set<vector<StatePtr> > partitionGroup(vector<StatePtr>& states)
    {
        // 根据每个State接受字符之后的下一个状态做映射
        map<map<char, int>, vector<StatePtr> > m;
        for(auto s : states)
        {
            map<char, int > key;
            for(auto ch : DFAChars)
            {
                if(s -> StateMap.find(ch) != s -> StateMap.end())
                    key[ch] = groupIdMap[s -> StateMap[ch][0]];
            }
            if(m.find(key) == m.end()) m[key] = {s};
            else m[key].push_back(s);
        }

        set<vector<StatePtr> > newset;
        for(auto kv : m)
            newset.insert(std::move(kv.second));
        // 更新新的id
        if(newset.size() > 1)
            UpdateGroupIdMap(newset);
        return newset;

    }

    // 对DFA中所有的State做划分
    static set<vector<StatePtr> > partition(vector<StatePtr>& states)
    {
        // 分为accept和不可accpet的
        set<vector<StatePtr> > groupSet = initSet(states);
        set<vector<StatePtr> > tmp;
        size_t presize = 0;
        while(tmp.size() != groupSet.size())
        {
            tmp.clear();
            for(auto sset : groupSet)
            {
                set<vector<StatePtr> > newGroupSet = partitionGroup(sset);
                tmp.insert(newGroupSet.begin(), newGroupSet.end());
            }
            tmp.swap(groupSet);
        }
        return groupSet;
    }

    // 生成最小DFA时，根据分组的id得到对应的DFA节点，还不存在的话则会去创建
    static StatePtr getId2StatePtr(StatePtr origin, map<int, StatePtr>& id2StatePtr)
    {
        int id = groupIdMap[origin];
        if(id2StatePtr.find(id) == id2StatePtr.end())
        {
            id2StatePtr[id] = make_shared<State>();
            // 判断是否为接受状态
            if(origin -> myid < 0) id2StatePtr[id] -> myid *= -1;
        }
        return id2StatePtr[id];
    }

    // 根据现有的划分group创建新的DFA
    static DFAPtr build(set<vector<StatePtr> >& groupSet, StatePtr startPtr)
    {
        map<int, StatePtr> id2StatePtr;
        DFAPtr res = make_shared<DFA>();
        for(auto sset : groupSet)
        {
            StatePtr origin = sset[0];
            int id = groupIdMap[origin];
            StatePtr now = getId2StatePtr(origin, id2StatePtr);
            for(auto kv : origin -> StateMap)
            {
                now -> addedge(kv.first, getId2StatePtr(kv.second[0], id2StatePtr));
            }
        }

        // 找到起始状态
        res -> st = id2StatePtr[groupIdMap[startPtr]];
        // DEBUG(id2StatePtr[groupIdMap[startPtr]]);
        return res;
    }

    static DFAPtr convert(DFAPtr origin)
    {
        set<vector<StatePtr> > groupSet = partition(origin -> states);
        State::init();
        DFAPtr res =  build(groupSet, origin -> st);
        return res;
    }
};

int MINDFA::groupId = 1;
map<StatePtr, int> MINDFA::groupIdMap;
