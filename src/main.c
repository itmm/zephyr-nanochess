/**************************************************************************\
| Toledo Nanochess (c) Copyright 2009 Oscar Toledo G. All rights reserved  |
| 1257 non-blank characters. Evolution from my winning IOCCC 2005 entry.   |
| o Use D2D4 algebraic style for movements.  biyubi@gmail.com  Nov/20/2009 |
| o On promotion add a number for final piece (3=N, 4=B, 5=R, 6=Q)         |
| o Press Enter alone for computer to play.                                |
| o Full legal chess moves.                       http://www.nanochess.org |
| o Remove these comments to get 1326 bytes source code (*NIX end-of-line) |
\**************************************************************************/

#include <stdbool.h>
#if defined(CONFIG_ARCH_POSIX)
	#include <stdio.h>
#else
	#include <zephyr/kernel.h>
	#include <zephyr/sys/printk.h>
	#include <zephyr/console/console.h>
#endif

#define MAX_LEVEL 5

const char* default_board =
	// initial board
	"ustvrtsu"
	"qqqqqqqq"
	"yyyyyyyy"
	"}{|~z|{}";

const char* ref =
	// points by piece
	"   76Ls"

	// points for center
	"abcddcba "

	// piece letters
	".pknbrq  PKNBRQ " /* no longer needed */

	// movements
	"?A6J57IKJT576,+-48HLSU";

const char* piece_letters = ".pknbrq  PKNBRQ ";

int origin_of_move, promote_to, can_en_passant;
int target_of_move, board[411], *best_enemy_move = board;
char actual_side = 0;

static inline void switch_sides() { actual_side ^= 8; }

int next(
	int recapture_square, int reset_enemy_score, int level,
	int initial_search_square, int pawn_for_en_passant, int max_level
) {
	int original_content, score;
	int origin_square = initial_search_square, net_score = -100000000;
	int mate_score = (78 - level) << 10, *tmp_square;
	int final_piece, *rook_origin, limit_offset, current_piece;
	int original_target_content, movement_offset, cant_castle;
	int pawn_direction = actual_side ? -10 : 10;
	switch_sides();
	best_enemy_move++;
	int in_check = recapture_square ||
		(max_level && max_level >= level && next(0, 0, 0, 21, 0, 0) > 10000);

	do {
		int target_square = origin_square;
		if ((original_content = board[target_square])) {
			current_piece = (original_content & 0xf) ^ actual_side;
			if (current_piece < 7) {
				limit_offset = current_piece-- & 2 ? 8 : 4;
				movement_offset = (original_content - 9) & 0xf ?
					"& .$  "[current_piece] : 42;

				do {
					target_square += ref[movement_offset] - 64;
					original_target_content = board[target_square];
					if (!recapture_square | (target_square == recapture_square)) {
						tmp_square = current_piece |
							(target_square + pawn_direction - pawn_for_en_passant) ?
								0 : board + pawn_for_en_passant;

						if (!original_target_content & (current_piece | (limit_offset < 3) || tmp_square) || ((((original_target_content + 1) & 0xf) ^ actual_side) > 9 && current_piece | (limit_offset > 2))) {
							if ((rook_origin = (int*) !((original_target_content - 2) & 7))) {
								--best_enemy_move;
								best_enemy_move[1] = origin_square;
								switch_sides();
								return mate_score;
							}
							cant_castle = final_piece = original_content & 0xf;
							int before_pawn = board[target_square - pawn_direction] & 0xf;
							int limit_piece = current_piece | (before_pawn - 7) ? final_piece : (final_piece += 2, 6 ^ actual_side);
							while (final_piece <= limit_piece) {
								score = original_target_content ? ref[original_target_content & 7] * 9 - 189 - level - current_piece : 0;
								if (max_level) {
									score += (
											 1 - current_piece ? ref[target_square / 10 + 5] - ref[origin_square / 10 + 5] + ref[target_square % 10 + 6] * -~!current_piece - ref[origin_square % 10 + 6] +
													 original_content / 16 * 8 : (rook_origin ? 1 : 0) * 9
										 ) + (
											 current_piece ? 0 : !(board[target_square - 1] ^ final_piece) + !(board[target_square + 1] ^ final_piece) + ref[final_piece & 7] * 9 - 386 + (tmp_square ? 1 : 0) * 99 +
													 (limit_offset < 2)
										 ) + !(before_pawn ^ actual_side ^ 9);
								}
								if (max_level > level || ((1 < max_level) & (max_level == level) && (score > 0xf) | in_check)) {
									board[target_square] = final_piece, board[origin_square] = rook_origin && tmp_square ? *tmp_square = *rook_origin, *rook_origin = 0 : tmp_square ? *tmp_square = 0 : 0;
									score -= next((max_level > level) | in_check ? 0 : target_square, score - net_score, level + 1, best_enemy_move[1], cant_castle = (current_piece | (limit_offset > 1)) ? 0 : target_square, max_level);
									if (!(
										level || (max_level - 1) | (origin_of_move - origin_square) | (promote_to - final_piece) | (target_square - target_of_move) | (score < -10000)
									)) {
										--best_enemy_move;
										switch_sides(); switch_sides();
										return can_en_passant = cant_castle;
									}
									cant_castle = (current_piece - 1) | (limit_offset < 7) || rook_origin || (!max_level) | in_check | original_target_content | (original_content < 0xf) || next(0, 0, 0, 21, 0, 0) > 10000;
									board[origin_square] = original_content;
									board[target_square] = original_target_content;
									rook_origin && tmp_square ?
										*rook_origin = *tmp_square, *tmp_square = 0 :
										tmp_square ? *tmp_square = 9 ^ actual_side : 0;
								}
								if (score > net_score) {
									*best_enemy_move = origin_square;
									if (max_level > 1) {
										if (level && reset_enemy_score - score < 0) {
											switch_sides();
											return --best_enemy_move, score;
										}
										if (!level) { promote_to = final_piece, origin_of_move = origin_square, target_of_move = target_square; }
									}
									net_score = score;
								}
								final_piece += cant_castle || (tmp_square = board + target_square, rook_origin = target_square < origin_square ? tmp_square - 3 : tmp_square + 2, (*rook_origin < 0xf) | rook_origin[origin_square - target_square] || board[target_square += target_square - origin_square]);
							}
						}
					}
				} while (
					!original_target_content & (current_piece > 2) || (
						target_square = origin_square, current_piece | (limit_offset > 2) | (
							(original_content > 0xf) & !original_target_content
						) && ++movement_offset && --limit_offset
					)
				);
			}
		}
	} while (++origin_square > 98 ? (origin_square = 20) : initial_search_square - origin_square);
	--best_enemy_move;
	switch_sides();
	return net_score + 100000000 && (net_score > -mate_score + 1924) | in_check ? net_score : 0;
}

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

