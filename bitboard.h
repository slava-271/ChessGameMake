#ifndef BITBOARD_H
#define BITBOARD_H
#include <cstdint>
#include <unordered_map>
#include <vector>


#define whiteFigures {0,(1ull << 8ull), (1ull << 9ull), (1ull << 10ull), (1ull << 11ull), (1ull << 12ull),(1ull << 13ull), (1ull << 14ull), (1ull << 15ull), 1ull, 2ull, 4ull, 8ull,16ull, 32ull, 64ull, 128ull}
#define blackFigures {0,(1ull << 55ull), (1ull << 54ull), (1ull << 53ull), (1ull << 52ull), (1ull << 51ull),(1ull << 50ull), (1ull << 49ull), (1ull << 48ull),   (1ull << 56ull), (1ull << 62ull),(1ull << 61ull), (1ull << 59ull), (1ull << 60ull),(1ull << 58ull), (1ull << 57ull), (1ull << 63ull)}



typedef enum {
    NONE, PAWN1, PAWN2, PAWN3, PAWN4, PAWN5, PAWN6, PAWN7,
    PAWN8, ROOK1, KNIGHT1, BISHOP1, QUEEN, KING, BISHOP2, KNIGHT2, ROOK2
} Figures;

extern std::vector<Figures> slided[2];
extern uint64_t slidedSet[2];

extern uint64_t leftSwapAval;
extern uint64_t rightSwapAval;
const uint64_t rightBorder = ~0x8080808080808080;
const uint64_t leftBorder =  ~0x0101010101010101;
const uint64_t maxBorder = 0xFFFFFFFFFFFFFFFF;

const uint64_t highBorder = 0xFF00000000000000;
const uint64_t lowBorder = 0x00000000000000FF;

const uint64_t doubleRightBorder = ~0xC0C0C0C0C0C0C0C0;
const uint64_t doubleLeftBorder = ~0x0303030303030303;
const uint64_t blackLineThough =  0x000000000000FF00;
const uint64_t whiteLineThough =  0x00000000FF000000;
const uint64_t blackLineStart = 0x00FF000000000000;
const uint64_t whiteLineStart = 0x000000000000FF00;
const uint64_t blackFigsStart = 0xFFFF000000000000;
const uint64_t whiteFigsStart = 0x000000000000FFFF;

const uint64_t swapArr[2] = {lowBorder, highBorder};

extern unsigned char shahCount;

extern uint64_t check;
extern uint64_t checkCount; // все линии шаха

typedef enum {
    WHITE, BLACK
} Color;

extern Color myCol, enCol;

extern Figures myFigs[17];

extern Figures enFigs[17]; // массив для превращения чужих фигур

extern uint64_t whites[17];

extern uint64_t blacks[17];

extern std::unordered_map<uint64_t, Figures> whiteChessBoard;

extern std::unordered_map<uint64_t, Figures> blackChessBoard;

extern std::unordered_map<uint64_t, Figures> ChessBoards[2];

extern uint64_t atMasks[17];

extern uint64_t board[2][17];

extern uint64_t attacks;

extern uint64_t figs[2];

extern uint64_t myMoves[17]; // потенциальные ходы

extern uint64_t prevPawns[2];

extern uint64_t currPawns[2];



void GenerateMoves();
void ReadyForNextStep();

#endif // BITBOARD_H
