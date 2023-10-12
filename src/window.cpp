#include "../headers/window.hpp"
std::wstring Window::widen (const std::string& utf8_string)
{
    std::wstring_convert <std::codecvt_utf8 <wchar_t>, wchar_t> convert;
    return convert.from_bytes (utf8_string);
}
Window::Window()
{
    _window.create({1280,800},"QuizApp",sf::Style::Titlebar|sf::Style::Close);
    _window.setFramerateLimit(60);
    _font.loadFromMemory(imageData,sizeof(imageData));
    _text.setFont(_font);
    isFine = true;
    wantToChoose = 160000;
}
void Window::loadFileFromDisk( const std::wstring &filePath)
{
    file.open(std::filesystem::path(filePath));
    if (!file.good())
        throw widen("Cannot open ")+filePath;
}
void Window::closeFile()
{
    file.close();
}
int Window::convertWideStringToInteger(const std::wstring& value)
{
    if (value.length() == 0)
        return 0;
    int result = 0;
    for (auto i: value)
    {
        if (i >= L'0' && i <= L'9')
        {
            result *= 10;
            result += i-L'0';
        }
    }
    return result;
}
void Window::loadInfoFile()
{
    loadFileFromDisk(widen("Config\\info.txt"));
    getline(file,myString);
    amountOfQuestions = convertWideStringToInteger(widen(myString).substr(6,myString.length()));
    if (amountOfQuestions <= 0 || amountOfQuestions > 1000)
    {
        closeFile();
        throw widen("Wrong amount of questions!");
    }
    getline(file,myString);
    dataFile = widen(myString);
    dataFile = dataFile.substr(14);
    while(dataFile.length() != 0 && *dataFile.begin() == L' ')
    {
        dataFile.erase(dataFile.begin());
    }

    dataFile += L".txt";
    closeFile();
    if (dataFile.length() == 0)
        throw widen("Data file doesn't exists!");
}
void Window::loadQuestions()
{
    loadFileFromDisk(widen("Config\\") + dataFile);
    std::vector<std::wstring> valueBuff;
    std::wstring helpVariable;
    std::vector<Answer> answersBuff;
    bool positiveBuff = false;
    for ( int i = 0; i < amountOfQuestions; i++)
    {
        valueBuff.clear();
        answersBuff.clear();
        for ( int j = 0; j < 2; j++)
        {
            getline(file,myString);
            if  ((myString.length() == 0)|| (myString.length() == 1 && myString[0] != 'x' && myString[0] != 'X'   ))
            {
                closeFile();
                throw widen("Question uncorrectly defined!");
            }

            if (!(myString.length() == 1 && (myString[0] == 'x' || myString[0] == 'X')))
            {
                valueBuff.push_back(widen(myString));
            }
        }
        for (int j = 0; j < 5; j++)
        {
            myString = "";
            positiveBuff = false;
            getline(file,myString);
            if (myString.length() <= 1)
            {
                closeFile();
                throw widen("Answer too short!");
            }
            if (myString[0] != '+' && myString[0] != '-')
            {
                closeFile();
                throw widen("Can't decide if answer is positive or not!");
            }
            if (myString[0] == '+')
                positiveBuff = true;
            myString.erase(myString.begin());
            answersBuff.push_back({widen(myString),positiveBuff});
        }
        _questions.push_back({answersBuff,valueBuff});
    }
    closeFile();
}
void Window::loadFiles()
{
    try
    {
        loadInfoFile();
        loadQuestions();
    }
    catch (const std::wstring &error)
    {
        isFine = false;
        _text.setString(error);
    }
}
void Window::shuffle_questions()
{
    for (auto &i: _questions)
    {
        i.doShuffle();
    }
    std::random_shuffle(_questions.begin(),_questions.end());
}
void Window::drawIntro()
{
    if (dataFile.length() == 0)
        throw widen("Wrong question name!");
    _text.setString(dataFile.substr(0,dataFile.length()-4));
    _text.setCharacterSize(140);
    sf::FloatRect buff = _text.getLocalBounds();
    _text.setOrigin(buff.left+buff.width/2.0f,buff.top+buff.height/2.0f);
    _text.setPosition(640,350);
    _window.draw(_text);
    _text.setString(widen("Kliknij spację by kontynuować!"));
    _text.setCharacterSize(70);
    _text.setPosition({640,700});
    buff = _text.getLocalBounds();
    _text.setOrigin(buff.left+buff.width/2.0f,buff.top+buff.height/2.0f);
    _window.draw(_text);
};
void Window::drawCounter(const int &index, const int &amountOfGoodAnswers)
{
    _text.setString(std::to_string(index+1)+"/"+std::to_string(amountOfQuestions));
    _text.setCharacterSize(60);
    sf::FloatRect buff = _text.getLocalBounds();
    _text.setOrigin(buff.left+buff.width/2.0f,buff.top+buff.height/2.0f);
    _text.setPosition({1190,750});
    _window.draw(_text);
    _text.setFillColor(sf::Color::Green);
    _text.setString(std::string("Dobre: ")+std::to_string(amountOfGoodAnswers));
    _text.setCharacterSize(30);
    _text.setOrigin({0,0});
    _text.setPosition({0,710});
    _window.draw(_text);
    _text.setFillColor(sf::Color::Red);
    _text.setString(widen("Złe: ")+widen(std::to_string(index-amountOfGoodAnswers)));
    _text.setCharacterSize(30);
    _text.setOrigin({0,0});
    _text.setPosition({0,750});
    _window.draw(_text);
    _text.setFillColor(sf::Color::White);
}
void Window::lastQuestion(const int& index)
{
    if (checkIfGoodAnswer(index))
    {
        _text.setFillColor(sf::Color::Green);
        _text.setString(L"Dobra odpowiedź!");
    }
    else
    {
        _text.setFillColor(sf::Color::Red);
        _text.setString(L"Zła odpowiedź!");
    }
    _text.setCharacterSize(120);
    sf::FloatRect buff = _text.getLocalBounds();
    _text.setOrigin(buff.left+buff.width/2.0f,buff.top+buff.height/2.0f);
    _text.setPosition({640,360});
    _window.draw(_text);
    _text.setFillColor(sf::Color::White);
}
bool Window::checkIfGoodAnswer(const int &index)
{
    return _questions[index].getAnswers()[wantToChoose].getPositivity();
}
void Window::drawAnswers(const int &index)
{
    if (index >= _questions.size())
    {
        throw("Tried to read question that doesn't exists!");
    }
    float yRect = 100.0f;
    sf::FloatRect b;
    sf::RectangleShape rectangle({1280,120});
    rectangle.setPosition({0,yRect});
    rectangle.setOutlineColor({55,55,55});
    rectangle.setOutlineThickness(5.f);
    std::vector<Answer> buff = _questions[index].getAnswers();
    for (size_t i = 0; i < buff.size(); i++)
    {
        if (i == wantToChoose)
        {
            rectangle.setFillColor(sf::Color(175,205,64));
        }
        else
        {
            rectangle.setFillColor({40,40,40});
        }
        rectangle.setPosition({0,yRect});
        _text.setString(buff[i].getValue());
        _text.setCharacterSize(32);
        sf::FloatRect b = _text.getLocalBounds();
        _text.setOrigin(b.left+b.width/2.0f,b.top+b.height/2.0f);
        _text.setPosition(rectangle.getPosition()+rectangle.getSize()/2.f);
        _window.draw(rectangle);
        _window.draw(_text);
        yRect += 120;
    }
}
const std::pair<sf::Color,std::string> Window::grade(const int &percentage)
{
    switch(percentage)
    {
    case 35 ... 49:
        return {sf::Color(255,165,0),"2"};
    case 50 ... 69:
        return {sf::Color::Yellow,"3"};
    case 70 ... 89:
        return {sf::Color(	144, 238, 144),"4"};
    case 90 ... 100:
        return {sf::Color::Green,"5"};
    }
    return {sf::Color::Red,"1"};
}
void Window::drawResult(const int &amountOfGoodAnswers)
{
    sfe::RichText text(_font);
    int percentage = round((static_cast<double>(amountOfGoodAnswers)/static_cast<double>(amountOfQuestions))*100.0);
    text.setCharacterSize(50);
    text.setPosition(640,100);
    //text.setString(L"Ilość pytań: " + widen(std::to_string(amountOfQuestions)));
    text << sf::Color::White << L"Ilość pytań: " + widen(std::to_string(amountOfQuestions));
    sf::FloatRect buff = text.getLocalBounds();
    text.setOrigin(buff.left+buff.width/2.0f,buff.top+buff.height/2.0f);
    _window.draw(text);
    text.setPosition(640,200);
   // text.setString(L"Ilość dobrych odpowiedzi: " + widen(std::to_string(amountOfGoodAnswers)) + widen(" (" + std::to_string(percentage) + "%)") );
    text.clear();
    text << sf::Color::White<<L"Ilość dobrych odpowiedzi: " << sf::Color::Green <<  widen(std::to_string(amountOfGoodAnswers)) << sf::Color::White << " (" + std::to_string(percentage) + "%)";
    buff = text.getLocalBounds();
    text.setOrigin(buff.left+buff.width/2.0f,buff.top+buff.height/2.0f);
    _window.draw(text);
    text.setPosition(640,300);
    text.clear();
//    text.setString(L"Ilość złych odpowiedzi: " + widen(std::to_string(amountOfQuestions-amountOfGoodAnswers)+ " (" + std::to_string(100-percentage) + "%)"   ));
     text << sf::Color::White<<L"Ilość złych odpowiedzi: " << sf::Color::Red <<  widen(std::to_string(amountOfQuestions-amountOfGoodAnswers)) << sf::Color::White << " (" + std::to_string(100-percentage) + "%)";
    buff = text.getLocalBounds();
    text.setOrigin(buff.left+buff.width/2.0f,buff.top+buff.height/2.0f);
    _window.draw(text);
    text.clear();
    text.setCharacterSize(100);
    text.setPosition(640,550);
    text << sf::Color::White << "Proponowana ocena: " << (grade(percentage)).first <<(grade(percentage)).second;
    buff = text.getLocalBounds();
    text.setOrigin(buff.left+buff.width/2.0f,buff.top+buff.height/2.0f);
    _window.draw(text);
}
void Window::drawQuestion(const int &index)
{
    if (index >= _questions.size())
    {
        throw("Tried to read question that doesn't exists!");
    }
    size_t y = 10;
    for (auto i: _questions[index].getValue())
    {
        _text.setString(i);
        _text.setCharacterSize(40);
        sf::FloatRect buff = _text.getLocalBounds();
        _text.setOrigin(buff.left+buff.width/2.0f,buff.top+buff.height/2.0f);
        _text.setPosition(640,buff.height/2.0f+y);
        y += 40;
        _window.draw(_text);
    }

}
void Window::mainLoop()
{
    loadFiles();
    shuffle_questions();
    int currentIndex = 0;
    int amountOfGoodAnswers = 0;
    wantToChoose = 0;
    bool upClickW = false;
    bool downClickS = false;
    bool upClickArrow = false;
    bool downClickArrow = false;
    bool enterClick = false;
    bool showAnswer = false;
    bool spaceClick = false;
    bool showIntro = true;
    while(_window.isOpen())
    {
        sf::Event event;
        while(_window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                _window.close();
            if (isFine)
            {
                if ((((sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) && enterClick == false) || ((sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) && spaceClick == false))&& (currentIndex != amountOfQuestions || showAnswer) )
                {
                    if (showIntro)
                    {
                        showIntro = false;
                    }
                    else if (showAnswer)
                    {
                        showAnswer = false;
                        wantToChoose = 0;
                        currentIndex++;
                    }
                    else
                    {
                        if (checkIfGoodAnswer(currentIndex))
                            amountOfGoodAnswers++;
                        showAnswer = true;
                    }
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
                    {
                        enterClick = true;
                    }
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
                    {
                        spaceClick = true;
                    }
                }
                if (!showAnswer && !showIntro)
                {
                    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W)) && upClickW == false)
                    {
                        wantToChoose--;
                        upClickW = true;
                    }
                    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) && upClickArrow == false)
                    {
                        wantToChoose--;
                        upClickArrow = true;
                    }
                    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::S)) && downClickS == false)
                    {
                        wantToChoose++;
                        downClickS = true;
                    }
                    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) && downClickArrow == false)
                    {
                        wantToChoose++;
                        downClickArrow = true;
                    }
                }
                if ((!sf::Keyboard::isKeyPressed(sf::Keyboard::S)) && downClickS == true)
                    downClickS = false;
                if ((!sf::Keyboard::isKeyPressed(sf::Keyboard::W)) && upClickW == true)
                    upClickW = false;
                if ((!sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) && downClickArrow == true)
                    downClickArrow = false;
                if ((!sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) && upClickArrow == true)
                    upClickArrow = false;
                if ((!sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) && enterClick == true)
                    enterClick = false;
                if ((!sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) && spaceClick == true)
                    spaceClick = false;

                if (wantToChoose < 0)
                    wantToChoose = 4;
                if (wantToChoose >= 5)
                    wantToChoose = 0;

            }

        }
        _window.clear(sf::Color(30,30,30));
        if (isFine)
        {
            if (showIntro)
            {
                try
                {
                    drawIntro();
                }
                catch(const std::wstring &error)
                {
                     isFine = false;
                    _text.setString(error);
                }
            }
            else if (showAnswer)
            {
                lastQuestion(currentIndex);
            }
            else if (currentIndex != amountOfQuestions)
            {
                try
                {
                    drawQuestion(currentIndex);
                    drawAnswers(currentIndex);
                    drawCounter(currentIndex,amountOfGoodAnswers);
                }
                catch(const std::wstring &error)
                {
                    isFine = false;
                    _text.setString(error);
                }
            }
            else
            {
                drawResult(amountOfGoodAnswers);
            }
        }
        else
        {
            _window.draw(_text);
        }
        _window.display();
    }
}

