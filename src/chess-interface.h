#if !defined (MekChessInterface_h)
	#define MekChessInterface_h

	#include "assert.h"

	namespace Mek::Chess {
		enum class Side : char { white = '>', black = '<' };

		enum class Piece : char {
			king = 'k', queen = 'q', bishop = 'b', knight = 'n',
			rook = 'r', pawn = 'p', none = ' '
		};

		Piece piece_from_ch(char ch);

		struct Position {
			Position(unsigned char col = 0, unsigned char row = 0):
				col { col }, row { row }
			{ assert(col <= 8); assert(row <= 8); }

			static unsigned char col_from_ch(char ch) {
				if (ch >= 'a' && ch <= 'h') { return ch + 1 - 'a'; }
				if (ch >= 'A' && ch <= 'H') { return ch + 1 - 'A'; }
				return 0;
			}

			static unsigned char row_from_ch(char ch) {
				if (ch >= '1' && ch <= '8') { return ch - '0'; }
				return 0;
			}

			Position(const char* str):
				Position { col_from_ch(str[0]), row_from_ch(str[1]) }
			{ }

			const unsigned char col: 4;
			const unsigned char row: 4;

			explicit operator bool() { return col > 0 && row > 0; }

			[[nodiscard]] char col_ch() const { return col + '0'; }
			[[nodiscard]] char row_ch() const { return row + '0'; }
		};

		static_assert(sizeof(Position) == 1);

		struct Move {
			Move(
				Side side, Position from, Position to, Piece promoted = Piece::none
			):
				side { side }, from { from }, to { to }, piece { piece },
				captured { captured }, promoted { promoted }
			{
				assert(from); assert(to); assert(piece != Piece::none);
			}

			const Side side;
			const Position from;
			const Position to;
			const Piece piece;
			const Piece captured;
			const Piece promoted;
		};

		class Engine {
			public:
				Side side { Side::white };

				virtual ~Engine() = default;

				void switch_sides() {
					side = side == Side::white ? Side::black : Side::white;
				}

				virtual void reset_board() { side = Side::white; }
				virtual void move(const Move& move) = 0;
				virtual void computer_move() = 0;
		};
	}

#endif
