//
// Created by anders on 4/17/17.
//

#ifndef CHESS_ANN_UCIEVENT_H
#define CHESS_ANN_UCIEVENT_H

#include <cstdint>
namespace uci {
namespace event {

const uint8_t TEST        = 0;
const uint8_t UCI         = 10;
const uint8_t DEBUG       = 20;
const uint8_t ISREADY     = 30;
const uint8_t SETOPTION   = 40;
const uint8_t REGISTER    = 50;
const uint8_t UCINEWGAME  = 60;
const uint8_t POSITION    = 70;
const uint8_t GO          = 80;
const uint8_t STOP        = 90;
const uint8_t PONDERHIT   = 100;
const uint8_t QUIT        = 110;

}
}

#endif //CHESS_ANN_UCIEVENT_H
