/*
* @file botTemplate.cpp
* @author Arun Tejasvi Chaganty <arunchaganty@gmail.com>
* @date 2010-02-04 
* Template for users to create their own bots
* Modified by Akash Mishra (CS19M008)
* and Sarath Chandra Bussa (CS19M057)
*/

#include "Othello.h"
#include "OthelloBoard.h"
#include "OthelloPlayer.h"
#include <cstdlib>
using namespace std;
using namespace Desdemona;
#include <unistd.h>


// Store the board here
OthelloBoard gboard;


/**
 * Using STATIC HEURISTIC for evaluation of a move
 * Matrix STATIC_HEURISTIC has entries for heuristic values
 * for each cell of the board.
 */


// ID BLACK = 1
// ID RED = 2


#define C 100    // CORNER        - SAFE HAVEN. Scored a corner
#define E 50     // EDGE          - STABLE. Got an edge.
#define S 25     // SAFE          - NORMAL. Bleh.
#define O 0      // NEUTRAL       - Neutral Move. Doesn't matters much
#define R -25    // RISKY         - Risky move. Get flanked and lose edge.
#define H -75    // HIGHLY RISKY  - Highly Risky. Getting flanked in next move
#define X -100   // EXTREME RISKY - Extremely Risky. Getting flanked in next move and losing the corner.


int STATIC_HEURISTIC[8][8] = {
    { C , H , E , E , E , E , H , C },
    { H , X , R , R , R , R , X , H },
    { E , R , S , O , O , S , R , E },
    { E , R , O , S , S , O , R , E },
    { E , R , O , S , S , O , R , E },
    { E , R , S , O , O , S , R , E },
    { H , X , R , R , R , R , X , H },
    { C , H , E , E , E , E , H , C },
};



/**
 * TODO :
 * SUCCESSOR FUNCTION -  to generate next moves
 * i.e. generate the game tree.
 * Defined for all layers in the tree;
 */


const int MAX_DEPTH = 4;



list<Move>::iterator MAX(list<Move> moves) {
    list<Move>::iterator it, bestit;
    int h = -1000;
    int x, y;

    //if(moves.size() < 1) cout<<"No moves left\n";
    it = moves.begin();
    bestit = it;

    for (; it != moves.end(); it++) {
        x = it->x;
        y = it->y;
        //cout<<x<<"  "<<y<<"\n";
        if(STATIC_HEURISTIC[x][y] > h) {
            h = STATIC_HEURISTIC[x][y];
            *bestit = *it;
        }
    }
    //cout<<b<<" plays move => "<<bestit->x<<" "<<bestit->y<<"\n";

    return bestit;
}


list<Move>::iterator MIN(list<Move> moves) {
    list<Move>::iterator it, bestit;
    int h = 1000;
    int x, y;

    //if(moves.size() < 1) cout<<"No moves left\n";
    it = moves.begin();
    bestit = it;

    for (; it != moves.end(); it++) {
        x = it->x;
        y = it->y;
        //cout<<x<<"  "<<y<<"\n";
        if(STATIC_HEURISTIC[x][y] < h) {
            h = STATIC_HEURISTIC[x][y];
            *bestit = *it;
        }
    }
    //cout<<b<<" plays move => "<<bestit->x<<" "<<bestit->y<<"\n";

    return bestit;
}



Move MiniMax(const OthelloBoard& board, int depth, Turn turn) {
    OthelloBoard cboard;
    cboard =  board;
    Turn next_turn;
    if(turn == BLACK)
        next_turn = RED;
    else if (turn == RED) {
        next_turn = BLACK;
    }

    list<Move> moves = cboard.getValidMoves( turn );

    list<Move> node_moves;

    if(depth == MAX_DEPTH) {

        return 

    }

    return 
}



class MyBot: public OthelloPlayer {
    public:
        /**
         * Initialisation routines here
         * This could do anything from open up a cache of "best moves" to
         * spawning a background processing thread. 
         */
        // constructor
        int b = rand()%7;
        MyBot( Turn turn );

        /**
         * Play something 
         */
        virtual Move play( const OthelloBoard& board );
    private:
};




MyBot::MyBot( Turn turn )
    : OthelloPlayer( turn ) {
}




Move MyBot::play( const OthelloBoard& board ) {

    // get a list of valid moves the bot can take

    gboard = board;
    usleep(1000000);
    printf("\033c");
    cout<<"It's this guy's turn = > "<<turn<<"\n";
    gboard.print();

    list<Move> moves = board.getValidMoves( turn );

    //cout<<"Piece 1 = > "<<board.get(3, 3)<<"\n";
    //cout<<"Piece 2 = > "<<board.get(3, 4)<<"\n";

    //int randNo = rand() % moves.size();
    
    list<Move>::iterator it, bestit;
    int h = -120;
    int x, y;

    //if(moves.size() < 1) cout<<"No moves left\n";
    it = moves.begin();
    bestit = it;

    for (; it != moves.end(); it++) {
        x = it->x;
        y = it->y;
        //cout<<x<<"  "<<y<<"\n";
        if(STATIC_HEURISTIC[x][y] > h) {
            h = STATIC_HEURISTIC[x][y];
            *bestit = *it;
        }
    }
    //cout<<b<<" plays move => "<<bestit->x<<" "<<bestit->y<<"\n";

    return *bestit;
}




// The following lines are _very_ important to create a bot module for Desdemona

extern "C" {
    OthelloPlayer* createBot( Turn turn ) {
        return new MyBot( turn );
    }

    void destroyBot( OthelloPlayer* bot ) {
        delete bot;
    }
}


