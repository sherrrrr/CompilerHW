#include "RE2NFA.hpp"
#include "NFA2DFA.hpp"
#include "MINDFA.hpp"
#include "stdio.h"
int show_level = 1;
DFAPtr translate(string re)
{
    if(show_level > 1) cout << "************************************ TEST ************************************" << endl;

    if(show_level > 1) cout << "************************************ RE2NFA ************************************" << endl;
    NFAPtr nfa = RE2NFA::translate(re);
    if(show_level > 1)  nfa -> display();

    if(show_level > 1) cout << "************************************ NFA2DFA ************************************" << endl;
    DFAPtr dfa = NFA2DFA::translate(nfa);
    if(show_level > 1)  dfa -> display();

    if(show_level > 0) cout << "************************************ MINDFA ************************************" << endl;
    DFAPtr mindfa = MINDFA::minimize(dfa);
    if(show_level > 0) mindfa -> display();

    if(show_level > 0) cout << endl;

    return mindfa;
}

void judge(string re1, string re2)
{
    auto res1 = translate(re1);
    auto res2 = translate(re2);
    if(res1 == res2) cout << "=" << endl;
    else if(res1 > res2) cout << ">" << endl;
    else if(res1 < res2) cout << "<" << endl;
    else cout << "!" << endl;
}


void test1()
{
    string re1 = "(a|b)*abb";
    string re2 = "(a|b)*a?ab";
    string re3 = "(a|b)*|a+ab";
    string re4 = "(a|b)*a*a|b";
    judge(re1, re2);
    judge(re1, re1);
}

void test2()
{
    string eq11 = "(a*b*)*";
    string eq12 = "E|(a|b)*";
    judge(eq11, eq12);
}

void test3()
{
    string eq21 = "(ab)*";
    string eq22 = "E|a(ba)*b";
    judge(eq21, eq22);
    judge(eq22, eq21);
    judge(eq22, eq22);
}

void solve()
{
    int T;
    cin >> T;
    string r,s;
    while(T--)
    {
        cin >> r >> s;
        judge(r, s);
    }

}

int main()
{
    freopen("in.txt", "r", stdin);
    show_level = 0;
    solve();

    // test1();
    // test2();
    // test3();

    // show_level = 1;
    // string s = "hf*a(u?g+i|fo*ay+oahf*a(u?g+i|fo*ay+oa(hf*a(u?g+i|fo*ay+oaw*)eh?s+eabgo*(ia+f?ggzby*ga)+w?o((a|b)*abb(a|b)*a*a|b)?(aff(a|b)*abb(a|b)*a*a|bff)*afh*)*)eh?s+e(hf*a(u?g+i|fo*ay+oahf*a(u?g+i|fo*ay+oa(hf*a(u?g+i|fo*ay+oaw*)eh?s+eabgo*(ia+f?ggzby*ga)+w?o((a|b)*abb(a|b)*a*a|b)?(aff(a|b)*abb(a|b)*a*a|bff)*afh*)*)eh?s+ehf*a(u?g+i|fo*ay+oahf*a(u?g+i|fo*ay+oa(hf*a(u?g+i|fo*ay+oaw*)eh?s+eabgo*(ia+f?ggzby*ga)+w?o((a|b)*abb(a|b)*a*a|b)?(aff(a|b)*abb(hf*a(u?g+i|fo*ay+oahf*a(u?g+i|fo*ay+oa(hf*a(u?g+i|fo*ay+oaw*)eh?s+eabgo*(ia+f?ggzby*ga)+w?o((a|b)*abb(a|b)*a*a|b)?(aff(a|b)*abb(a|b)*a*a|bff)*afh*)*)eh?s+eabgo*(ia+f?ggzby*ga)+w?oafh*w*)eh?s+eabgo*(ia+f?ggzby*ga)+w?oafh*|b)*a*a|bff)*afh*)*)eh?s+eabgo*(ia+f?ggzby*ga)+w?oafh*w*)eh?s+eabgo*(ia+f?ggzby*ga)+w?oafh*bgo*(ia+f?ggzby*ga)+w?oafh*w*)eh?s+eabgo*(ia+f?ggzby*ga)+w?oafh*)?bgo*(ia+f?ggzby*ga)+w?oafh*w*)eh?s+eabgo*(ia+f?ggzby*ga)+w?oafh*";
    // translate(s);


    // translate("(a|b)*a*a|b");
    // judge("(a|b)*abb","(a|b)*a?ab");
    // judge("(a|b)*a?ab","(a|b)*abb");
}
