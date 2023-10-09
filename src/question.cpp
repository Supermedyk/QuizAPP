#include "../headers/question.hpp"
Question::Question(const std::vector<Answer> &answers, const std::vector<std::wstring> &value) : _value(value), _answers(answers)
{

}
std::vector<Answer> Question::getAnswers()
{
    return _answers;
}
std::vector<std::wstring> Question::getValue()
{
    return _value;
}
void Question::doShuffle()
{
    std::random_shuffle(_answers.begin(),_answers.end());
}

