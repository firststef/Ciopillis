#include "CardGenerator.h"
#include "Card.h"
#include "json.hpp"
#include <fstream>

using json = nlohmann::json;

CardGenerator::CardGenerator(std::string path)
{
    container = GetDatabase(path);
}
CardGenerator::~CardGenerator() = default;

void from_json(const json& j, Card& card) {
    const std::string aux = j.at("name");
    card.name = aux;
}

void from_json(const json& j, CardContainer& cont) {
    const json& sj = j.at("cards");
    cont.cards.resize(sj.size());
    std::copy(sj.begin(), sj.end(), cont.cards.begin());
}

CardContainer CardGenerator::GetDatabase(std::string path)
{
    std::ifstream i(path);
    json j;
    i >> j;
    CardContainer database = j;
    return database;
}
