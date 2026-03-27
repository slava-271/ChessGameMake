#include "bitboard.h"

Color myCol = WHITE;
Color enCol = BLACK;

std::vector<Figures> slided[2] = {{BISHOP1, BISHOP2, ROOK1, ROOK2, QUEEN}, {BISHOP1, BISHOP2, ROOK1, ROOK2, QUEEN}};

uint64_t slidedSet[2] = { 1 << BISHOP1 | 1 << BISHOP2 | 1 << ROOK1 | 1 << ROOK2 | 1 << QUEEN,   1 << BISHOP1 | 1 << BISHOP2 | 1 << ROOK1 | 1 << ROOK2 | 1 << QUEEN};

uint64_t leftSwapAval = 0xFFFFFFFFFFFFFFFF;
uint64_t rightSwapAval = 0xFFFFFFFFFFFFFFFF;

unsigned char shahCount = 0;

uint64_t check, checkCount = 0; // все линии шаха



Figures myFigs[17] = {NONE, PAWN1, PAWN2, PAWN3, PAWN4, PAWN5, PAWN6, PAWN7,
                             PAWN8, ROOK1, KNIGHT1, BISHOP1, QUEEN, KING, BISHOP2, KNIGHT2, ROOK2}; // массив для превращения своих фигур

Figures enFigs[17] = {NONE, PAWN1, PAWN2, PAWN3, PAWN4, PAWN5, PAWN6, PAWN7,
                             PAWN8, ROOK1, KNIGHT1, BISHOP1, QUEEN, KING, BISHOP2, KNIGHT2, ROOK2}; // массив для превращения чужих фигур

uint64_t whites[17] = {0,(1ull << 8ull), (1ull << 9ull), (1ull << 10ull), (1ull << 11ull), (1ull << 12ull),
                              (1ull << 13ull), (1ull << 14ull), (1ull << 15ull), 1ull, 2ull, 4ull, 8ull,
                              16ull, 32ull, 64ull, 128ull};

uint64_t blacks[17] = {0,(1ull << 55ull), (1ull << 54ull), (1ull << 53ull), (1ull << 52ull), (1ull << 51ull),
                              (1ull << 50ull), (1ull << 49ull), (1ull << 48ull),   (1ull << 63ull), (1ull << 62ull),
                              (1ull << 61ull), (1ull << 59ull), (1ull << 60ull),
                              (1ull << 58ull), (1ull << 57ull), (1ull << 56ull)};

std::unordered_map<uint64_t, Figures> whiteChessBoard = { {0, NONE},
    {1, ROOK1}, {2, KNIGHT1}, {4, BISHOP1}, {8, QUEEN},
    {16, KING}, {32, BISHOP2}, {64, KNIGHT2}, {128, ROOK2},
    {256, PAWN1}, {512, PAWN2}, {1024, PAWN3}, {2048, PAWN4},
    {4096, PAWN5}, {8192, PAWN6}, {16384, PAWN7}, {32768, PAWN8}
};

std::unordered_map<uint64_t, Figures> blackChessBoard = { {0, NONE},
    {1ull << 56ull, ROOK2}, {1ull << 57ull, KNIGHT2}, {1ull << 58ull, BISHOP2}, {1ull << 59ull, QUEEN},
    {1ull << 60ull, KING}, {1ull << 61ull, BISHOP1}, {1ull << 62ull, KNIGHT1}, {1ull << 63ull, ROOK1},
    {1ull << 48ull, PAWN8}, {1ull << 49ull, PAWN7}, {1ull << 50ull, PAWN6}, {1ull << 51ull, PAWN5},
    {1ull << 52ull, PAWN4}, {1ull << 53ull, PAWN3}, {1ull << 54ull, PAWN2}, {1ull << 55ull, PAWN1}
};

std::unordered_map<uint64_t, Figures> ChessBoards[2] = {whiteChessBoard, blackChessBoard};

