#include <iostream>
#include <map>
#include <cstring>
#include <cstdio>

char ioChar;

class Screen {
    private:
        const int numLines = 2;
        const int lineLength = 10;
        char charArray[20];

    public:
        Screen() : charArray{} {}

        void clear() {
            memset(charArray, 0, sizeof(charArray));
        }

        void loadBuffer(std::map<int, char> _charMap) {
            for (auto const& [key, val] : _charMap) {
                charArray[key] = val;
            }
        }

        void printLine(int lineNum = 0) {
            std::cout << '[';
            for (int i = 0; i < lineLength; i++) {
                if (charArray[(lineNum*lineLength) + i] != '\000') {
                    std::cout << charArray[(lineNum*lineLength) + i];
                } else {
                    std::cout << ' ';
                }
            }
            std::cout << ']' << std::endl;
        }

        void printToConsole() {
            for (int j = 0; j < numLines; j++) {
                printLine(j);
            }
            std::cout << "--------------------" << std::endl;
        }
};

class Element {
    private:
        bool visible = true;
        bool interactive;
        std::map<int, char> charMap;
    public:
        Element() : charMap{} {}
        Element(std::map<int, char> _charMap) : charMap(_charMap) {}
        Element(std::string text, int lineNum, int colNum, bool interactive = false) : interactive(interactive), charMap{} {
            setCharMap(text, lineNum, colNum);
        }
        Element(std::string text, int numChars, int lineNum, int colNum, bool interactive = false) 
            : interactive(interactive)
            , charMap{} 
            {
            setCharMap(text, numChars, lineNum, colNum);
        }

        void hide() {
            visible = false;
        }

        void show() {
            visible = true;
        }

        void toggleVisibility() {
            visible = !visible;
        }

        bool getInteractive() {
            return interactive;
        }

        void clear() {
            for (auto const& [key, val] : charMap) {
                charMap[key] = 0;
            }
        }

        void setText(std::string text) {
            // assign the text into charMap in order
            // one by one for each address,
            // WITHOUT CHANGING THE ADDRESSES!!!

            // charMap[address] = text[i]
            clear(); // clear the element before setting text
            int i = 0;
            for (auto const& [key, val] : charMap) {
                if (text[i] == '\000') {
                    break;
                } else {
                    charMap[key] = text[i++];
                }
            }
        }

        std::map<int, char> getCharMap() {
            return charMap;
        }

        void setCharMap(std::map<int, char>& _charMap) {
            charMap = _charMap;
        }

        // generates the char map from a string and a start index
        void setCharMap(std::string text, int lineNum, int colNum) {
            int startIndex = (lineNum - 1) * 10 + colNum;
            for (int i = 0; i < sizeof(text); i++) {
                if ((startIndex + i < 20) && text[i] != '\000') {
                    charMap[startIndex + i] = text[i];
                } else {
                    break;
                }
            }
        }

        // generates the char map from a string and a start index
        void setCharMap(std::string text, int numChars, int lineNum, int colNum) {
            char buffer[numChars];
            int actualNumDigits;
            for (int i = 0; i < sizeof(text); i++) {
                actualNumDigits = i;
                if (text[i] == '\000') {
                    break;
                }
            }

            int startIndex = (lineNum - 1) * 10 + colNum;
            for (int i = 0; i < numChars; i++) {
                if ((startIndex + i < 20) && i < numChars) {
                    charMap[startIndex + i] = text[i - (numChars - actualNumDigits)];
                } else {
                    break;
                }
            }
        }

        void render(Screen& s) {
            if (visible) {
                s.loadBuffer(charMap);
            }
        }
};

class NumericElement : public Element {
    private:
        int value;
    public:
        // Element(std::string text, int lineNum, int colNum, bool interactive = false) : interactive(interactive), charMap{} {
        //     setCharMap(text, lineNum, colNum);
        // }
        NumericElement(int lineNum, int colNum, bool interactive = false, int value = 0, int digits = 2)
            : Element{std::to_string(value), digits, lineNum, colNum, interactive}
            , value(value) {}

        int getValue() {
            return value;
        }

        void setValue(int _v) {
            value = _v;
            setText(std::to_string(value));
        }

        void incValue() {
            setValue(++value);
        }

        void decValue() {
            setValue(--value);
        }
};

class BooleanElement : public Element {
    private:
        bool value;
        std::string trueText;
        std::string falseText;

