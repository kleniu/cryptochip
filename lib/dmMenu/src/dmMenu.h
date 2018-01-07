#ifndef DMMENU_H
#define DMMENU_H

#include <mbed.h>

#ifndef DMMENU_SIZE
#define DMMENU_SIZE 10
#endif

#ifndef DMMENU_ITEM_TXTSIZE
#define DMMENU_ITEM_TXTSIZE 30
#endif

struct Item {
    char text[DMMENU_ITEM_TXTSIZE];
    char index;
    void (*itemFunction)();
};

class dmMenu {
    public:
        dmMenu();
        ~dmMenu();
        // public methods
        void init(Serial *ser);
        // set header
        void setHeader(const char *text);
        void addItem(char index, const char *text, void (*itemFunction)() );
        void display();
        int8_t getNumber(const char *promptMsg);
        bool getBool(const char *promptMsg, char trueChar );
    private:
        Serial *pc;
        uint8_t isDigit(uint8_t ch);
        uint8_t getStrLength(char *text);
        uint8_t strCopy(const char *src, char *des);
        uint8_t readPrompt();
        Item items[DMMENU_SIZE];
        char menuHeader[DMMENU_ITEM_TXTSIZE];
        uint8_t curItems;
};

#endif
