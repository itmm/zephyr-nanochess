#include "nano-chess.h"

#if defined(CONFIG_ARCH_POSIX)
	#include <cstdio>
#else
	#include <zephyr/kernel.h>
	#include <zephyr/sys/printk.h>
	#include <zephyr/console/console.h>
#endif

char get_char() {
	#if defined(CONFIG_ARCH_POSIX)
		return static_cast<char>(getchar());
	#else
		for (;;) {
			int got = console_getchar();
			if (got) {
				printk("%c", got);
				if (got == 0x0d) { printk("\x0a"); got = 0x0a; }
				return static_cast<char>(got);
			}
		}
	#endif
}

int main [[noreturn]]() {
	#if !defined(CONFIG_ARCH_POSIX)
		console_init();
	#endif
	Mek::Chess::Nano_Chess engine;
	engine.reset_board();
	for (;;) {
		char from[3], to[3];
		from[0] = get_char();
		if (from[0] != '\n') {
			from[1] = get_char(); from[2] = '\0';
			Mek::Chess::Position from_pos { from };
			to[0] = get_char(); to[1] = get_char(); to[2] = '\0';
			Mek::Chess::Position to_pos { to };
			Mek::Chess::Piece promote_to { Mek::Chess::Piece::none };
			char promote_ch = get_char();
			while (promote_ch != '\n') {
				promote_to = Mek::Chess::piece_from_ch(promote_ch);
				promote_ch = get_char();
			}
			engine.move(Mek::Chess::Move { from_pos, to_pos, promote_to });
		} else { engine.computer_move(); }
	}
}