
#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace std;

void print_map(int n, int m, int** map) {
    printf("%d %d\n", n, m);
    for (int i = 0; i < n; i++) {
        for(int j = 0; j < m; j ++) {
            printf("%d", map[i][j]);
        }
        printf("\n");
    }
}

void print_solution(int n, int m, int* x) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (x[i * m + j]) {
                printf("(%d, %d)\n", i + 1, j + 1);
            }
        }
    }
}

void load_map(int& n, int& m, int**& map) {
    scanf("%d%d", &n, &m);
    map = (int**)malloc(sizeof(int*) * n);
    for (int i = 0; i < n; i++) {
        map[i] = (int*)malloc(sizeof(int) * m);
        for(int j = 0; j < m; j ++) {
            scanf("%d", &(map[i][j]));
        }
    }
}

int ones(int v) {
	if (!v) {
		return 0;
	} else {
		return ones(v & (v - 1)) + 1;
	}
}

//                        press[i - 1][j]
//   press[i][j - 1]      press[i][j]          press[i][j + 1]
//                        press[i + 1][j]
//
//   equation:
//       (press[i - 1][j] + press[i][j - 1] + press[i][j] + press[i][j + 1] + press[i + 1][j]) = state[i][j] (mod 2)
//
//   map press[i][j] to v.bit[i * m + j]
//   all combination of press[i][j] can be represented in (0,0,...,0) to (1,1,...,1). v in [0, 1 << (n*m) - 1]
bool solve_map(int n, int m, int** map, int*& x) {
    int N = n * m;
    int best_v = -1;
 
	for (int v = 0; v < (1<<N); v++) {
		// check solution
		bool is_valid = true;
	    for (int i = 0; is_valid && (i < n); i++) {
	        for (int j = 0; is_valid && (j < m); j++) {
	        	int flag = 0;
	            {
	                // (i, j)
	                int index = i * m + j;
	                flag += (v & (1 << index)) ? 1 : 0;
	            }
	            if (i > 0) {
	                // (i-1, j)
	                int index = (i - 1) * m + j;
	                flag += (v & (1 << index)) ? 1 : 0;
	            }
	            if (i + 1 < n) {
	                // (i+1, j)
	                int index = (i + 1) * m + j;
	                flag += (v & (1 << index)) ? 1 : 0;
	            }
	            if (j > 0) {
	                // (i, j - 1)
	                int index = i * m + (j - 1);
	                flag += (v & (1 << index)) ? 1 : 0;
	            }
	            if (j + 1 < m) {
	                // (i, j + 1)
	                int index = i * m + (j + 1);
	                flag += (v & (1 << index)) ? 1 : 0;
	            }
	            if ((flag % 2) != map[i][j]) {
	            	is_valid = false;
	            }
	        }
	    }

		// find one valid solution
		if (is_valid) {
			if (best_v == -1) {
				best_v = v;
			} else if (ones(best_v) > ones(v)) {
				best_v = v;
			}
		}
	}
	if (best_v >= 0) {
	    x = (int*)malloc(sizeof(int) * N);
		for (int i = 0; i < N; i++) {
		    x[i] = (best_v & (1 << i));
		}
		return true;
	} else{
		return false;
	}
}

int main() {
    int n, m;
    int** map;
    load_map(n, m, map);
    // print_map(n, m, map);

    int* x;
    if (solve_map(n, m, map, x)) {
        print_solution(n, m, x);
    } else {
        printf("No solution!\n");
	}
    return 0;
}
