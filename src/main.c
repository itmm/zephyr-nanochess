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

const char* l =
	// initial board
	"ustvrtsu"
	"qqqqqqqq"
	"yyyyyyyy"
	"}{|~z|{}"

	// points by piece; points for center; piece letters; movements
		  "   76Lsabcddcba .pknbrq  PKNBRQ ?A6J57IKJT576,+-48HLSU";

int B, i, y, u, b, I[411], * G = I, x = 10, z = 15, M = 1e4;

int X(int w, int c, int h, int e, int S, int s) {
	int t, o, L, E, d, O = e, N = -M * M, K = 78 - h << x, p, * g, n, * m, A, q, r, C, J, a = y ? -x : x;
	y ^= 8;
	G++;
	d = w || s && s >= h && X(0, 0, 0, 21, 0, 0) > M;
	do { ;
		if ((o = I[p = O])) {
			q = o & z ^ y;
			if (q < 7) {
				A = q-- & 2 ? 8 : 4;
				C = o - 9 & z ? q["& .$  "] : 42;
				do {
					r = I[p += C[l] - 64];
					if (!w | p == w) {
						g = q | p + a - S ? 0 : I + S;
						if (!r & (q | A < 3 || g) || (r + 1 & z ^ y) > 9 && q | A > 2) { ;
							if (m = !(r - 2 & 7)) { return --G, y ^= 8, G[1] = O, K; }
							J = n = o & z;
							E = I[p - a] & z;
							t = q | E - 7 ? n : (n += 2, 6 ^ y);
							while (n <= t) {
								L = r ? l[r & 7] * 9 - 189 - h - q : 0;
								if (s) {
									L += (
											 1 - q ? l[p / x + 5] - l[O / x + 5] + l[p % x + 6] * -~!q - l[O % x + 6] +
													 o / 16 * 8 : !!m * 9
										 ) + (
											 q ? 0 : !(I[p - 1] ^ n) + !(I[p + 1] ^ n) + l[n & 7] * 9 - 386 + !!g * 99 +
													 (A < 2)
										 ) + !(E ^ y ^ 9);
								}
								if (s > h || 1 < s & s == h && L > z | d) {
									p[I] = n, O[I] = m ? *g = *m, *m = 0 : g ? *g = 0 : 0;
									L -= X(s > h | d ? 0 : p, L - N, h + 1, G[1], J = q | A > 1 ? 0 : p, s);
									if (!(
										h || s - 1 | B - O | i - n | p - b | L < -M
									)) {
										return --G, y ^= 8, y ^= 8, u = J;
									}
									J = q - 1 | A < 7 || m || !s | d | r | o < z || X(0, 0, 0, 21, 0, 0) > M;
									O[I] = o;
									p[I] = r;
									m ? *m = *g, *g = 0 : g ? *g = 9 ^ y : 0;
								};
								if (L > N) {
									*G = O;
									if (s > 1) { ;
										if (h && c - L < 0) { return --G, y ^= 8, L; }
										if (!h) { i = n, B = O, b = p; }
									}
									N = L;
								}
								n += J || (g = I + p, m = p < O ? g - 3 : g + 2, *m < z | m[O - p] || I[p += p - O]);
							}
						}
					}
				} while (!r & q > 2 || (
					p = O, q | A > 2 | o > z & !r && ++C * --A
				));
			}
		}
	} while (++O > 98 ? O = 20 : e - O);
	return --G, y ^= 8, N + M * M && N > -K + 1924 | d ? N : 0;
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
int main() {
	#if !defined(CONFIG_ARCH_POSIX)
		console_init();
	#endif
	while (++B < 121) { *G++ = B / x % x < 2 | B % x < 2 ? 7 : B / x & 4 ? 0 : *l++ & 31; }
	while (B = 19) {
		while (B++ < 99) { my_putc(B % x ? l[B[I] | 16] : x); }
		if (x - (B = my_getc() & z)) {
			i = I[B += (x - my_getc() & z) * x] & z;
			b = my_getc() & z;
			b += (x - my_getc() & z) * x;
			while (x - (*G = my_getc() & z)) { i = *G ^ 8 ^ y; }
		} else { X(0, 0, 0, 21, u, 5); }
		X(0, 0, 0, 21, u, 1);
	}
}