uint64_t atMasks[17] ={maxBorder, maxBorder, maxBorder, maxBorder, maxBorder,maxBorder, maxBorder,
                               maxBorder,maxBorder,maxBorder,maxBorder,maxBorder,maxBorder,maxBorder,maxBorder,maxBorder,maxBorder};

uint64_t board[2][17] = {whiteFigures, blackFigures}; // массив фигур белых и черных

uint64_t attacks = 0;

uint64_t figs[2] = {whiteFigsStart, blackFigsStart}; //маски объедиенные всех фигур

uint64_t myMoves[17] = {}; // потенциальные ходы

uint64_t prevPawns[2] =  {};

uint64_t currPawns[2] =  {whiteLineStart, blackLineStart};





uint64_t WhiteAttackPrim(uint64_t& pawn) {
        return  (( (pawn << 7) & rightBorder) | ( (pawn << 9) & leftBorder));
}

uint64_t BlackAttackPrim(uint64_t& pawn) {
    return  (( (pawn >> 7) & leftBorder) | ( (pawn >> 9) & rightBorder));
}

uint64_t GenWhitePawnAttacks(uint64_t& pawn) {
    uint64_t attack = ( (pawn << 7) & rightBorder) | ( (pawn << 9) & leftBorder);
    uint64_t throughAttack = attack & blackLineThough & (prevPawns[BLACK] >> 8)
     &  (~prevPawns[BLACK] << 8) & (~currPawns[BLACK] >> 8) & (currPawns[BLACK] << 8);
    attack = throughAttack | (attack & figs[BLACK]);
    return attack;
}

uint64_t GenBlackPawnAttacks(uint64_t& pawn) {
    uint64_t attack = ( (pawn >> 7) & leftBorder) | ( (pawn >> 9) & rightBorder);
    uint64_t throughAttack = attack & whiteLineThough & (prevPawns[WHITE] << 8)
    &  (~prevPawns[WHITE] >> 8) & (~currPawns[WHITE] << 8) & (currPawns[WHITE] >> 8);
    attack = throughAttack | (attack & figs[WHITE]);
    return attack;
}

uint64_t GenWhitePawnMoves(uint64_t& pawn) {
    uint64_t pre = (pawn << 8) & ~figs[BLACK] & ~figs[WHITE];
    uint64_t moves = pre | ( ( (pawn & whiteLineStart) << 16) & (pre << 8));
    moves |=  GenWhitePawnAttacks(pawn);
    currPawns[WHITE] |= moves;
    return moves;
}

uint64_t GenBlackPawnMoves(uint64_t& pawn) {
    uint64_t pre = (pawn >> 8) & ~figs[BLACK] & ~figs[WHITE];
    uint64_t moves = pre | ( ( (pawn & blackLineStart) >> 16) & (pre >> 8));
    moves |=  GenBlackPawnAttacks(pawn);
    currPawns[BLACK] |= moves;
    return moves;
}

typedef uint64_t (*PawnMoveFunc)(uint64_t&);

PawnMoveFunc pawnMoveFunctions[2] = {GenWhitePawnMoves, GenBlackPawnMoves};

PawnMoveFunc primArr[2] = {WhiteAttackPrim, BlackAttackPrim};

uint64_t GenPawnMove(uint64_t& pawn, const Color color) {
    return pawnMoveFunctions[color](pawn);
}

uint64_t PawnPrim(uint64_t& pawn, const Color color) {
    return primArr[color](pawn);
}

uint64_t GenKingPrim(uint64_t& king, const Color color) {
    // uint64_t buff = (king << 8);
    // uint64_t aboba = (king >> 8);
    // aboba *= 2;
    // aboba |= buff;
    uint64_t res;
    res = WhiteAttackPrim(king);
    res |= BlackAttackPrim(king);
    res |= (king << 8);
    res |= (king >> 8);
    res |= (king << 1) & leftBorder;
    res |= (king >> 1) & rightBorder;
    return res;


}