void my_putc(char ch) {
	#if defined(CONFIG_ARCH_POSIX)
		putchar(ch);
	#else
		printk("%c", ch);
	#endif
}

void my_puts(const char* s) {
	while (*s) { my_putc(*s++); }
}

static inline char next_default_piece(int offset) {
	if (offset / 10 % 10 < 2 || offset % 10 < 2) { return 7; }
	if (offset / 10 & 4) { return 0; }
	return *default_board++ & 31;
}

static inline void setup_board() {
	origin_of_move = 0;
	while (++origin_of_move < 121) {
		*best_enemy_move++ = next_default_piece(origin_of_move);
	}
}

static inline void print_board() {
	int i = 19;
	while (i++ < 99) {
		if (i % 10) {
			my_putc(piece_letters[board[i] & ~16]);
		} else { my_putc('\n'); }
	}
}

static inline void read_promotion_piece() {
	promote_to = board[origin_of_move] & 0xf;
	for (;;) {
		char ch = my_getc() & 0xf;
		if (ch == '\n') { break; }
		promote_to = ch ^ 8 ^ actual_side;
	}
}

int main() {
	#if !defined(CONFIG_ARCH_POSIX)
		console_init();
	#endif
	setup_board();
	for (;;) {
		print_board();

		origin_of_move = my_getc() & 0xf;
		if (origin_of_move != '\n') {
			origin_of_move += ((10 - my_getc()) & 0xf) * 10;
			target_of_move = my_getc() & 0xf;
			target_of_move += ((10 - my_getc()) & 0xf) * 10;
			read_promotion_piece();
			my_puts("perform move ");
			my_putc(9 - origin_of_move % 10 + 'A');
			my_putc(10 - origin_of_move / 10 + '0');
			my_putc(9 - target_of_move % 10 + 'A');
			my_putc(10 - target_of_move / 10 + '0');
			my_putc('\n');
		} else {
			my_puts("perform computer move\n");
			next(0, 0, 0, 21, can_en_passant, MAX_LEVEL);
		}
		next(0, 0, 0, 21, can_en_passant, 1);
	}
}