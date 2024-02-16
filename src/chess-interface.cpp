#include "chess-interface.h"

using namespace Mek::Chess;

Piece Mek::Chess::piece_from_ch(char ch) {
	switch (ch) {
		case 'k': case 'K': return Piece::king;
		case 'q': case 'Q': return Piece::queen;
		case 'b': case 'B': return Piece::bishop;
		case 'n': case 'N': return Piece::knight;
		case 'r': case 'R': return Piece::rook;
		case 'p': case 'P': return Piece::pawn;
		default: return Piece::none;
	}
}

