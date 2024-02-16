#include "nano-chess.h"

#if defined(CONFIG_ARCH_POSIX)
	#include <stdio.h>
#else
	#include <zephyr/kernel.h>
	#include <zephyr/sys/printk.h>
	#include <zephyr/console/console.h>
#endif

int my_getc() {
	#if defined(CONFIG_ARCH_POSIX)
	return getchar();
	#else
	for (;;) {
		int got = console_getchar();
		if (got) {
			printk("%c", got);
			if (got == 0x0d) { printk("\x0a"); got = 0x0a; }
			return got;
		}
	}
	#endif
}

int main() {
	#if !defined(CONFIG_ARCH_POSIX)
		console_init();
	#endif
	Mek::Chess::Nano_Chess engine;
	engine.reset_board();
	for (;;) {
		char from[3], to[3];
		from[0] = my_getc();
		if (from[0] != '\n') {
			from[1] = my_getc(); from[2] = '\0';
			Mek::Chess::Position from_pos { from };
			to[0] = my_getc(); to[1] = my_getc(); to[2] = '\0';
			Mek::Chess::Position to_pos { to };
			Mek::Chess::Piece promote_to { Mek::Chess::Piece::none };
			char promote_ch = my_getc();
			while (promote_ch != '\n') {
				promote_to = Mek::Chess::piece_from_ch(promote_ch);
			}
			engine.move(Mek::Chess::Move {
				engine.side, from_pos, to_pos, Mek::Chess::Piece::pawn /* TODO */,
				Mek::Chess::Piece::none /* TODO */, promote_to
			});
		} else { engine.computer_move(); }
	}
}