uint64_t GenKnight(uint64_t& knight, const Color color) {
    uint64_t moves = ((knight << 17) & leftBorder) | ((knight << 15) & rightBorder) |
    ((knight << 6) & doubleRightBorder) | ((knight << 10) & doubleLeftBorder) |
    ((knight >> 17) & rightBorder) | ((knight >> 15) & leftBorder) | ((knight >> 6) & doubleLeftBorder) |
    ((knight >> 10) & doubleRightBorder);
    return moves;
}

uint64_t SlidePrimLeft(uint64_t& slide,const uint64_t frColor, const uint64_t num) {
    slide <<= num;
    return (slide) & ~frColor;
}

uint64_t SlidePrimRight(uint64_t& slide,const uint64_t frColor, const uint64_t num) {
    slide >>= num;
    return (slide) & ~frColor;
}

uint64_t SlideKingLeft(uint64_t& slide,const uint64_t num) {
    slide <<= num;
    return slide;
}

uint64_t SlideKingRight(uint64_t& slide,const uint64_t num) {
    slide >>= num;
    return slide;
}





uint64_t GenSlide(uint64_t slide,const Color color, const uint64_t mask, const uint64_t num, uint64_t (*func)(uint64_t& slide,const uint64_t frColor, const uint64_t num)) {
    uint64_t buff, moves = 0;
    do {
        buff = func(slide, figs[color], num);
        moves |= buff;
        buff = buff & ~figs[color ^ 1]  & mask;
    }while (buff);
    moves &= mask;
    return moves;
}

uint64_t GenSlideKing(uint64_t slide,const Color color, const uint64_t mask, const uint64_t num, uint64_t (*func)(uint64_t& slide, const uint64_t num)) {
    uint64_t buff, moves = 0;
    do {
        buff = func(slide, num);
        moves |= buff;
        buff = buff & ~figs[color ^ 1] & ~figs[color] & mask;
    }while (buff);
    moves &= mask;
    return moves;
}

uint64_t checkArr[2] = {0, 0xFFFFFFFFFFFFFFFF};

uint64_t GenSlideAttack(uint64_t slide,const Color color, const uint64_t mask,const uint64_t num, uint64_t (*func)(uint64_t& slide,const uint64_t frColor, const uint64_t num)) {
    uint64_t buff, moves = 0, flag;
    do {
        buff = func(slide, figs[color], num);
        moves |= buff;
        flag = buff & ~figs[color] & mask & ~board[color][KING];
        buff = buff & figs[color ^ 1] & mask;
    }while (buff);

    while (flag) {
        buff = func(slide, figs[color], num);
        moves |= buff;
        flag = buff & ~figs[color ^ 1] & mask;
        buff = buff & figs[color ^ 1] & mask;
    }
    buff = moves | board[color ^ 1][KING];
    moves = buff & checkArr[buff == moves];
    uint64_t index = figs[color^1] & moves;
    atMasks[ChessBoards[color][index]] = checkArr[!!(index)] | moves;
    return moves;
}



uint64_t GenBishop(uint64_t& bishop, const Color color) {
    uint64_t moves;
    moves = GenSlide(bishop, color,rightBorder, 7, SlidePrimLeft);
    moves |= GenSlide(bishop, color, leftBorder, 7, SlidePrimRight);
    moves |= GenSlide(bishop, color, leftBorder, 9, SlidePrimLeft);
    moves |= GenSlide(bishop, color, rightBorder, 9, SlidePrimRight);
    return moves;
}

uint64_t GenBishopKing(uint64_t& bishop, const Color color) {
    uint64_t moves;
    moves = GenSlideKing(bishop, color,rightBorder, 7, SlideKingLeft);
    moves |= GenSlideKing(bishop, color, leftBorder, 7, SlideKingRight);
    moves |= GenSlideKing(bishop, color, leftBorder, 9, SlideKingLeft);
    moves |= GenSlideKing(bishop, color, rightBorder, 9, SlideKingRight);
    return moves;
}

