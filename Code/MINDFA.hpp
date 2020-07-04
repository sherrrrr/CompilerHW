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

    static void UpdateGroupIdMap(set<vector<StatePtr> >& groupSet)
    {
        for(auto sset : groupSet)
        {
            ++groupId;
            for(auto s : sset) groupIdMap[s] = groupId;
        }
    }


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

    static set<vector<StatePtr> > partition(vector<StatePtr>& states)
    {
        // 分为accept和不可accpet的
        set<vector<StatePtr> > groupSet = initSet(states);
        size_t presize = 0;
        while(presize != groupSet.size())
        {
            presize = groupSet.size();
            for(auto sset : groupSet)
            {
                set<vector<StatePtr> > newGroupSet = partitionGroup(sset);
                groupSet.erase(sset);
                groupSet.insert(newGroupSet.begin(), newGroupSet.end());
            }
        }
        return groupSet;
    }

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
