#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include "answer.hpp"
class Question
{
private:
    std::vector<std::wstring> _value;
    std::vector<Answer> _answers;
public:
    Question(const std::vector<Answer> &answers, const std::vector<std::wstring> &value);
    std::vector<Answer> getAnswers();
    std::vector<std::wstring> getValue();
    void doShuffle();
};
