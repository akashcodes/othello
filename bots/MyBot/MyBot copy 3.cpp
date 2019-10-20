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

// Global variables represent corners coordinates
int CORNERS[4][2] = {
    {0, 0},
    {0, 7},
    {7, 0},
    {7, 7}
};


// variable represents
int NEIGHBOURS[8][2] = {
    {0, 1},
    {1, 0},
    {1, 1},
    {0, -1},
    {-1, 0},
    {-1, 1},
    {1, -1},
    {-1, -1}
};


/**
 * Using dynamic + STATIC HEURISTIC for evaluation of a move
 * Matrix STATIC_HEURISTIC has entries for heuristic values
 * for each cell of the board.
 */


// ID BLACK = 1
// ID RED = 2


#define C 10    // CORNER        - SAFE HAVEN. Scored a corner
#define E 5     // EDGE          - STABLE. Got an edge.
#define S 2     // SAFE          - NORMAL. Bleh.
#define O 0      // NEUTRAL       - Neutral Move. Doesn't matters much
#define R -2    // RISKY         - Risky move. Get flanked and lose edge.
#define H -7    // HIGHLY RISKY  - Highly Risky. Getting flanked in next move
#define X -10   // EXTREME RISKY - Extremely Risky. Getting flanked in next move and losing the corner.

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


float HEURISTIC_WEIGHTS[6] = {
    11,  // Parity
    850.724,  // Corner
    382.026, // Closeness
    86.922, // mobility
    78.396,  // frontier
    1000   // static
};


double computeWeightage(double values[]) {
    double weight = 0;
    for(int i = 0; i < 6; i++) 
        weight += values[i]*HEURISTIC_WEIGHTS[i];
    return weight;
}



double coinParity(OthelloBoard board, Turn turn) {
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
    if((mycoins + oppcoins) == 0) return 0;
    return 100 * (mycoins - oppcoins) / (mycoins + oppcoins);
}


double mobility(OthelloBoard board, Turn turn) {
    list<Move> moves = board.getValidMoves(turn);
    return moves.size();
}


double frontier(OthelloBoard board) {
    int x, y;
    int mfront = 0, ofront = 0;
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            if(board.get(i, j) != 0)   {
                for(int k = 0; k < 8; k++)  {

                    x = i + NEIGHBOURS[k][0];
                    y = j + NEIGHBOURS[k][1];
                    if(x >= 0 && x < 8 && y >= 0 && y < 8 && board.get(x, y) == 0) {
                        if(board.get(i, j) == ME)  mfront++;
                        else ofront++;
                        break;
                    }
                }
            }
        }
    }
    if((ofront +  mfront) == 0) return 0;
    return 100 * (ofront - mfront) / (ofront + mfront);
}


double statich(OthelloBoard board, Turn turn, Move move) {
    double stat=0;
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            if(board.get(i, j) == turn) {
                stat += STATIC_HEURISTIC[i][j];
            }
        }
    }
    
    return stat;
}



double evaluationFunction(OthelloBoard board, Turn turn, Move move) {
    board.makeMove(turn, move);

    // parity
    double parity = coinParity(board, turn);
    double corner = 0;
    double close = 0;
    double mob = 0;//mobility(board, turn);
    double front = frontier(board);//mobility(board, turn);
    double stat = STATIC_HEURISTIC[move.x][move.y];
    

    double vals[6] = {parity, corner, close, mob, front, stat};
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
    
    if(ME == turn) ret *= -1;
    
    if(!(newMoves.size())) return ret;

    OthelloBoard cboard;

	for(;iter!=newMoves.end();iter++) {
		cboard = board;
        double curr = AlphaBeta(cboard,*iter,turn,level+1,alpha,beta);
		
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

    list<Move> moves = board.getValidMoves( turn );

    list<Move>::iterator it = moves.begin();
    Move bestMove((*it).x,(*it).y);
    
    double retVal = -INF;
    double MAX = INF, MIN = -INF;
    
    OthelloBoard cboard = board;

    short level = 1;
    
    for(;it!=moves.end();it++) {
    	cboard = board;
        double currValue = AlphaBeta(cboard,*it,turn,level,MIN,MAX);
    	
        if(currValue > retVal) {
    		retVal = currValue;
    		bestMove = *it;
    	}

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


