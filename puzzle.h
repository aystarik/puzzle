// 2 11 15 5 13 4 6 7 12 8 10 1 9 3 14 0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const unsigned S = 4;
const unsigned S2 = S*S;
enum blank_dir {
	UNDEF = 0,
	UP = 1,
	DOWN,
	LEFT,
	RIGHT,
};

struct cost_matrix {
    unsigned h[S2][S2];
	cost_matrix() {
        for (unsigned n = 0; n < S2; ++n) {
			h[n][0] = 0;
            for (unsigned m = 1; m < S2; ++m) {
				int dy = n / S - m / S;
				int dx = n % S - m % S;
				h[n][m] = abs(dx) + abs(dy);
			}
		}
	}
    int operator()(int i, int j) const {return h[i][j];}
} cm;

//const int ex[] = {2, 11, 15, 5, 13, 4, 6, 7, 12, 8, 10, 1, 9, 3, 14, 0};
//const int ex[] = {1, 2, 3, 0, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
const int ex[] = {4, 5, 7, 2, 9, 14, 12, 13, 0, 3, 6, 11, 8, 1, 15, 10};

struct node {
    unsigned short cell[S2];
	node const *parent;
	blank_dir dir;
	int cst;
	node() : parent(0), dir(UNDEF), cst(0) {}

	void read() {
        int c = 1, n, bits = 0;
        for (int i = 0; i < S2; ++i) {
            //c = scanf("%d", &n);
            n = ex[i];
            if (c != 1 || n < 0 || n >= S2)
				exit(1);
			if (bits & (1 << n))
				exit(1);
			bits &= 1 << n;
			cell[i] = n;
			cst += cm(i, n);
		}
	}

    bool move_blocked(blank_dir d) const {
		int x, y;
        for (unsigned i = 0; i < S2; ++i) {
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
                return true;
            return false;
		}
        return true;
	}
    void move_blank(const node &n, blank_dir d) {
		int i = 0, j = 0;

		parent = &n;
        for (; j < S2; ++j) {
			cell[j] = n.cell[j];
            if (!cell[j])
                i = j;
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
    int cost() const {return cst;}
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
            printf("Nodes created: %ld\n", count);
		}
	}
    int dfs(node &nd, unsigned depth) {
        blank_dir dirs[4] = {UNDEF, UNDEF, UNDEF, UNDEF};
        if (nd.cost() + depth > bound) {
            return depth + nd.cost();
        }
        if (!nd.cost()) {
            return nd.cost();
        }
        unsigned j = 0;
        if (deep_path.length > depth && use_deep_path) {
            dirs[j++] = deep_path.dir[depth];
        }
        for (unsigned i = UP; i <= RIGHT ; ++i) {
            if (dirs[0] == i || nd.move_blocked((blank_dir)i))
                continue;
            dirs[j++] = (blank_dir)i;
        }
        node nxt;
        int ret = 1000;
        int h = nd.cost();
        for (unsigned i = 0; i < j; ++i) {
            nxt.move_blank(nd, dirs[i]);
            ++count;
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
        return ret;
    }
};


