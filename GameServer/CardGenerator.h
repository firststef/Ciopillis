#pragma once
#include "Card.h"

class CardGenerator
{
public:
    CardContainer container;
    
    CardGenerator();
    CardGenerator(std::string path);
    ~CardGenerator();

    CardContainer GetDatabase(std::string path);
};

