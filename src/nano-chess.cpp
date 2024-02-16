#include "nano-chess.h"

/**************************************************************************\
| Toledo Nanochess (c) Copyright 2009 Oscar Toledo G. All rights reserved  |
| 1257 non-blank characters. Evolution from my winning IOCCC 2005 entry.   |
| o Use D2D4 algebraic style for movements.  biyubi@gmail.com  Nov/20/2009 |
| o On promotion add a number for final piece (3=N, 4=B, 5=R, 6=Q)         |
| o Press Enter alone for computer to play.                                |
| o Full legal chess moves.                       http://www.nanochess.org |
| o Remove these comments to get 1326 bytes source code (*NIX end-of-line) |
\**************************************************************************/

#if defined(CONFIG_ARCH_POSIX)
	#include <stdio.h>
#else
	#include <zephyr/kernel.h>
	#include <zephyr/sys/printk.h>
	#include <zephyr/console/console.h>
#endif

using namespace Mek::Chess;

namespace {
	#define MAX_LEVEL 5

	const char* orig_default_board =
		// initial board
		"ustvrtsu"
		"qqqqqqqq"
		"yyyyyyyy"
		"}{|~z|{}";

	const char* default_board = orig_default_board;

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
	const char* movement_offsets = "& .$  ";
	char origin_of_move;
	char promote_to;
	char can_en_passant;
	char target_of_move;
	char board[411];
	char* best_enemy_move = board;
	char actual_side = 0;

	static inline void nano_switch_sides() { actual_side ^= 8; }

