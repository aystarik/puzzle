// 2 11 15 5 13 4 6 7 12 8 10 1 9 3 14 0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int S = 4;
enum blank_dir {
	UNDEF = 0,
	UP = 1,
	DOWN,
	LEFT,
	RIGHT,
};

struct cost_matrix {
	unsigned short h[S*S][S*S];
	cost_matrix() {
		for (int n = 0; n < S*S; ++n) {
			h[n][0] = 0;
			for (int m = 1; m < S*S; ++m) {
				int dy = n / S - m / S;
				int dx = n % S - m % S;
				h[n][m] = abs(dx) + abs(dy);
			}
		}
	}
	int operator()(int i, int j) {return h[i][j];}
} cm;

struct node {
	unsigned short cell[S*S];
	node const *parent;
	blank_dir dir;
	int cst;
	node() : parent(0), dir(UNDEF), cst(0) {}

	void read() {
		int c, n, bits = 0;
		for (int i = 0; i < S*S; ++i) {
			c = scanf("%d", &n);
			if (c != 1 || n < 0 || n >= S*S)
				exit(1);
			if (bits & (1 << n))
				exit(1);
			bits &= 1 << n;
			cell[i] = n;
			cst += cm(i, n);
		}
	}

	int move_blocked(int d) {
		int x, y;
		for (int i = 0; i < S*S; ++i) {
			if (cell[i])
				continue;
			x = i % S;
			y = i / S;
		
			switch (d) {
			case UP:
				--y;
				break;
			case DOWN:
				++y;
				break;
			case LEFT:
				--x;
				break;
			case RIGHT:
				++x;
				break;
			default:;
			}
			if (x < 0 || x >= S || y < 0 || y >= S)
				return 1;
			return 0;
		}
		return 1;
	}
	void move_blank(const node &n, int d) {
		int i = 0, j = 0;

		parent = &n;
		for (; j < S*S; ++j) {
			cell[j] = n.cell[j];
			if (!cell[j]) i = j;
		}

		j = i;

		switch (d) {
		case UP:
			j -= S; dir = UP;
			break;
		case DOWN:
			j += S; dir = DOWN;
			break;
		case LEFT:
			--j; dir = LEFT;
			break;
		case RIGHT:
			++j; dir = RIGHT;
			break;
		default:;
		}
		cell[i] = cell[j];
		cell[j] = 0;
		cst = n.cst + cm(i, cell[i]) - cm(j, cell[i]);
	}
	int cost() {return cst;}
};

struct path {
	blank_dir dir[80];
	unsigned int length;
	path() : length(0) {}
	void update(int depth, const node &n) {
		length = depth + 1;
		node const *t = &n;
		for (; depth >= 0; --depth) {
			if (!t)
				break;
			dir[depth] = t->dir;
			t = t->parent;
		}
	}
	void print() {
		for (unsigned i = 0; i < length; ++i) {
			switch(dir[i]) {
			case UP:
				printf(" UP");
				break;
			case DOWN:
				printf(" DOWN");
				break;
			case LEFT:
				printf(" LEFT");
				break;
			case RIGHT:
				printf(" RIGHT");
				break;
			default:;
			}
		}
		printf(": %d steps\n", length);
	}
};

struct solver {
	int bound;
	node start;
	path deep_path;
	bool use_deep_path;
	unsigned long count;
	void iterate() {
		bound = start.cost();
		count = 0;
		while (bound) {
			bound = dfs(start, 0);
			use_deep_path = true;
			deep_path.print();
			printf("Nodes created: %d\n", count);
		}
	}
	int dfs(node &nd, unsigned depth);
};

int solver::dfs(node &nd, unsigned depth) {
	int h = nd.cost();
	int ret = 1000;
	int dirs[4];
	node nxt;
	int j = 0;
	if (h + depth > bound) {
		ret = depth + h;
		goto end;
	}
	if (!h) {
		ret = h;
		goto end;
	}
	if (deep_path.length > depth && use_deep_path) {
		dirs[j++] = deep_path.dir[depth];
	}
	for (int i = UP; i <= RIGHT ; ++i) {
		if (dirs[0] == i || nd.move_blocked(i))
			continue;
		dirs[j++] = i;
	}
	for (int i = 0; i < j; ++i) {
		nxt.move_blank(nd, dirs[i]);
		count++;
		h = dfs(nxt, depth + 1);
		if (h < ret) {
			ret = h;
			if (deep_path.length < depth ||
				(!ret && (deep_path.length == depth))) {
				use_deep_path = false;
				deep_path.update(depth, nxt);
			}
		}
	}
	end:
	return ret;
}

int main() {
	solver sl;
	sl.start.read();
	sl.iterate();
	return 0;
}
