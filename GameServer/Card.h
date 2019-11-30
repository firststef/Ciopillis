#pragma once
#include <vector>
#include <string>

using CardID = unsigned int;

enum CardType
{
    ATTACK = 0,
    DEFENCE = 1,
    VERSATILE = 2
};

struct Card
{
    std::string name;
    std::string path;
    std::string description;
    std::vector<std::string> functionality;
    CardType type;

    CardID id;

    Card() = default;
    Card(std::string name);
};

struct CardContainer {
    std::vector<Card> cards;
};

enum Element
{
    None,
    Water,
    Earth,
    Light,
    Energy,
    Force,
    Matter,
    Wind,
    Plant,
    Fire,
    Life,
    Information,
    Arcanum,
    Chaos,
    Aether,
    Gaia
};