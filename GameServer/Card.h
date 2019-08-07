#pragma once
#include <vector>
#include <memory>

class Card
{
public:
    std::string name;

    Card() = default;
    Card(std::string name);
};

struct CardContainer {
    std::vector<Card> cards;
};