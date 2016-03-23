
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

void print_equation(int** w, int* b, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d * x[%2d] + ", w[i][j], j);
        }
        printf("%d = 0 (mod 2)\n", b[i]);
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

int ones(int v) {
	if (!v) {
		return 0;
	} else {
		return ones(v & (v - 1)) + 1;
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

// convert to equation:
//     w[0][0] * x[0]       + w[0][1] * x[1]       + ... + w[0][n*m - 1] * x[n*m - 1]       + b[0]       = 0 (mod 2)
//     w[1][0] * x[0]       + w[1][1] * x[1]       + ... + w[1][n*m - 1] * x[n*m - 1]       + b[1]       = 0 (mod 2)
//     ......
//     w[n*m - 1][0] * x[0] + w[n*m - 1][1] * x[1] + ... + w[n*m - 1][n*m - 1] * x[n*m - 1] + b[n*m - 1] = 0 (mod 2)
void map2equation(int n, int m, int** map, int**& w, int*& b, int& N) {
    N = n * m;
    w = (int**)malloc(sizeof(int*) * N);
    b = (int*)malloc(sizeof(int) * N);

    // Zero all w[][]
    for (int i = 0; i < N; i++) {
        w[i] = (int*)malloc(sizeof(int) * N);
        for (int j = 0; j < N; j++) {
            w[i][j] = 0;
        }
    }
    // Init
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            int index = i * m + j, index_neighbor;

            {
                // (i, j)
                w[index][index] = 1;
            }
            if (i > 0) {
                // (i-1, j)
                index_neighbor = (i - 1) * m + j;
                w[index][index_neighbor] = 1;
            }
            if (i + 1 < n) {
                // (i+1, j)
                index_neighbor = (i + 1) * m + j;
                w[index][index_neighbor] = 1;
            }
            if (j > 0) {
                // (i, j - 1)
                index_neighbor = i * m + (j - 1);
                w[index][index_neighbor] = 1;
            }
            if (j + 1 < m) {
                // (i, j + 1)
                index_neighbor = i * m + (j + 1);
                w[index][index_neighbor] = 1;
            }
            if (map[i][j]) {
                b[index] = 1;
            } else {
                b[index] = 0;
            }
        }
    }
}

bool search(int**& w, int*& b, int N, int*& result, int& result_ones, int*& candidate, int k) {
    if (k < 0) {
    	int ones = 0;
    	for (int i = 0; i < N; i++) {
    		if (candidate[i] == 1) {
    			ones++;
			}
		}
    	if (result_ones == -1 || ones < result_ones) {
    		result_ones = ones;
    		for (int i = 0; i < N; i++) {
    			result[i] = candidate[i];
			}
		}
        return true;
    }

    if (w[k][k] == 1) {
        if (b[k] == 0) {
            candidate[k] = 0;
            if (search(w, b, N, result, result_ones, candidate, k - 1)) {
                return true;
            }
        }else{
            candidate[k] = 1;
            vector<int> arr;
            arr.clear();
            for (int i = k - 1; i >= 0; i--) {
                if (w[i][k] == 1) {
                    b[i] = 1 - b[i];
                    arr.push_back(i);
                }
            }
            bool flag = search(w, b, N, result, result_ones, candidate, k - 1);
            for (size_t i = 0; i < arr.size(); i++) {
                b[arr[i]] = 1 - b[arr[i]];
            }
            return flag;
        }
    } else {
        if (b[k] == 1) {
            // w[k][k] = 0, b[k] = 1, no solution
            return false;
        } else {
        	bool flag = false;
        	
            // w[k][k] = b[k] = 0, x[k] = 0 or 1
            candidate[k] = 0;
            if (search(w, b, N, result, result_ones, candidate, k - 1)) {
                flag = true;
            }

            candidate[k] = 1;
            vector<int> arr;
            for (int i = k - 1; i >= 0; i--) {
                if (w[i][k] == 1) {
                    b[i] = 1 - b[i];
                    arr.push_back(i);
                }
            }
            if (search(w, b, N, result, result_ones, candidate, k - 1)) {
            	flag = true;
			}
            for (size_t i = 0; i < arr.size(); i++) {
                b[arr[i]] = 1 - b[arr[i]];
            }
            return flag;
        }
    }
}

bool solve_equation(int**& w, int*& b, int N, int*& result) {
    // 1st round eliminate - convert to top-right triangle
    for (int k = 0; k < N; k++) {
        // find pilot, w[pilot][k] = 1
        int pilot = -1;
        for (int i = k; i < N; i++) {
            if(w[i][k]) {
                pilot = i;
                break;
            }
        }
        if (pilot < 0) {
            continue;
        }

        // swap line pilot-th and k-th
        if (pilot > k) {
            for (int j = k; j < N; j++) {
                int tmp = w[pilot][j]; w[pilot][j] = w[k][j]; w[k][j] = tmp;
            }
            int tmp = b[pilot]; b[pilot] = b[k]; b[k] = tmp;
        }

        // eliminate
        for (int i = k + 1; i < N; i++) {
            if (w[i][k] == 0) {
                continue;
            }
            for (int j = k; j < N; j++) {
                w[i][j] = w[i][j] ^ w[k][j];
            }
            b[i] = b[i] ^ b[k];
        }
    }

    // find result
    result = (int*)malloc(sizeof(int) * N);
    int result_ones = -1;
    int* candidate = (int*)malloc(sizeof(int) * N);
    return search(w, b, N, result, result_ones, candidate, N - 1);
}

int main() {
    int n, m;
    int** map;
    load_map(n, m, map);
    //print_map(n, m, map);

    int** w;
    int* b;
    int N;
    map2equation(n, m, map, w, b, N);

    int* x;
    if (solve_equation(w, b, N, x))
        print_solution(n, m, x);
    else
        printf("No solution!\n");
    return 0;
}
