#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;
typedef pair<int, int> pii;

int L, N, Q;
int mat[41][41]; //0빈칸, 1함정, 2벽
int Kmat[41][41]; //기사 번호 저장

struct Knight {
	int r, c, h, w, k;
	bool alive = true;
};
vector<Knight> knight(31);

int initialK[31]; //초기 체력 저장

int dx[4] = {-1,0,1,0}; //상우하좌
int dy[4] = {0,1,0,-1};

vector<int> willMove;

void init() {
	cin >> L >> N >> Q;

	for (int i = 1; i <= L; i++) {
		for (int j = 1; j <= L; j++) {
			cin >> mat[i][j];
		}
	}

	for (int i = 1; i <= N; i++) {
		cin >> knight[i].r >> knight[i].c >> knight[i].h >> knight[i].w >> knight[i].k;
		initialK[i] = knight[i].k;

		for (int x = knight[i].r; x < knight[i].r + knight[i].h; x++) {
			for (int y = knight[i].c; y < knight[i].c + knight[i].w; y++) {
				Kmat[x][y] = i;
			}
		}
	}
}

bool canMove(int i, int d) {

	if (knight[i].alive == false) return true; //사라진 기사는 무시

	bool canGo = true;

	if (d == 0) { //상
		int r = knight[i].r + dx[0];
		for (int c = knight[i].c; c < knight[i].c + knight[i].w; c++) {
			if (mat[r][c] == 2 || !(1 <= r && r <= L && 1 <= c && c <= L)) { //벽을 만나면
				canGo = false;
				break;
			}
			if (Kmat[r][c]) { //다른 기사를 만나면
				canGo &= canMove(Kmat[r][c], d);
			}
		}
	}
	else if (d == 1) { //우
		int c = knight[i].c + knight[i].w - 1 + dy[1];
		for (int r = knight[i].r; r < knight[i].r + knight[i].h; r++) {
			if (mat[r][c] == 2 || !(1 <= r && r <= L && 1 <= c && c <= L)) { //벽을 만나면
				canGo = false;
				break;
			}
			if (Kmat[r][c]) { //다른 기사를 만나면
				canGo &= canMove(Kmat[r][c], d);
			}
		}
	}
	else if (d == 2) { //하
		int r = knight[i].r + knight[i].h-1 + dx[2];
		for (int c = knight[i].c; c < knight[i].c + knight[i].w; c++) {
			if (mat[r][c] == 2 || !(1 <= r && r <= L && 1 <= c && c <= L)) { //벽을 만나면
				canGo = false;
				break;
			}
			if (Kmat[r][c]) { //다른 기사를 만나면
				canGo &= canMove(Kmat[r][c], d);
			}
		}
	}
	else if (d == 3) { //좌
		int c = knight[i].c + dy[3];
		for (int r = knight[i].r; r < knight[i].r + knight[i].h; r++) {
			if (mat[r][c] == 2 || !(1 <= r && r <= L && 1 <= c && c <= L)) { //벽을 만나면
				canGo = false;
				break;
			}
			if (Kmat[r][c]) { //다른 기사를 만나면
				canGo &= canMove(Kmat[r][c], d);
			}
		}
	}

	if (canGo) {
		willMove.push_back(i); 
		willMove.erase(unique(willMove.begin(), willMove.end()), willMove.end());
	}
	return canGo;
}

void move(int d) {
	//willMove에 있는 기사들을 d방향으로 한칸씩 밀기
	for (int i = 0; i < willMove.size(); i++) {
		int num = willMove[i];

		for (int x = knight[num].r; x < knight[num].r + knight[num].h; x++) {
			for (int y = knight[num].c; y < knight[num].c + knight[num].w; y++) {
				Kmat[x][y] = 0;
			}
		}
		knight[num].r += dx[d];
		knight[num].c += dy[d];
		for (int x = knight[num].r; x < knight[num].r + knight[num].h; x++) {
			for (int y = knight[num].c; y < knight[num].c + knight[num].w; y++) {
				Kmat[x][y] = num;
			}
		}
	}
}

void demage() {
	//willMove에 있는 기사들 중 마지막 기사를 제외하고 데미지 입힘
	//체력0이하면 체스판에서 사라지고 alive=false로 세팅
	for (int w = 0; w < willMove.size()-1; w++) {
		int i = willMove[w];
		int sum = 0; //함정 개수
		for (int x = knight[i].r; x < knight[i].r + knight[i].h; x++) {
			for (int y = knight[i].c; y < knight[i].c + knight[i].w; y++) {
				if (mat[x][y] == 1) sum++;
			}
		}
		knight[i].k -= sum;
		if (knight[i].k <= 0) { //현재 체력 이상의 대미지를 받으면 체스판에서 사라짐
			knight[i].alive = false;
			for (int x = knight[i].r; x < knight[i].r + knight[i].h; x++) {
				for (int y = knight[i].c; y < knight[i].c + knight[i].w; y++) {
					Kmat[x][y] = 0;
				}
			}
		}
	}
}

int sumOfAliveDemage() {
	int sum = 0;
	for (int i = 1; i <= N; i++) {
		if (knight[i].alive) {
			sum += initialK[i] - knight[i].k;
		}
	}
	return sum;
}

void printKmat() {
	for (int i = 1; i <= L; i++) {
		for (int j = 1; j <= L; j++) {
			cout << Kmat[i][j] << " ";
		}
		cout << '\n';
	}
}
int main() {

	init();

	for (int q = 0; q < Q; q++) {
		int i, d; //i번 기사를 방향 d로 한 칸 이동
		cin >> i >> d;

		willMove.clear();
		if (canMove(i, d)) {
			move(d); 
			demage();
		}
		//printKmat();
	}
	//printKmat();
	cout << sumOfAliveDemage();

	return 0;
}