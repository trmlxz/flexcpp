#include "dfa.ih"

void DFA::determineAccCount(AccCount &accCount, size_t thisRow, size_t fmRow, 
                            size_t fmCount, DFA &dfa)
{
    if (dfa.d_rule != accCount.rule())
        return;

cout << "   initialize accCount in row " << thisRow << ", coming from " <<
fmRow << ", previous AccCount value: " << fmCount << '\n';

    if (not dfa.inspectAccCount(accCount, fmCount))
{
cout << "No action for this accCount: done here\n";
        return;
}

cout << "accCount in row " << thisRow << " for rule " << dfa.d_rule << ": " <<
accCount.accCount() << '\n';

    std::unordered_map<size_t, size_t> const &transitMap = 
                                                    dfa.d_row[thisRow].map();

    for_each(transitMap.begin(), transitMap.end(), 
        FnWrap::unary(transitAccCount, thisRow, accCount.accCount(), dfa));
}