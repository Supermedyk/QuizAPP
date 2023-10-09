#pragma once
#include <string>
class Answer
{
private:
    std::wstring _value;
    bool _isPositive;

public:
    Answer(const std::wstring &value = L"",const bool &isPositive = false);
    bool getPositivity();
    std::wstring getValue();
};

