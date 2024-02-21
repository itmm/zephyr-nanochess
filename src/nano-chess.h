#if !defined(Nano_Chess_h)
#define Nano_Chess_h

	#include "chess-interface.h"

	void put_string(const char* s);

	namespace Mek::Chess {
		class Nano_Chess : public Engine {
			public:
				void reset_board() override;
				void move(const Move& move) override;
				Move compute_move() override;
		};
	}

#endif
