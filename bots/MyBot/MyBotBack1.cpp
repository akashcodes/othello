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

// GLOBAL var to represent player's bot
Turn ME;


/**
 * Using STATIC HEURISTIC for evaluation of a move
 * Matrix STATIC_HEURISTIC has entries for heuristic values
 * for each cell of the board.
 */


// ID BLACK = 1
// ID RED = 2


#define C 4    // CORNER        - SAFE HAVEN. Scored a corner
#define E 2     // EDGE          - STABLE. Got an edge.
#define S 1     // SAFE          - NORMAL. Bleh.
#define O 0      // NEUTRAL       - Neutral Move. Doesn't matters much
#define R -1    // RISKY         - Risky move. Get flanked and lose edge.
#define H -3    // HIGHLY RISKY  - Highly Risky. Getting flanked in next move
#define X -4   // EXTREME RISKY - Extremely Risky. Getting flanked in next move and losing the corner.

double STATIC_HEURISTIC[8][8] = {
    { C , H , E , E , E , E , H , C },
    { H , X , R , R , R , R , X , H },
    { E , R , S , O , O , S , R , E },
    { E , R , O , S , S , O , R , E },
    { E , R , O , S , S , O , R , E },
    { E , R , S , O , O , S , R , E },
    { H , X , R , R , R , R , X , H },
    { C , H , E , E , E , E , H , C },
};

float HEURISTIC_WEIGHTS[4] = {
    0.6,  // HEURISTIC TABLE
    0.3,  // COIN PARITY
    0.05, // STABILITY
    0.05  // MOBILITY
};


double computeWeightage(double values[]) {
    double weight = 0;
    for(int i = 0; i < 4; i++) 
        weight += values[i]*HEURISTIC_WEIGHTS[i];
    return weight;
}



double coinParity(OthelloBoard board) {
    // 1 -> Black
    // 2 -> Red
    int mycoins, oppcoins;
    if(ME == 1) { 
        mycoins = board.getBlackCount();
        oppcoins = board.getRedCount();
    } else {
        mycoins = board.getRedCount();
        oppcoins = board.getBlackCount();
    }

    double retval = 100*(mycoins-oppcoins)/(mycoins+oppcoins);
    return 100*(mycoins-oppcoins)/(mycoins+oppcoins);
}


double mobility(OthelloBoard board, Turn turn) {
    list<Move> moves = board.getValidMoves(turn);
    if(turn == ME)
        return moves.size();
    else
        return -moves.size();
}


double statich(Turn turn, Move move) {
    double sval = STATIC_HEURISTIC[move.x][move.y];
    if(ME == turn)
        return sval;
    else
        return -sval;
}



double evaluationFunction(OthelloBoard board, Turn turn, Move move) {
    double sval = STATIC_HEURISTIC[move.x][move.y];
    double cpar = coinParity(board);
    double mob = mobility(board, turn);
    double vals[4] = {sval, cpar, mob, 0};
    return computeWeightage(vals);
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
    
    //if(level&1) ret *= -1;
    
    if(!(newMoves.size())) return ret;
	for(;iter!=newMoves.end();iter++) {
		double curr = AlphaBeta(board,*iter,turn,level+1,alpha,beta);
		if(ME == turn) {
            ret = max(ret,curr);
			alpha = max(alpha,ret);	
		}
		else {
			ret = min(ret,curr);
			beta = min(beta,ret);	
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



// initializations go here
MyBot::MyBot( Turn turn ) : OthelloPlayer( turn ) {
    ME = turn;
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


