#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "raylib.h"
#include <string.h>

//===============================================================//
//              DONN WILSONS BLACKJACK!!                         //
//                  B37VB Game Project                           //
//                      :)                                       //
//===============================================================//


//========================================
//Making the cards
//========================================

//the rank names 
char *rankNames[] = {
    "", "A", "2", "3", "4", "5", "6",
     "7", "8", "9", "10", "J", "Q", "K"
};
//the suit names although they wont come up visually 
char *suitNames[] = {"Hearts", "Diamonds", "Spades", "Clubs"};
const char* suitSymbols[] = {"h", "d", "s", "c"};

//
//FUNCTIONS!!
//

//makes the diamonds and hearts red and the spades and clubs black
Color getSuitColor(int suit) {
    if (suit == 0 || suit == 1) return RED;
    return BLACK;
}

//makes a full deck (so that cards dont repeat and its not just making up random cards) (#realism)
void createDeck(int ranks[], int suits[], int values[]) {
    int index = 0;

    for (int suit = 0; suit < 4; suit++) {
        for (int rank = 1; rank <= 13; rank++) {

            ranks[index] = rank;
            suits[index] = suit;

            if (rank > 10) values[index] = 10;
            else if (rank == 1) values[index] = 11;
            else values[index] = rank;

            index++;
        }
    }
}

//shuffles the deck
void shuffleDeck(int ranks[], int suits[], int values[]) {
    for (int i = 0; i < 52; i++) {
        int j = rand() % 52;

        int temp;

        temp = ranks[i];
        ranks[i] = ranks[j];
        ranks[j] = temp;

        temp = suits[i];
        suits[i] = suits[j];
        suits[j] = temp;

        temp = values[i];
        values[i] = values[j];
        values[j] = temp;
    }
}
//calculates the total value of the hand you have 
//(it handles the aces correclty meaning its worth 11 unless itll make you go bust then its worth 1)
int calculateTotal(int cards[], int count, int values[], int ranks[]) {
    int total = 0;
    int aceCount = 0;

    for (int i = 0; i < count; i++) {
        total += values[cards[i]];
        if (ranks[cards[i]] == 1) aceCount++;
    }
    //this is the bit that makes the aces function correctly
    while (total > 21 && aceCount > 0) {
        total -= 10;
        aceCount--;
    }

    return total;
}
//====================================
//suit designs!!!! 
//======================================

//draws the heart (two circles with a rotated circle at the bottom)
void drawHeart(int x, int y, Color color) {
    DrawCircle(x + 12, y + 14, 12, color);
    DrawCircle(x + 28, y + 14, 12, color);
    
    //there was an issue with making triangles function so i used a rotated square
    Rectangle rect = { x + 20, y + 22, 24, 24 };

    DrawRectanglePro(rect, (Vector2){12, 12}, 45.0f, color);
}
//diamond is also a rotated square due to issues with triangles
void drawDiamond(int x, int y, Color color) {
    Rectangle rect = { x, y, 30, 30 };

    DrawRectanglePro(rect, (Vector2){15, 15}, 45.0f, color);
}
//this is the spade, the triangle for some reason worked with this one and only this oney
void drawSpade(int x, int y, Color color) {
   DrawTriangle(
    (Vector2){x + 15, y},
    (Vector2){x, y + 25},
    (Vector2){x + 30, y + 25},
    color
   );
   
   DrawCircle(x + 10, y + 25, 8, color);
   DrawCircle(x + 20, y + 25, 8, color);

   DrawRectangle(x + 13, y + 30, 4, 10, color);
}
//bunch of circles and a rectangle makes the club
void drawClub(int x, int y, Color color) {
    DrawCircle(x + 15, y + 10, 8, color);
    DrawCircle(x + 7, y + 20, 8, color);
    DrawCircle(x + 23, y + 20, 8, color);
    DrawCircle(x + 15, y + 20, 7, color);
    DrawRectangle(x + 12, y + 26, 6, 14, color);
}

//=====================================================
//DRAWING THE CARDS !!!
//=====================================================

//draws a visible card
void drawCard(int x, int y, const char *rank, int suit) {
    Color color = getSuitColor(suit);
    
    //this is the card background 
    DrawRectangleRounded((Rectangle){x, y, 80, 120}, 0.2, 6, RAYWHITE);
    DrawRectangleRoundedLines((Rectangle){x, y, 80, 120}, 0.2, 6, BLACK);

    //this is the rank that is at the top left of the card
    DrawText(rank, x + 5, y + 5, 18, color);

    //this is the big suit in the middle of the card
    switch (suit) {
        case 0: drawHeart(x + 20, y + 35, color); break;
        case 1: drawDiamond(x + 40, y + 60, color); break;
        case 2: drawSpade(x + 25, y + 35, color); break;
        case 3: drawClub(x + 25, y + 35, color); break;
    }
    //this is the rank thats in the bottom right
    DrawText(rank, x + 55, y + 95, 20, color);
}

