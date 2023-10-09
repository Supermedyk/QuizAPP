#include "../headers/answer.hpp"

Answer::Answer(const std::wstring& value, const bool& isPositive) : _value(value), _isPositive(isPositive)
{}

bool Answer::getPositivity()
{
    return _isPositive;
}
std::wstring Answer::getValue()
{
    return _value;
}
