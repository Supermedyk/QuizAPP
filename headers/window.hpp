#pragma once
#include "question.hpp"
#include "answer.hpp"
#include "font.hpp"
#include <cmath>
#include <string>
#include <vector>
#include <fstream>
#include <codecvt>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <filesystem>
class Window
{
private:
    sf::Font _font;
    sf::RenderWindow _window;
    sf::Text _text;
    std::vector<Question> _questions;
    std::fstream file;
    std::string myString;
    std::wstring dataFile;
    std::wstring specialUtf8String;
    int wantToChoose;
    int amountOfQuestions;
    bool isFine;
    void loadInfoFile();
    void loadFileFromDisk(const std::wstring &filePath);
    void closeFile();
    void loadAmountOfQuestions();
    void loadQuestions();
    void loadFiles();
    void shuffle_questions();
    void drawCounter(const int &index);
    void lastQuestion(const int &index, const int &amountOfGoodAnswers);
    void drawAnswers(const int &index);
    void drawResult(const int &amountOfGoodAnswers);
    bool checkIfGoodAnswer(const int &index);
    void drawQuestion(const int &index);
    const std::string grade(const int &percentage);
    int convertWideStringToInteger(const std::wstring& value);
    std::wstring widen (const std::string& utf8_string);

public:
    Window();
    void mainLoop();
};
