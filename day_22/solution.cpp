#include <algorithm>
#include <deque>
#include <regex>
#include <fstream>
#include <unordered_set>
#include <memory>
#include <iostream>

using std::deque;
using std::istream;
using std::ostream;
using std::unordered_set;

using Card = int;

class Player;
namespace std {
    template<>
    struct hash<Player> {
        std::size_t operator()(const Player& player) const noexcept;
    };
}

class Player {
public:
    Player() {}

    Player(const Player& player, size_t size) : id{player.id}
    {
        std::copy_n(player.deck.begin(), size, std::back_inserter(deck));
    }

    int get_id() const { return id; }

    bool empty() const { return deck.empty(); }

    int size() const { return deck.size(); }

    Card draw_top()
    {
        auto card = deck.front();
        deck.pop_front();
        return card;
    }

    void push_bottom(Card card) { deck.push_back(card); }

    friend bool operator==(const Player& p1, const Player& p2)
    {
        return p1.id == p2.id && p1.deck == p2.deck;
    }

    friend istream& operator>>(istream& is, Player& player);
    friend ostream& operator<<(ostream& os, const Player& player);
    friend size_t std::hash<Player>::operator()(const Player&) const noexcept;

private:
    deque<Card> deck;
    int id;
};

namespace std {
    std::size_t hash<Player>::operator()(const Player& player) const noexcept
    {
        std::size_t seed = player.get_id() ^ player.size();
        for (auto i : player.deck) {
            seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }

    template<>
    struct hash<std::pair<Player, Player>> {
        std::size_t operator()(const std::pair<Player, Player>& players) const noexcept
        {
            auto h = std::hash<Player>{};
            return h(players.first) ^ h(players.second);
        }
    };
};

istream& operator>>(istream& is, Player& player)
{
    is >> std::ws;
    std::smatch matches;
    std::string line;

    if (getline(is, line)) {
        if (!std::regex_match(line, matches, std::regex("^Player ([[:digit:]]+):$"))) {
            is.clear(std::ios::failbit);
            return is;
        }
        player.id = std::stoi(matches[1].str());
    }

    int card;
    while (is >> card) {
        player.deck.push_back(card);
    }
    is.clear();

    return is;
}

ostream& operator<<(ostream& os, const Player& player)
{
    os << "Player " << player.id << ":" << std::endl;
    for (auto c : player.deck) {
        os << c << std::endl;
    }
    return os;
}

int combat(Player p1, Player p2)
{
    while (!(p1.empty() || p2.empty())) {
        Card p1_card = p1.draw_top();
        Card p2_card = p2.draw_top();

        if (p1_card > p2_card) {
            p1.push_bottom(p1_card);
            p1.push_bottom(p2_card);
        } else if (p2_card > p1_card) {
            p2.push_bottom(p2_card);
            p2.push_bottom(p1_card);
        } else {
            p1.push_bottom(p1_card);
            p2.push_bottom(p2_card);
        }
    }

    Player& winner = p1.empty() ? p2 : p1;

    int score = 0;
    while (!winner.empty()) {
        int multiplier = winner.size();
        score += multiplier * winner.draw_top();
    }

    return score;
}

Player recursive_combat_game(
    std::pair<Player, Player>& players
) {
    unordered_set<std::pair<Player, Player>> prev_rounds;
    while (true) {
        if (prev_rounds.find(players) != prev_rounds.end()) {
            return players.first;
        }
        // new configuration
        prev_rounds.insert(players);

        Card c1 = players.first.draw_top();
        Card c2 = players.second.draw_top();

        int winner = -1;

        if (players.first.size() >= c1 && players.second.size() >= c2) {
            // play new game of recursive combat
            std::pair<Player, Player> rec_players = {
                Player(players.first, c1),
                Player(players.second, c2)
            };
            const auto& rec_winner = recursive_combat_game(rec_players);
            if (rec_winner == rec_players.first) {
                winner = 0;
            } else {
                winner = 1;
            }
        } else if (c1 > c2) {
            winner = 0;
        } else if (c2 < c1) {
            winner = 1;
        }

        if (winner == 0) {
            players.first.push_bottom(c1);
            players.first.push_bottom(c2);
        } else {
            players.second.push_bottom(c2);
            players.second.push_bottom(c1);
        }
        if (players.first.empty()) {
            return players.second;
        }
        if (players.second.empty()) {
            return players.first;
        }
    }
}

int recursive_combat(std::pair<Player, Player> players)
{
    const auto& rec_winner = recursive_combat_game(players);
    Player& winner = (
        rec_winner.get_id() == players.first.get_id()
        ? players.first
        : players.second
    );

    int score = 0;
    while (!winner.empty()) {
        int multiplier = winner.size();
        score += multiplier * winner.draw_top();
    }

    return score;
}

int main()
{
    std::ifstream data{"input.txt"};

    Player p1;
    data >> p1;

    Player p2;
    data >> p2;

    // Part 1
    std::cout << combat(p1, p2) << std::endl;

    // Part 2
    std::pair<Player, Player> players = {p1, p2};
    std::cout << recursive_combat(players) << std::endl;

}
