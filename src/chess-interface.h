#if !defined (MekChessInterface_h)
	#define MekChessInterface_h

	#include <cassert>

	namespace Mek::Chess {
		enum class Piece : char {
			king = 'k', queen = 'q', bishop = 'b', knight = 'n',
			rook = 'r', pawn = 'p', none [[maybe_unused]] = ' '
		};

		Piece piece_from_ch(char ch);

		struct Position {
			static unsigned char valid_num(int num) {
				assert(num >= 0); assert(num <= 8);
				return static_cast<unsigned char>(num & 0xf);
			}

			explicit Position(int col = 0, int row = 0):
				col { valid_num(col) }, row { valid_num(row) }
			{ }

			static unsigned char col_from_ch(char ch) {
				if (ch >= 'a' && ch <= 'h') { return ch + 1 - 'a'; }
				if (ch >= 'A' && ch <= 'H') { return ch + 1 - 'A'; }
				return 0;
			}

			static unsigned char row_from_ch(char ch) {
				if (ch >= '1' && ch <= '8') { return ch - '0'; }
				return 0;
			}

			explicit Position(const char* str):
				Position { col_from_ch(str[0]), row_from_ch(str[1]) }
			{ }

			const unsigned char col: 4;
			const unsigned char row: 4;

			explicit operator bool() const { return col > 0 && row > 0; }

			[[nodiscard]] char col_ch() const {
				return static_cast<char>(col + '0');
			}

			[[nodiscard]] char row_ch() const {
				return static_cast<char>(row + '0');
			}
		};

		static_assert(sizeof(Position) == 1);

		struct Move {
			Move(Position from, Position to, Piece piece, Piece promoted = Piece::none):
				from { from }, to { to }, piece { piece }, promoted { promoted }
			{ assert(from); assert(to); assert(piece != Piece::none); }

			const Position from;
			const Position to;
			const Piece piece;
			const Piece promoted;
		};

		class Engine {
			public:
				virtual ~Engine() = default;

				virtual void reset_board() = 0;
				virtual void move(const Move& move) = 0;
				virtual void computer_move() = 0;
		};
	}

#endif
