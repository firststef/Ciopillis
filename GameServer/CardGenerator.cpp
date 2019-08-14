#include "CardGenerator.h"
#include "Card.h"
#include "json.hpp"
#include <fstream>

using json = nlohmann::json;

CardGenerator::CardGenerator(std::string path)
{
    container = GetDatabase(path);
}

void from_json(const json& j, Card& card) {
    const std::string aux1 = j.at("name");
    card.name = aux1;
    const std::string aux2 = j.at("description");
    card.description = aux2;

    const json& sj = j.at("functionality");
    card.functionality.resize(sj.size());
    std::copy(sj.begin(), sj.end(), card.functionality.begin());

    const int aux3 = j.at("type");
    card.type = CardType(aux3);
}

void from_json(const json& j, CardContainer& cont) {
    const json& sj = j.at("cards");
    cont.cards.resize(sj.size());
    std::copy(sj.begin(), sj.end(), cont.cards.begin());

    unsigned idx = 0;
    for (auto& card : cont.cards)
    {
        card.id = idx++;
        card.path = std::string("../cards/Card") + std::string(std::to_string(idx)) + std::string(".png");
    }
}

CardContainer CardGenerator::GetDatabase(std::string path)
{
    std::ifstream i(path);
    json j;
    i >> j;
    CardContainer database = j;
    return database;
}