    public:
        BooleanElement(bool value, std::string trueText, std::string falseText, int lineNum, int colNum, bool interactive)
            : Element{value ? trueText : falseText, lineNum, colNum, interactive}
            , value(value)
            , trueText(trueText)
            , falseText(falseText) {}

        bool getValue() {
            return value;
        }

        void setValue(bool _v) {
            value = _v;
            setText(_v ? trueText : falseText);
        }

        void toggle() {
            setValue(!value);
        }
};

class Page {
    private:
        int numElements;
        Element* elements[10];
        int activeEl; // index for currently active element

        // make sure there are always two elements for next/previous page resp.
    public:
        Page() : numElements(0), elements{}, activeEl(-1) {}

        void addElement(Element& e) {
            elements[numElements++] = &e;
        }

        void nextActive() {
            int ogVal = activeEl;
            if (activeEl > 0) {
                do {
                    if (activeEl < numElements - 1) {
                        activeEl++;
                    } else if (activeEl + 1 == ogVal) {
                        break;
                    } else {
                        activeEl = 0;
                    }
                } while (elements[activeEl]->getInteractive() == false);
            }

            std::cout << activeEl << std::endl;
        }

        void prevActive() {
            int ogVal = activeEl;
            do {
                if (activeEl > 0) {
                    activeEl--;
                } else if (activeEl - 1 == ogVal) {
                    break;
                } else {
                    activeEl = numElements - 1;
                }
            } while (elements[activeEl]->getInteractive() == false);

            std::cout << activeEl << std::endl;
        }

        void render(Screen& s) {
            s.clear();
            for (int i = 0; i < numElements; i++) {
                if (i == activeEl) {
                    elements[i]->hide();
                } else {
                    elements[i]->show();
                }
                elements[i]->render(s);
            }
        }
};

class CtrlPanel {
    private:
        Screen screen;
        Page *pages[];
    public:
        CtrlPanel(Screen _screen, Page *_pages[])
            : screen{}
            , pages{*_pages}
            {
                // construct
            }
};

int main() {
    Element Tmea_label = Element("Tmea", 1, 3);
    NumericElement Tmea_value = NumericElement(1, 7, false, 6);
    Element Ttar_label = Element("Ttar:", 2, 2);
    NumericElement Ttar_value = NumericElement(2, 7, true, 270, 3);
    Element tempBack = Element("<-", 1, 0, true);
    Element tempNext = Element("->", 1, 8, true);
    BooleanElement yesOrNo = BooleanElement(true, "Y", "N", 2, 0, true);

    Page myPage = Page();
    myPage.addElement(Tmea_label);    
    myPage.addElement(Tmea_value);
    myPage.addElement(Ttar_label);
    myPage.addElement(Ttar_value);
    myPage.addElement(tempBack);
    myPage.addElement(tempNext);
    myPage.addElement(yesOrNo);

    // Element myEl3 = Element();
    // myEl3.setCharMap("Start:", 0);

    // Element myEl4 = Element();
    // myEl4.setCharMap("End:", 10);

    // Page myPage2 = Page();
    // myPage2.addElement(myEl3);
    // myPage2.addElement(myEl4);

    Screen myScreen = Screen();
    myPage.render(myScreen);
    myScreen.printToConsole();

    // yesOrNo.toggle();
    // myPage.render(myScreen);
    // myScreen.printToConsole();

    // Tmea_label.setText("54");
    // myTemp.decValue();
    // myPage.render(myScreen);
    // myScreen.printToConsole();

    // myPage2.render(myScreen);
    // myScreen.printToConsole();

    while (1) {
        std::cin >> ioChar;

        
        if (ioChar == 'w') {
            std::cout << "Increase value" << std::endl;
            Ttar_value.incValue();
        } else if (ioChar == 'a') {
            std::cout << "Navigate left" << std::endl;
            myPage.prevActive();

        } else if (ioChar == 's') {
            std::cout << "Decrease value" << std::endl;
            Ttar_value.decValue();

        } else if (ioChar == 'd') {
            std::cout << "Navigate right" << std::endl;
            myPage.nextActive();

        } else if (ioChar == 'e') {
            std::cout << "Select" << std::endl;
            yesOrNo.toggle();
            // myEl.toggleVisibility();
            // myPage.render(myScreen);
            // myScreen.printToConsole();
        }

        myPage.render(myScreen);
        myScreen.printToConsole();

    }
}