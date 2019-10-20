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

#define INF 1e18


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


double coinParity(OthelloBoard board, Turn turn) {
    // 1 -> Black
    // 2 -> Red
    int mycoins, oppcoins;
    if(turn == 1) { 
        mycoins = board.getBlackCount();
        oppcoins = board.getRedCount();
    } else {
        mycoins = board.getRedCount();
        oppcoins = board.getBlackCount();
    }
    return 100*(mycoins-oppcoins)/(mycoins+oppcoins);
}



double evaluationFunction(OthelloBoard board, Turn turn, Move move) {
    double sval = STATIC_HEURISTIC[move.x][move.y];
    double cpar = coinParity(board, turn);
    return sval+cpar;
}



/**
 * TODO :
 * SUCCESSOR FUNCTION -  to generate next moves
 * i.e. generate the game tree.
 * Defined for all layers in the tree;
 */


const int MAX_LEVEL = 6;


double AlphaBeta(OthelloBoard board, Move move, Turn turn, short level, double alpha, double beta) {


    if(level == MAX_LEVEL) {
		return evaluationFunction(board, turn, move);
	}


    board.makeMove(turn,move);
	turn = other(turn);
	list<Move> newMoves = board.getValidMoves(turn);
	list<Move>::iterator iter = newMoves.begin();
	double ret = -INF;
    if(level&1) ret *= -1;
    if(!(newMoves.size())) return ret;
	for(;iter!=newMoves.end();iter++) {
		double curr = AlphaBeta(board,*iter,turn,level+1,alpha,beta);
		if(level&1) {
			ret = min(ret,curr);
			beta = min(beta,ret);
		}
		else {
			ret = max(ret,curr);
			alpha = max(alpha,ret);		
		}
		if(beta<=alpha) break;
	}
	return ret; 
}



class MyBot: public OthelloPlayer {
    public:
        /**
         * Initialisation routines here
         * This could do anything from open up a cache of "best moves" to
         * spawning a background processing thread. 
         */
        // constructor
        //int b = rand()%7;
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
    //usleep(1000000);
    //printf("\033c");
    //cout<<"It's this guy's turn = > "<<turn<<"\n";
    //gboard.print();

    list<Move> moves = board.getValidMoves( turn );

    list<Move>::iterator it = moves.begin();
    Move bestMove((*it).x,(*it).y);
    double retVal = -INF;
    double MAX = INF, MIN = -INF;
    OthelloBoard copyBoard = board;
    short level = 1;
    for(;it!=moves.end();it++) {
    	double currValue = AlphaBeta(copyBoard,*it,turn,level,MIN,MAX);
    	if(currValue > retVal) {
    		retVal = currValue;
    		bestMove = *it;
    	}
    	copyBoard = board;
    }
    return bestMove;
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