//this is the face down card that the dealer has, all the #s make up the design on the back
void drawHiddenCard(int x, int y) {
    DrawRectangleRounded((Rectangle){x, y, 80, 120}, 0.2, 6, DARKBLUE);
    DrawRectangleRoundedLines((Rectangle){x, y, 80, 120}, 0.2, 6, BLACK);
    DrawText("#####\n#####\n#####\n#####\n#####\n", x + 7, y + 7, 20, WHITE);
}

//======================================
// THE MAIN GAME ! :D
//======================================


int main() {
    srand(time(NULL));

    InitWindow(800, 600, "Blackjack");
    SetTargetFPS(60);

    //data for the deck
    int ranks[52], suits[52], values[52];
    int deckIndex = 0;

    createDeck(ranks, suits, values);
    shuffleDeck(ranks, suits, values);
    //the players hand
    int playerCards[10];
    int playerCardCount = 2;
    playerCards[0] = deckIndex++;
    playerCards[1] = deckIndex++;

    //the dealers hand
    int dealerCards[10];
    int dealerCardCount = 2;
    dealerCards[0] = deckIndex++;
    dealerCards[1] = deckIndex++;

    int playerTotal = calculateTotal(playerCards, playerCardCount, values, ranks);

    //game states so that the game doesnt just go on forever with no end goal
    bool playerStand = false;
    bool dealerDone = false;
    bool gameOver = false;

    //==================================
    // the game loop....
    //=================================
    while (!WindowShouldClose()) {

        //updates the players and the dealers total every frame
        playerTotal = calculateTotal(playerCards, playerCardCount, values, ranks);
        int dealerTotal = calculateTotal(dealerCards, dealerCardCount, values, ranks);

        //mechjanics for if the player hits
        if (IsKeyPressed(KEY_H) && !playerStand && playerTotal <= 21 && playerCardCount < 10) {
            playerCards[playerCardCount++] = deckIndex++;
        }
        //mechanics for if the player stands
        if (IsKeyPressed(KEY_S)) {
            playerStand = true;
        }
        //this is the dealers play after you stand
        if (playerStand && !dealerDone) {
            while (dealerTotal < 17) {
                dealerCards[dealerCardCount++] = deckIndex++;
                dealerTotal = calculateTotal(dealerCards, dealerCardCount, values, ranks);
            }
            dealerDone = true;
        }
        //==============================================
        //drawing
        //==============================================
        BeginDrawing();

        //dark green background like the felt on an actual blackjack table
        ClearBackground(DARKGREEN);
        //title and controls
        DrawText("Donns Blackjack", 320, 20, 30, WHITE);
        DrawText("Press H to Hit or S to Stand", 250, 500, 20, WHITE);

        //displays the value of the hand the player has
        DrawText(TextFormat("%d", playerTotal), 120, 340, 30, WHITE);

        //draws the players card 
        for (int i = 0; i < playerCardCount; i++) {
            drawCard(200 + i * 100, 300,
                rankNames[ranks[playerCards[i]]],
                suits[playerCards[i]]);
        }
        //also draws the dealers cards
        if (!playerStand) {
            drawCard(200, 100,
                rankNames[ranks[dealerCards[0]]],
                suits[dealerCards[0]]);
            drawHiddenCard(300, 100);
        } else {
            for (int i = 0; i < dealerCardCount; i++) {
                drawCard(200 + i * 100, 100,
                    rankNames[ranks[dealerCards[i]]],
                    suits[dealerCards[i]]);
            }
        }
        //shows the dealers total (just shows a ? whenever the card is face down)
        if (!playerStand) {
            DrawText("?", 120, 140, 30, WHITE);
        } else {
            DrawText(TextFormat("%d", dealerTotal), 120, 140, 30, WHITE);
        }
        //lables for neatness
        DrawText("Dealer", 120, 110, 20, WHITE);
        DrawText("You", 120, 310, 20, WHITE);

        //the win / lose logicc
        if (playerTotal > 21) {
            DrawText("busted...", 250, 230, 30, RED);
            gameOver = true;
        }
        else if (playerStand && dealerDone) {
            gameOver = true;

            if (dealerTotal > 21) {
                DrawText("dealer busted... yay...", 250, 230, 30, GREEN);
            }
            else if (playerTotal > dealerTotal) {
                DrawText("you won and the dealer is MAD", 250, 230, 30, GREEN);
            }
            else if (playerTotal < dealerTotal) {
                DrawText("aw man the dealer wins", 250, 230, 30, RED);
            }
            else {
                DrawText("thats a draw baby", 250, 230, 30, WHITE);
            }
        }

        DrawText("Press R to Restart", 260, 550, 20, WHITE);

        EndDrawing();

        //lets you restart the game by pressing R after the game is done
        if (gameOver && IsKeyPressed(KEY_R)) {
            deckIndex = 0;
            shuffleDeck(ranks, suits, values);

            playerCardCount = 2;
            dealerCardCount = 2;

            playerCards[0] = deckIndex++;
            playerCards[1] = deckIndex++;

            dealerCards[0] = deckIndex++;
            dealerCards[1] = deckIndex++;

            playerStand = false;
            dealerDone = false;
            gameOver = false;
        }
    }

    CloseWindow();
    return 0;
}
//the end.......