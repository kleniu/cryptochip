#include "dmMenu.h"

dmMenu::dmMenu() {

}

dmMenu::~dmMenu() {
}

void dmMenu::init(Serial *ser) {
    uint8_t i=0, j=0;
    pc = ser;
    curItems=0; // on startup none menu items has been added to the menu
    for( j=0; j<DMMENU_SIZE; j++)
        for( i=0; i<DMMENU_ITEM_TXTSIZE; i++ )
            items[j].text[i]=0;
}

void dmMenu::setHeader(const char *text) {
    uint8_t i=0;
    for( i=0; text[i]!=0 && i<DMMENU_ITEM_TXTSIZE-1; i++ )
        menuHeader[i] = text[i];
    menuHeader[DMMENU_ITEM_TXTSIZE-1]=0;
}

void dmMenu::display() {
    uint8_t i, len;
    char selectedItem;
    bool itemFound;
    uint8_t itemIndex;
    bool displayMenu = true;

    while (true) {
        pc->printf("\n\r");
        if (displayMenu) {
            len = getStrLength(menuHeader);
            for (i=0; i<len; i++) pc->printf("*");
            pc->printf("\n\r");

            pc->printf("%s\n\r",menuHeader);
            pc->printf("press 0 or hit ENTER for menu\n\r");

            for (i=0; i<len; i++) pc->printf("*");
            pc->printf("\n\r");

            for (i=0; i<curItems; i++) {
                pc->printf(" %c - %s\n\r", items[i].index, items[i].text);
            }
        }

        pc->printf("> ");
        selectedItem = readPrompt();
        if (selectedItem == '0') {
            displayMenu = true;
        }
        else {
            displayMenu = false;
            i = 0;
            itemIndex = 0; itemFound = false;
            while (i<DMMENU_SIZE) {
                if (items[i].index == selectedItem) {
                    itemFound = true;
                    itemIndex = i;
                    i = DMMENU_SIZE;
                }
                i++;
            }
            if (itemFound) {
                pc->printf("%c \n\r",items[itemIndex].index);
                items[itemIndex].itemFunction();
            }
            else {
                pc->printf("Unrecognized menu item : %c - Try again.", selectedItem);
            }
        }
    }
}



void dmMenu::addItem(char index, const char *text, void (*itemFunction)() ) {
    if( curItems < DMMENU_SIZE ) {
        items[curItems].index        = index;
        strCopy( text, items[curItems].text );
        items[curItems].itemFunction = itemFunction;
        curItems++;
    }
}

int8_t dmMenu::getDigit(const char *promptMsg) {
    int8_t num = -1;

    pc->printf(promptMsg);

    while ( num < 0 || num > 9 )
        num = readPrompt() - 0x30;

    pc->printf("%d\n\r", num);
    return num;
}

bool dmMenu::getBool(const char *promptMsg, char trueChar ) {
    bool retVal = false;
    uint8_t buf[1] = { 0 };

    pc->printf("%s", promptMsg);
    buf[0] = pc->getc();
    if( buf[0] == trueChar )
        retVal = true;
    else
        retVal = false;

    // flush all other chars
    while( pc->readable() > 0 ) pc->getc();

    if ( retVal ) pc->printf("true\n\r");
    else          pc->printf("false\n\r");

    return retVal;
}

int16_t dmMenu::getString(const char *promtMsg, uint8_t *buf, uint16_t buflen) {
  int16_t i = 0;
  int8_t ch = 0;

  pc->printf("%s", promtMsg);
  while( i<buflen-1 && ch!='\n' && ch!='\r' ) {
    ch = pc->getc();
    //printf("\n\r%03d",ch);
    if( ( ch >= '0' && ch <= '9' ) ||
        ( ch >= 'a' && ch <= 'z' ) ||
        ( ch >= 'A' && ch <= 'Z' ) ||
        ( ch == ' ' || ch == '-' || ch == '_' || ch == '/' || ch == '+' || ch == '=' )) {
      pc->printf("%c", ch);
      buf[i++] = ch;
    }
  }
  if( i!=0 ) buf[i]=0x00;
  pc->printf("\n\r");
  return i;
}

///////////////////////////////////////////////////////////////////////////////
// private methods
uint8_t dmMenu::readPrompt() {
    uint8_t retChar = 0;
    while (!pc->readable()) {
        wait_ms(100);
    }

    retChar = pc->getc();

    while( pc->readable() > 0 ) pc->getc();

    if( !isDigit(retChar) && !isAlpha(retChar) ) {
        retChar = '0';
    }
    return retChar;
}


uint8_t dmMenu::isDigit(uint8_t ch) {
        return (ch >= '0') && (ch <= '9');
}

uint8_t dmMenu::isAlpha(uint8_t ch) {
  return ((ch >= 'a') && (ch <= 'z')) || ((ch >= 'A') && (ch <= 'Z'));
}

uint8_t dmMenu::getStrLength(char *text) {
    uint8_t i=0;
    while ( text[i]!=0 && i<DMMENU_ITEM_TXTSIZE-1)
        i++;
    return i;
}

uint8_t dmMenu::strCopy(const char *src, char *des) {
    uint8_t i=0;
    for( i=0; src[i]!=0 && i<DMMENU_ITEM_TXTSIZE-1; i++ )
        des[i] = src[i];
    des[i-1]=0;
    return i;
}
