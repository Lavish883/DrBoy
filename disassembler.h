#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include <iostream>
#include <sstream>
#include "util.h"

uint8 opcode_disassembler(uint8 opcode, std::ostringstream& name);

#endif