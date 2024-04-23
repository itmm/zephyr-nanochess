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

void print_prompt() { put_string("\n> "); }
void input_error(const char* reason) {
	put_string(reason);
	put_string(", try again");
	print_prompt();
}

void main_loop [[noreturn]] () {
	Mek::Chess::Nano_Chess engine;
	engine.reset_board(); print_prompt();
	char line[16];
	const char* line_end { line + sizeof(line)/sizeof(*line) - 1 };

	for (;;) {
		char* line_cur = line;
		for (;;) {
			char got { get_char() };
			if (got == '\n') { break; }
			if (got <= ' ') { continue; }
			if (line_cur == line_end) {
				while (get_char() != '\n') { }
				line_cur = line;
				input_error("line too long"); continue;
			}
			*line_cur++ = got;
		}
		if (line_cur == line) {
			engine.move(engine.compute_move()); print_prompt();
			continue;
		}
		if (line_cur - line < 5) { input_error("missing move or piece"); continue; }

		Mek::Chess::Position from_pos { line };
		Mek::Chess::Position to_pos { line + 2 };
		auto piece { Mek::Chess::piece_from_ch(line[4]) };
		if (!from_pos) { input_error("from position not valid"); continue; }
		if (!to_pos) { input_error("to position not valid"); continue; }
		if (piece == Mek::Chess::Piece::none) { input_error("piece not valid"); continue; }

		Mek::Chess::Piece promote_to { Mek::Chess::Piece::none };

		auto cur { line + 5};
		if (cur < line_cur && *cur == '!') {
			++cur;
			if (cur >= line_cur) { put_string("missing promote piece"); continue; }
			promote_to = Mek::Chess::piece_from_ch(*cur++);
			if (promote_to == Mek::Chess::Piece::none) {
				input_error("invalid promote piece"); continue;
			}
		}

		Mek::Chess::Piece captured { Mek::Chess::Piece::none };
		const char* captured_position { nullptr };
		if (cur < line_cur && *cur == 'x') {
			++cur;
			if (cur >= line_cur) { put_string("missing captured piece"); continue; }
			captured = Mek::Chess::piece_from_ch(*cur++);
			if (captured == Mek::Chess::Piece::none) {
				input_error("invalid captured piece"); continue;
			}
			if (cur < line_cur) {
				if (cur + 1 >= line_cur) {
					put_string("incomplete captured position"); continue;
				}
				captured_position = cur;
				cur += 2;
			}
		}

		if (cur != line_cur) {
			put_string("unexpected characters in input line"); continue;
		}

		if (captured != Mek::Chess::Piece::none) {
			if (captured_position) {
				engine.move(Mek::Chess::Move {
					from_pos, to_pos, piece, promote_to, captured, Mek::Chess::Position { captured_position }
				});
			} else {
				engine.move(Mek::Chess::Move { from_pos, to_pos, piece, promote_to, captured, to_pos });
			}
		} else {
			engine.move(Mek::Chess::Move { from_pos, to_pos, piece, promote_to });
		}
		print_prompt();
	}
}

int main() {
	#if !defined(CONFIG_ARCH_POSIX)
		console_init();
	#endif
	main_loop();
}