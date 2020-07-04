#include "RE2NFA.hpp"
#include "NFA2DFA.hpp"
#include "MINDFA.hpp"

const bool show = false;
void test(string re)
{
    if(show) cout << "************************************ TEST ************************************" << endl;

    if(show) cout << "************************************ RE2NFA ************************************" << endl;
    NFAPtr nfa = RE2NFA::translate(re);
    if(show)  nfa -> display();

    if(show) cout << "************************************ NFA2DFA ************************************" << endl;
    DFAPtr dfa = NFA2DFA::translate(nfa);
    if(show)  dfa -> display();

    if(show) cout << "************************************ MINDFA ************************************" << endl;
    DFAPtr mindfa = MINDFA::minimize(dfa);
    if(show) mindfa -> display();
    
    if(show) cout << endl;
}

int main()
{
    test("(a|b)*abb");
    test("(a|b)*a?ab");
    test("(a|b)*|a+ab");
    test("(a|b)*a*a|b");
}