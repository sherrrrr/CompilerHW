#include "common.hpp"
#include <exception>
class RE2NFA{
public:
    static NFAPtr translate(string& exp)
    {
        State::init();
        NFAPtr res = convert(exp);
        StatePtr end = make_shared<State>();
        end -> myid = -1; // 结束状态
        res -> out -> addedge('E', end);
        res -> out = end;
        return res;
    }

private:
    // 为 * + ？
    static bool is_special(char ch)
    {
        return ch == '*' || ch == '+' || ch == '?';
    }

    // 为a-z
    static bool is_base(char ch)
    {
        return (ch >= 'a' && ch <= 'z') || ch == 'E' ;
    }

    static size_t findClose(string& exp, size_t st)
    {
        int count = 0;
        for(size_t i = st; i < exp.length(); ++i)
        {
            if(exp[i] == '(') ++count;
            else if(exp[i] == ')')
            {
                if(count == 1) return i;
                else --count;
            }
        }
        return -1;
    }

    // 统一一下形式，还是返回一下吧
    static NFAPtr str2NFA(string& innerExp, int& pos, NFAPtr pre)
    {
       NFAPtr next;
       char lastChar = innerExp[innerExp.size() - 1];
       switch(lastChar){
            case '*' : {
                string iinnerExp = innerExp.substr(0, innerExp.size() - 1);
                next = start2NFA(iinnerExp);
                break;
            }
            case '+' : {
                string iinnerExp = innerExp.substr(0, innerExp.size() - 1);
                next = add2NFA(iinnerExp);
                break;
            }
            case '?' : {
                string iinnerExp = innerExp.substr(0, innerExp.size() - 1);
                next = q2NFA(iinnerExp);
                break;
            }
            case ')' : {
                // 读到)表示(xxx)*+?已经递归到了(xxx)，取出xxx即可
                string iinnerExp = innerExp.substr(1, innerExp.size() - 2);
                next = str2NFA(iinnerExp);
                break;
            }
            default : {
                next = str2NFA(innerExp);
                break;
            }
       }
       pos += innerExp.length();
       if(!pre) return next;
       pre -> link(next);
       return pre;
    }

    // a|b形式，此时a为NFA形式，b为字符串形式
    static NFAPtr or2NFA(NFAPtr first, string& second_s)
    {
        NFAPtr second = convert(second_s);
        NFAPtr res = make_shared<NFA>();
        res -> in -> addedge('E', first -> in);
        res -> in -> addedge('E', second -> in);
        first -> out -> addedge('E', res -> out);
        second -> out -> addedge('E', res -> out);
        return res;
    }

    // 一个字符转NFA
    static NFAPtr str2NFA(string& exp)
    {
        if(exp.length() == 1 && is_base(exp[0]))
            return make_shared<NFA>(exp[0]);
        else return convert(exp);
    }

    // 正则表达式后跟*的,只传入不包括*的部分
    static NFAPtr start2NFA(string& exp)
    {
        NFAPtr inner = convert(exp);
        NFAPtr res = make_shared<NFA>('E');
        res -> in -> addedge('E', inner -> in);
        inner -> out -> addedge('E', res -> out);
        inner -> out -> addedge('E', inner -> in);
        return res;
    }

    // 正则表达式后跟?的,只传入不包括?的部分
    static NFAPtr q2NFA(string& exp)
    {
        NFAPtr inner = convert(exp);
        NFAPtr res = make_shared<NFA>('E');
        res -> in -> addedge('E', inner -> in);
        inner -> out -> addedge('E', res -> out);
        // inner -> out -> addedge('E', inner -> in);
        return res;
    }

    // 正则表达式后跟?的,只传入不包括?的部分
    static NFAPtr add2NFA(string& exp)
    {
        NFAPtr inner = convert(exp);
        NFAPtr res = make_shared<NFA>();
        res -> in -> addedge('E', inner -> in);
        inner -> out -> addedge('E', res -> out);
        inner -> out -> addedge('E', inner -> in);
        return res;
    }

    static NFAPtr convert(string& exp)
    {
        NFAPtr res;
        exp += '#';
        size_t len = exp.length();
        int pos = 0;
        while(1)
        {
            // # 结束
            if(exp[pos] == '#') break;
            // a-z 根据下一个字符判断生成
            if( is_base(exp[pos]) ){
                //找到下一个不是*+?的字符
                int endpos = pos;
                while(is_special(exp[++endpos])) ;
                string innerExp = exp.substr(pos, endpos - pos);
                res = str2NFA(innerExp, pos, res);
            }
            else if(exp[pos] == '('){
                //括号匹配，找到相对应的下一个括号
                size_t closePos = findClose(exp, pos);
                if(closePos == -1){
                    if(pos >= 1)
                        cerr << "ERROR when find ) RE : " + exp.substr(pos - 1);
                    else
                        cerr << "ERROR when find ) RE : " + exp.substr(pos);
                    exit(0);
                }
                //找到下一个不是*+?的字符
                int endpos = closePos;
                while(is_special(exp[++endpos])) ;
                string innerExp = exp.substr(pos, endpos - pos);
                //
                res = str2NFA(innerExp, pos, res);
            }
            else if(exp[pos] == '|')
            {
                string second = exp.substr(pos + 1, exp.length() - 1 - pos - 1);
                res = or2NFA( res, second);
                break;
            }
            else{
                if(pos >= 1)
                    cerr << "ERROR when parse RE : " + exp.substr(pos - 1);
                else
                    cerr << "ERROR when parse RE : " + exp.substr(pos);
                exit(0);
            }
        }
        return res;
    }
};
