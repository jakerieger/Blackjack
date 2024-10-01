// Author: Jake Rieger
// Created: 10/01/24.
//

#define ALL_ALIASES
#define INC_VECTOR
#define INC_OPTION
#define INC_DICTIONARY
#define CUSTOM_CONTAINERS
#include <Types.h>
#include <iostream>
#include <numeric>
#include <random>

enum Rank {
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    TEN,
    JACK,
    QUEEN,
    KING,
    ACE,
    NUM_RANKS,
};

static const Dictionary<u8, str> RankNames = {
  {TWO, "Two"},
  {THREE, "Three"},
  {FOUR, "Four"},
  {FIVE, "Five"},
  {SIX, "Six"},
  {SEVEN, "Seven"},
  {EIGHT, "Eight"},
  {NINE, "Nine"},
  {TEN, "Ten"},
  {JACK, "Jack"},
  {QUEEN, "Queen"},
  {KING, "King"},
  {ACE, "Ace"},
};

enum Suit {
    CLUBS,
    DIAMONDS,
    HEARTS,
    SPADES,
    NUM_SUITS,
};

static const Dictionary<u8, str> SuitNames = {
  {CLUBS, "Clubs"},
  {DIAMONDS, "Diamonds"},
  {HEARTS, "Hearts"},
  {SPADES, "Spades"},
};

struct Card {
    u8 rank;
    u8 suit;

    Card(u8 rank, u8 suit) : rank(rank), suit(suit) {}

    [[nodiscard]] u32 getValue() const {
        switch (rank) {
            case TWO:
            case THREE:
            case FOUR:
            case FIVE:
            case SIX:
            case SEVEN:
            case EIGHT:
            case NINE:
                return (u32)rank + 2;
            case TEN:
            case JACK:
            case QUEEN:
            case KING:
                return 10;
            case ACE:
                return 11;
            default:
                return 0;
        }
    }

    [[nodiscard]] u16 getBits() const {
        return CAST<u16>(rank) << 8 | suit;
    }

    static u8 getRankFromBits(u16 bits) {
        return CAST<u8>(bits >> 8) & 0xFF;
    }

    static u8 getSuitFromBits(u16 bits) {
        return CAST<u8>(bits) & 0xFF;
    }

    void printBits() const {
        std::cout << std::setfill('0') << std::setw(4) << std::hex << getBits() << '\n';
    }

    [[nodiscard]] str toString() const {
        std::ostringstream oss;
        oss << RankNames.find(rank)->second;
        oss << " of ";
        oss << SuitNames.find(suit)->second;
        return oss.str();
    }
};

class Deck {
public:
    Vector<Card> cards;

    Deck() {
        cards.reserve(52);
        for (auto rank = 0; rank < NUM_RANKS; ++rank) {
            for (auto suit = 0; suit < NUM_SUITS; ++suit) {
                Card card(CAST<u8>(rank) & 0xFF, CAST<u8>(suit) & 0xFF);
                cards.push_back(card);
            }
        }
    }

    void shuffle(i32 iterations = 10) {
        for (auto i = 0; i < iterations; i++) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::ranges::shuffle(cards, gen);
        }
    }

    Option<Card> deal() {
        if (cards.empty())
            return std::nullopt;

        auto c = cards.back();
        cards.pop_back();
        return c;
    }

    void print() const {
        std::cout << "DECK\n";
        std::cout << "Size: " << cards.size() << "\n\n";

        for (auto& card : cards) {
            std::cout << "[ " << card.toString() << " ]\n";
            std::cout << card.getValue() << '\n';
        }
    }
};

class Player {
public:
    Vector<Card> cards;
    bool hasAce = false;
    Player()    = default;

    void addCard(const Card& card) {
        cards.push_back(card);
        if (card.rank == ACE) {
            hasAce = true;
        }
    }

    [[nodiscard]] u32 getHandValue() const {
        u32 value =
          std::accumulate(cards.begin(), cards.end(), 0, [&](u32 total, const Card& card) {
              return total + card.getValue();
          });

        if (value > 21 && hasAce)
            value -= 10;

        return value;
    }

    [[nodiscard]] bool isBust() const {
        return getHandValue() > 21;
    }

    [[nodiscard]] bool isBlackjack() const {
        return (cards.size() == 2) && (getHandValue() == 21);
    }
};

class Blackjack {
public:
    Deck deck;
    Player player;
    Player dealer;

    Blackjack() {
        deck.shuffle();

        player.addCard(deck.deal().value());
        player.addCard(deck.deal().value());

        dealer.addCard(deck.deal().value());
        dealer.addCard(deck.deal().value());
    }

    void playerTurn() {
        if (player.isBlackjack() && !dealer.isBlackjack()) {
            return;
        }

        char input;
        do {
            std::cout << "Your hand: " << player.getHandValue() << '\n';
            std::cout << "Hit (h) or Stand(s): ";
            std::cin >> input;
            if (input == 'h') {
                player.addCard(deck.deal().value());
            }
        } while (input == 'h' && !player.isBust());
    }

    void dealerTurn() {
        while (dealer.getHandValue() < 17) {
            dealer.addCard(deck.deal().value());
        }
    }

    void results() const {
        if (player.isBust()) {
            std::cout << "You busted! Dealer wins.\n";
        } else if (dealer.isBust()) {
            std::cout << "Dealer busted! You win.\n";
        } else if (player.getHandValue() > dealer.getHandValue()) {
            std::cout << "You won with: " << player.getHandValue() << '\n';
        } else if (player.getHandValue() == dealer.getHandValue()) {
            std::cout << "It's a tie!\n";
        } else {
            std::cout << "Dealer wins with: " << dealer.getHandValue() << '\n';
        }
    }
};

int main() {
    Blackjack bj;
    bj.playerTurn();
    if (!bj.player.isBust()) {
        bj.dealerTurn();
    }
    bj.results();

    return 0;
}