uint64_t GenBishopAttack(uint64_t& bishop, const Color color) {
    uint64_t moves;
    moves = GenSlideAttack(bishop, color,rightBorder, 7, SlidePrimLeft);
    moves |= GenSlideAttack(bishop, color, leftBorder, 7, SlidePrimRight);
    moves |= GenSlideAttack(bishop, color, leftBorder, 9, SlidePrimLeft);
    moves |= GenSlideAttack(bishop, color, rightBorder, 9, SlidePrimRight);
    return moves;
}


uint64_t GenRook(uint64_t& rook, const Color color) {
    uint64_t moves;
    moves = GenSlide(rook, color, maxBorder, 8, SlidePrimLeft);
    moves |= GenSlide(rook, color, maxBorder, 8, SlidePrimRight);
    moves |= GenSlide(rook, color, rightBorder, 1, SlidePrimLeft);
    moves |= GenSlide(rook, color, leftBorder, 1, SlidePrimRight);
    return moves;
}

uint64_t GenRookKing(uint64_t& bishop, const Color color) {
    uint64_t moves;
    moves = GenSlideKing(bishop, color,rightBorder, 8, SlideKingLeft);
    moves |= GenSlideKing(bishop, color, leftBorder, 8, SlideKingRight);
    moves |= GenSlideKing(bishop, color, leftBorder, 1, SlideKingLeft);
    moves |= GenSlideKing(bishop, color, rightBorder, 1, SlideKingRight);
    return moves;
}

uint64_t GenRookAttack(uint64_t& rook, const Color color) {
    uint64_t moves;
    moves = GenSlideAttack(rook, color, maxBorder, 8, SlidePrimLeft);
    moves |= GenSlideAttack(rook, color, maxBorder, 8, SlidePrimRight);
    moves |= GenSlideAttack(rook, color, rightBorder, 1, SlidePrimLeft);
    moves |= GenSlideAttack(rook, color, leftBorder, 1, SlidePrimRight);
    return moves;
}

uint64_t GenQueen(uint64_t& queen, const Color color) {
    uint64_t moves;
    moves = GenBishop(queen, color);
    moves |= GenRook(queen, color);
    return moves;
}

uint64_t GenQueenKing(uint64_t& queen, const Color color) {
    uint64_t moves;
    moves = GenBishopKing(queen, color);
    moves |= GenRookKing(queen, color);
    return moves;
}


uint64_t GenQueenAttack(uint64_t& queen, const Color color) {
    uint64_t moves;
    moves = GenBishopAttack(queen, color);
    moves |= GenRookAttack(queen, color);
    return moves;
}




typedef uint64_t (*GenFunc)(uint64_t&, const Color);

uint64_t GenKing(uint64_t& king, const Color color);

GenFunc genFuncArr[ROOK2 + 1] = {GenPawnMove, GenPawnMove, GenPawnMove, GenPawnMove, GenPawnMove,
GenPawnMove,GenPawnMove,GenPawnMove,GenPawnMove, GenRook, GenKnight, GenBishop, GenQueen, GenKingPrim,
GenBishop, GenKnight, GenRook};

GenFunc genMoveArr[ROOK2 + 1] = {GenPawnMove, GenPawnMove, GenPawnMove, GenPawnMove, GenPawnMove,
                                 GenPawnMove,GenPawnMove,GenPawnMove,GenPawnMove, GenRook, GenKnight, GenBishop, GenQueen, GenKing,
                                 GenBishop, GenKnight, GenRook};

GenFunc genSlideArr[ROOK2 + 1] = {GenPawnMove, GenPawnMove, GenPawnMove, GenPawnMove, GenPawnMove,
                                 GenPawnMove,GenPawnMove,GenPawnMove,GenPawnMove, GenRookAttack, GenKnight, GenBishopAttack, GenQueenAttack, GenKingPrim,
                                 GenBishopAttack, GenKnight, GenRookAttack};

