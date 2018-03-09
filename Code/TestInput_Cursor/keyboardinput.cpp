#include "keyboardinput.h"
#include "inputmethod.h"


QInputContext *KeyboardInput::allocaInputContext()
{
    return new InputMethod;
}
