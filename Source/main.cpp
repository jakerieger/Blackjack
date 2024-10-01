// Author: Jake Rieger
// Created: 10/01/24.
//

#define ALL_ALIASES
#define INC_VECTOR
#define INC_OPTION
#define INC_DICTIONARY
#define CUSTOM_CONTAINERS
#include <Types.h>
#include <assert.h>
#include <iostream>
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

    [[nodiscard]] u32 getValue(u32 aceValue = 11) const {
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
                return aceValue;
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

enum GameState {
    START,
    ACTIVE,
    BUST,
    WIN,
    QUIT,
};

int main() {
    Deck deck;
    deck.shuffle();

    GameState state = START;
    bool running    = true;
    u32 player      = 0;
    u32 dealer      = 0;

    while (running) {
        switch (state) {
            case START: {
                auto player1 = deck.deal();
                assert(player1.has_value());
                auto dealer1 = deck.deal();
                assert(dealer1.has_value());
                auto player2 = deck.deal();
                assert(player2.has_value());
                auto dealer2 = deck.deal();
                assert(dealer2.has_value());

                player = player1->getValue() + player2->getValue();
                dealer = dealer1->getValue() + dealer2->getValue();

                state = ACTIVE;
            } break;
            case ACTIVE: {
            } break;
            case BUST: {
                state = QUIT;
            } break;
            case WIN: {
                state = QUIT;
            } break;
            case QUIT: {
                running = false;
            } break;
        }

        // Update game state in TUI
        std::cout << "PLAYER: " << player << '\r' << std::flush;
        // std::cout << "DEALER: " << dealer << '\r' << std::flush;
    }

    return 0;
}