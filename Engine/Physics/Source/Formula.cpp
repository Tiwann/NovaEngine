#pragma once
#include "Formula.h"

unsigned int Factorial(unsigned int Number)
{
    if (Number == 0)
        return 1;

    return Number * Factorial(Number - 1);
}