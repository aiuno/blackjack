#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define DECK_COUNT      8
#define BLACKJACK_MULTI 1.5

// TODO: Raylib

typedef enum {
    SUIT_HEARTS = 0,
    SUIT_DIAMONDS,
    SUIT_CLUBS,
    SUIT_SPADES,
} suit_s;

typedef enum {
    HIT = 0,
    STAND
} dealer_actions_s;

typedef struct {
    suit_s suit;
    uint8_t num;
} card_t;

void shuffle(card_t *cards) {
    for (int i = 0; i < 52 * DECK_COUNT; ++i) {
        int count = 0;
        uint32_t suit = arc4random() % 4;
        uint32_t num = arc4random() % 13 + 1;

        for (int j = 0; j < 52 * DECK_COUNT; ++j) {
            if (cards[j].num == 0) break;
            if (cards[j].suit == suit && cards[j].num == num) count += 1;
        }

        if (count < DECK_COUNT) {
            cards[i].suit = suit;
            cards[i].num = num;
        } else {
            i -= 1;
        }
    }
}

uint32_t calc_total(card_t *cards) {
    uint32_t total = 0;
    int ace_count = 0;
    for (int i = 0; i < 52 * DECK_COUNT; ++i) {
        if (cards[i].num == 0) break;
        if (cards[i].num > 1 && cards[i].num < 11) {
            total += cards[i].num;
        } else if (cards[i].num > 10) {
            total += 10;
        } else {
            ace_count += 1;
        }
    }

    for (int i = 0; i < ace_count; ++i) {
        if (total + 11 > 21) {
            total += 1;
        } else {
            total += 11;
        }
    }

    return total;
}

void deal(card_t *cards, card_t *hand, int *deck_pos, int *hand_pos) {
    hand[*hand_pos] = cards[*deck_pos];
    *deck_pos += 1;
    *hand_pos += 1;
}

void deal_single(card_t *cards, card_t *hand, int *deck_pos) {
    *hand = cards[*deck_pos];
    *deck_pos += 1;
}

// These are the rules blackjack dealers follow
// If dealer cards is (ace + [card]) >= 17, they must stand and cannot continue to draw
dealer_actions_s dealer_think(card_t *dealer_hand) {
    if (calc_total(dealer_hand) < 17) return HIT;
    else return STAND;
}

const char *get_card_name(uint8_t num) {
    static char buf[256];

    switch (num) {
        case 1:
            strcpy(buf, "ace");
            break;
        case 11:
            strcpy(buf, "jack");
            break;
        case 12:
            strcpy(buf, "queen");
            break;
        case 13:
            strcpy(buf, "king");
            break;
        default:
            sprintf(buf, "%d", num);
            break;
    }

    return buf;
}

const char *get_suit(suit_s suit) {
    switch (suit) {
        case SUIT_HEARTS:
            return "hearts";
        case SUIT_DIAMONDS:
            return "diamonds";
        case SUIT_CLUBS:
            return "clubs";
        case SUIT_SPADES:
            return "spades";
    }
}

int main() {
    int deck_pos = 0;
    int player_pos = 0;
    int dealer_pos = 0;

    card_t cards[52 * DECK_COUNT] = {0};
    card_t player_hand[52 * DECK_COUNT] = {0};
    card_t dealer_hand[52 * DECK_COUNT] = {0};
    card_t dealer_reserve = {0};
    shuffle(cards);

    bool playing = true;
    bool player_standing = false;
    bool dealer_standing = false;
    bool restart = true;

    while (playing) {
        if (deck_pos > (52 * DECK_COUNT) - ((52 * DECK_COUNT) / 3)) {
            deck_pos = 0;
            shuffle(cards);
        }

        if (calc_total(player_hand) > 21) {
            printf("Player bust, you lose!\n");
            restart = true;
        }

        if (calc_total(dealer_hand) > 21) {
            printf("Dealer bust, you win!\n");
            restart = true;
        }

        if (player_standing && dealer_standing) {
            if (calc_total(player_hand) == calc_total(dealer_hand)) {
                printf("Result: Push\n");
                restart = true;
            }
            if (calc_total(player_hand) < calc_total(dealer_hand)) {
                printf("Result: Lose\n");
                restart = true;
            }
            if (calc_total(player_hand) > calc_total(dealer_hand)) {
                printf("Result: Win\n");
                restart = true;
            }
        }

        if (restart) {
            printf("\n\n");

            player_standing = false;
            dealer_standing = false;

            for (int i = 0; i < player_pos; ++i) {
                player_hand[i] = (card_t) {
                        .num = 0,
                        .suit = 0
                };
            }

            for (int i = 0; i < dealer_pos; ++i) {
                dealer_hand[i] = (card_t) {
                        .num = 0,
                        .suit = 0
                };
            }

            player_pos = 0;
            dealer_pos = 0;

            deal(cards, player_hand, &deck_pos, &player_pos);
            deal(cards, dealer_hand, &deck_pos, &dealer_pos);
            deal(cards, player_hand, &deck_pos, &player_pos);
            deal_single(cards, &dealer_reserve, &deck_pos);

            for (int i = 0; i < 52 * DECK_COUNT; ++i) {
                if (dealer_hand[i].num == 0) break;
                printf("Dealer card: %s of %s\n", get_card_name(dealer_hand[i].num), get_suit(dealer_hand[i].suit));
            }
            printf("Dealer card: *hidden*\n");
            printf("Dealer total: %d\n", calc_total(dealer_hand));

            for (int i = 0; i < 52 * DECK_COUNT; ++i) {
                if (player_hand[i].num == 0) break;
                printf("Player card: %s of %s\n", get_card_name(player_hand[i].num), get_suit(player_hand[i].suit));
            }
            printf("Player total: %d\n", calc_total(player_hand));

            restart = false;
        }

        if (!player_standing) {
            printf("Hit (h), stand (s) or quit (q)? ");
            switch (getchar()) {
                case 'h':
                    deal(cards, player_hand, &deck_pos, &player_pos);
                    printf("Player drew: %s of %s\n", get_card_name(player_hand[player_pos - 1].num),
                           get_suit(player_hand[player_pos - 1].suit));
                    printf("Player total: %d\n", calc_total(player_hand));
                    if (calc_total(player_hand) == 21) player_standing = true; // Force stand on 21
                    break;
                case 's':
                    player_standing = true;

                    dealer_hand[dealer_pos] = dealer_reserve;
                    dealer_pos += 1;

                    for (int i = 0; i < 52 * DECK_COUNT; ++i) {
                        if (dealer_hand[i].num == 0) break;
                        printf("Dealer card: %s of %s\n", get_card_name(dealer_hand[i].num), get_suit(dealer_hand[i].suit));
                    }
                    break;
                case 'q':
                    playing = false;
                    break;
            }
            getchar(); // Consume newline char i guess
        } else {
            if (dealer_think(dealer_hand) == HIT) {
                deal(cards, dealer_hand, &deck_pos, &dealer_pos);
                printf("Dealer drew: %s of %s\n", get_card_name(dealer_hand[dealer_pos - 1].num),
                       get_suit(dealer_hand[dealer_pos - 1].suit));
                printf("Dealer total: %d\n", calc_total(dealer_hand));
            } else {
                dealer_standing = true;
            }
        }
    }

    return 0;
}
