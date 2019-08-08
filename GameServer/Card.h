#pragma once
#include <vector>
#include <memory>

using CardID = unsigned int;

struct Card
{
    std::string name;
    std::string description;
    std::vector<std::string> functionality;

    CardID id;

    Card() = default;
    Card(std::string name);
};

struct CardContainer {
    std::vector<Card> cards;
};