	int next(
		char recapture_square, char reset_enemy_score, char level,
		char initial_search_square, char pawn_for_en_passant, char max_level
	) {
		char origin_square = initial_search_square;
		int net_score = -100000000;
		const int mate_score = (78 - level) << 10;
		char* rook_origin = NULL;
		const char pawn_direction = actual_side ? -10 : 10;
		nano_switch_sides();
		best_enemy_move++;
		char in_check = recapture_square ||
						(max_level && max_level >= level && next(0, 0, 0, 21, 0, 0) > 10000);

		do {
			char target_square = origin_square;
			char original_content = board[(int) target_square];
			if (original_content) {
				char current_piece = (original_content & 0xf) ^ actual_side;
				if (current_piece < 7) {
					char limit_offset = current_piece-- & 2 ? 8 : 4;
					char movement_offset = (original_content - 9) & 0xf ?
						movement_offsets[(int) current_piece] : 42;

					char original_target_content;
					do {
						target_square += ref[(int) movement_offset] - 64;
						original_target_content = board[(int) target_square];
						if (!recapture_square | (target_square == recapture_square)) {
							char *tmp_square = current_piece |
											   (target_square + pawn_direction - pawn_for_en_passant) ?
								0 : board + pawn_for_en_passant;

							if (!original_target_content & (current_piece | (limit_offset < 3) || tmp_square) || ((((original_target_content + 1) & 0xf) ^ actual_side) > 9 && current_piece | (limit_offset > 2))) {
								if (!((original_target_content - 2) & 7)) {
									--best_enemy_move;
									best_enemy_move[1] = origin_square;
									nano_switch_sides();
									return mate_score;
								}
								char final_piece = original_content & 0xf;
								char cant_castle = final_piece;
								char before_pawn = board[target_square - pawn_direction] & 0xf;
								char limit_piece = current_piece | (before_pawn - 7) ? final_piece : (final_piece += 2, 6 ^ actual_side);
								while (final_piece <= limit_piece) {
									int score = original_target_content ? ref[original_target_content & 7] * 9 - 189 - level - current_piece : 0;
									if (max_level) {
										score += (
													 1 - current_piece ? ref[target_square / 10 + 5] - ref[origin_square / 10 + 5] + ref[target_square % 10 + 6] * (!current_piece ? 2 : 1) - ref[origin_square % 10 + 6] +
																		 original_content / 16 * 8 : (rook_origin ? 1 : 0) * 9
												 ) + (
													 current_piece ? 0 : !(board[target_square - 1] ^ final_piece) + !(board[target_square + 1] ^ final_piece) + ref[final_piece & 7] * 9 - 386 + (tmp_square ? 1 : 0) * 99 +
																		 (limit_offset < 2)
												 ) + !(before_pawn ^ actual_side ^ 9);
									}
									if (max_level > level || ((1 < max_level) & (max_level == level) && (score > 0xf) | in_check)) {
										board[(int) target_square] = final_piece;
										board[(int) origin_square] = rook_origin && tmp_square ? *tmp_square = *rook_origin, *rook_origin = 0 : tmp_square ? *tmp_square = 0 : 0;
										score -= next((max_level > level) | in_check ? 0 : target_square, score - net_score, level + 1, best_enemy_move[1], cant_castle = (current_piece | (limit_offset > 1)) ? 0 : target_square, max_level);
										if (!(
											level || (max_level - 1) | (origin_of_move - origin_square) | (promote_to - final_piece) | (target_square - target_of_move) | (score < -10000)
										)) {
											--best_enemy_move;
											nano_switch_sides(); nano_switch_sides();
											return can_en_passant = cant_castle;
										}
										cant_castle = (current_piece - 1) | (limit_offset < 7) || rook_origin || (!max_level) | in_check | original_target_content | (original_content < 0xf) || next(0, 0, 0, 21, 0, 0) > 10000;
										board[(int) origin_square] = original_content;
										board[(int) target_square] = original_target_content;
										if (rook_origin && tmp_square) {
											*rook_origin = *tmp_square;
											*tmp_square = 0;
										} else if (tmp_square) {
											*tmp_square = 9 ^ actual_side;
										}
									}
									if (score > net_score) {
										*best_enemy_move = origin_square;
										if (max_level > 1) {
											if (level && reset_enemy_score - score < 0) {
												nano_switch_sides();
												--best_enemy_move;
												return score;
											}
											if (!level) {
												promote_to = final_piece;
												origin_of_move = origin_square;
												target_of_move = target_square;
											}
										}
										net_score = score;
									}
									final_piece += cant_castle || (tmp_square = board + target_square, rook_origin = target_square < origin_square ? tmp_square - 3 : tmp_square + 2, (*rook_origin < 0xf) | rook_origin[(int) (origin_square - target_square)] || board[(int) (target_square += target_square - origin_square)]);
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
		nano_switch_sides();
		return net_score + 100000000 && (net_score > -mate_score + 1924) | in_check ? net_score : 0;
	}

	static inline char next_default_piece(int offset) {
		if (offset / 10 % 10 < 2 || offset % 10 < 2) { return 7; }
		if (offset / 10 & 4) { return 0; }
		return *default_board++ & 31;
	}

	static inline void setup_board() {
		default_board = orig_default_board;
		best_enemy_move = board;
		origin_of_move = 0;
		while (++origin_of_move < 121) {
			*best_enemy_move++ = next_default_piece(origin_of_move);
		}
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

	static inline void print_board() {
		int i = 19;
		while (i++ < 99) {
			if (i % 10) {
				my_putc(piece_letters[board[i] & ~16]);
			} else { my_putc('\n'); }
		}
	}

	void print_position(int pos) {
		my_putc(pos % 10 + 'A' - 1); my_putc(10 - pos / 10 + '0');
	}
}

void Nano_Chess::reset_board() {
	Engine::reset_board();
	setup_board(); print_board();
}

void Nano_Chess::move(const Move& move) {
	origin_of_move = move.from.col_ch() & 0xf;
	origin_of_move += ((10 - move.from.row_ch()) & 0xf) * 10;
	target_of_move = move.to.col_ch() & 0xf;
	target_of_move += ((10 - move.to.row_ch()) & 0xf) * 10;
	if (move.promoted != Piece::none) {
		promote_to = static_cast<char>(move.promoted);
	} else {
		promote_to = board[(int) origin_of_move] & 0xf;
	}
	my_puts("\nperform move ");
	print_position(origin_of_move);
	print_position(target_of_move);
	my_putc('\n');
	next(0, 0, 0, 21, can_en_passant, 1);
	print_board();
	switch_sides();
}

void Nano_Chess::computer_move() {
	my_puts("\nperform computer move\n");
	next(0, 0, 0, 21, can_en_passant, MAX_LEVEL);
	next(0, 0, 0, 21, can_en_passant, 1);
	print_board();
	switch_sides();
}