GenFunc genKingArr[ROOK2 + 1] = {PawnPrim, PawnPrim, PawnPrim, PawnPrim, PawnPrim,
                                 PawnPrim,PawnPrim,PawnPrim,PawnPrim, GenRookKing, GenKnight, GenBishopKing, GenQueenKing,GenKingPrim,
                                 GenBishopKing, GenKnight, GenRookKing};

uint64_t GenPawn(uint64_t& pawn, uint64_t figs[], const Figures fig, const Color color) {
    return genFuncArr[figs[fig]](pawn, color);
}

GenFunc genAttackArr[ROOK2 + 1] = {PawnPrim, PawnPrim, PawnPrim, PawnPrim, PawnPrim,
PawnPrim,PawnPrim,PawnPrim,PawnPrim, GenRook, GenKnight, GenBishop, GenQueen,GenKingPrim,
GenBishop, GenKnight, GenRook};



uint64_t RecCheck(uint64_t& king, const Figures figure) {
    uint64_t buff, buffCheck;
    buff = genAttackArr[enFigs[figure]](board[enCol][figure], enCol);
    buff |= board[enCol][figure];
    buffCheck = king | buff;
    checkCount += (buffCheck == buff);
    buffCheck = checkArr[buffCheck == buff] & buffCheck;
    return buffCheck;
}

uint64_t FindChecks(uint64_t& king) {
    checkCount = 0;
    check = 0;
    for (char i = 1; i < 17 && checkCount < 2; i++) {
        check |= RecCheck(king,(Figures)i);
    }
    check |= checkArr[check == 0];
    return check;
}

uint64_t IsUnderAttack() {
    attacks = 0;
    for (char i = 1; i < 17; i++) {
        attacks |= genKingArr[i](board[enCol][i], enCol);
    }
    return attacks;
}

uint64_t GenKing(uint64_t& king, const Color color) {
    IsUnderAttack();
    uint64_t moves = GenKingPrim(king, color);
    uint64_t buff;
    moves &= ~attacks;
    moves &= ~figs[myCol];

    buff = (king << 2);
    buff &= ~attacks;
    buff |= (moves << 1);
    buff &= ~figs[enCol];
    buff &= rightSwapAval;
    buff &= swapArr[color];
    moves |= buff;

    buff = (king >> 2);
    buff &= ~attacks;
    buff |= (moves >> 1);
    buff &= ~figs[enCol];
    buff &= leftSwapAval;
    buff &= swapArr[color];
    moves |= buff;

    moves |= (king >> 2) & ~attacks & (moves >> 1) & ~figs[enCol] & leftSwapAval & swapArr[color];
    return moves;
}

void GenDoubleAttacks(std::vector<Figures>& slides) {
    for (size_t i = 0; i < slides.size(); i++) {
        genSlideArr[enFigs[slides[i]]](board[enCol][slides[i]], enCol);
    }
}

void GenerateMoves() {
    prevPawns[myCol] = currPawns[myCol];
    FindChecks(board[myCol][KING]);
    GenDoubleAttacks(slided[enCol]);
    for (char i = 1; i < 17; i++) {
        myMoves[i] = genMoveArr[i](board[myCol][i], myCol) & check & atMasks[i];
    }
}

void ReadyForNextStep() {
    for (int i = 0; i < 17; i++) {
        atMasks[i] = maxBorder;
    }
    figs[myCol] = 0;
    figs[enCol] = 0;
    for (int i = 1; i < 17; i++) {
        figs[myCol] |= board[myCol][i];
        figs[enCol] |= board[enCol][i];
    }
    currPawns[myCol] = 0;
    for (int i = 1; i < 8; i++) {
        if ((myFigs[i] <= PAWN8) && (myFigs[i] > NONE))
          currPawns[myCol] |= board[myCol][i];
    }
}











