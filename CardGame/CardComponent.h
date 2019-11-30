#pragma once
struct CardComponent : IComponent
{
    Card card;

    CardComponent(Card card) : card(card) {}
};
