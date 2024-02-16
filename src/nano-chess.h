#if !defined(Nano_Chess_h)
#define Nano_Chess_h

	#include "chess-interface.h"

	namespace Mek::Chess {
		class Nano_Chess : public Engine {
			public:
				void reset_board() override;
				void move(const Move& move) override;
				void computer_move() override;
		};
	}

#endif
