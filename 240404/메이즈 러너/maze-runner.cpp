#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;
typedef pair<int, int> pii;
typedef pair<int, pii> ipii;

int N, M, K;
int mat[11][11]; //0: 빈칸, 1~9: 벽
vector<pii> player;
int exitR, exitC;
int dr[4] = { -1, 1, 0, 0 };
int dc[4] = { 0, 0, -1, 1 };
int sum = 0; //이동거리 합

void init() {
	cin >> N >> M >> K;

	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= N; j++) {
			cin >> mat[i][j];
		}
	}

	for (int i = 0; i < M; i++) {
		int r, c; cin >> r >> c;
		player.push_back({ r,c });
	}

	cin >> exitR >> exitC;
}

void move() {
	for (int p = 0; p < player.size(); p++) {
		int dist = abs(player[p].first - exitR) + abs(player[p].second - exitC);
		//상하 보다 좌우가 출구랑 더 가까워지면 그게 우선임. 거리 같을때만 상하가 우선!!!!
		int ndist = dist;
		for (int d = 0; d < 4; d++) {
			int nr = player[p].first + dr[d];
			int nc = player[p].second + dc[d];
			if (nr<1 || nr>N || nc<1 || nc>N) continue;
			if (mat[nr][nc] >= 1 && mat[nr][nc] <= 9) continue;
			if (abs(nr - exitR) + abs(nc - exitC) >= dist) continue;
			ndist = min(ndist, abs(nr - exitR) + abs(nc - exitC));
		}
		if (ndist == dist) continue; //움직일 수 없다면 가만히있음
		for (int d = 0; d < 4; d++) {
			int nr = player[p].first + dr[d];
			int nc = player[p].second + dc[d];
			if (nr<1 || nr>N || nc<1 || nc>N) continue;
			if (mat[nr][nc] >= 1 && mat[nr][nc] <= 9) continue;
			if (abs(nr - exitR) + abs(nc - exitC) != ndist) continue;
			player[p].first = nr;
			player[p].second = nc;
			sum++;
			break;
		}
	}
	//참가자가 출구에 있으면 삭제
	for (int p = 0; p < player.size(); p++) {
		if (player[p].first == exitR && player[p].second == exitC) {
			player.erase(player.begin() + p);
			p--; //이거 해줘야 함!!!
		}
	}
}

struct cmp {
	bool operator()(ipii a, ipii b) {
		if (a.first != b.first) return a.first > b.first;
		else if (a.second.first != b.second.first) return a.second.first > b.second.first;
		else return a.second.second > b.second.second;
	}
};

void rotate() {
	priority_queue<ipii, vector<ipii>, cmp> pq;

	//정사각형 잡기
	//각 플레이어마다 만든 정사각형 한변의 길이랑 좌상단(r,c)를 pq에 넣기
	for (int p = 0; p < player.size(); p++) {
		int pR = player[p].first;
		int pC = player[p].second;
		int len=0; //한 변의 길이
		int r=0, c=0; //좌상단 좌표

		//사람과 출구가 같은 행
		if (pR == exitR) {
			if (pC - exitC > 0) { //사람이 출구보다 오른쪽
				len = pC - exitC + 1;
				r = exitR - len + 1;
				while (r < 1) r++;
				c = exitC;
			}
			else { //사람이 출구보다 왼쪽
				len = exitC - pC + 1;
				r = pR - len + 1;
				while (r < 1) r++;
				c = pC;
			}
		}
		//사람과 출구가 같은 열
		else if (pC == exitC) {
			if (pR - exitR > 0) { //사람이 출구보다 아래쪽
				len = pR - exitR + 1;
				r = exitR;
				c = exitC - len + 1;
				while (c < 1) c++;
			}
			else { //사람이 출구보다 위쪽
				len = exitR - pR + 1;
				r = pR;
				c = pC - len + 1;
				while (c < 1) c++;
			}
		}
		//사람과 출구의 열과 행 둘다 다름
		else if(pR != exitR && pC != exitC){
			if (exitR - pR > 0) {
				if (exitC - pC > 0) { // 사람이 출구 왼쪽 위
					len = max(exitR - pR, exitC - pC) + 1;
					r = exitR - len + 1;
					c = exitC - len + 1;
					while (r < 1) r++;
					while (c < 1) c++;
				}
				else if (exitC - pC < 0) { //사람이 출구 오른쪽 위
					len = max(exitR - pR, pC - exitC) + 1;
					r = exitR - len + 1;
					c = pC - len + 1;
					while (r < 1) r++;
					while (c < 1) c++;
				}
			}
			else if (exitR - pR < 0) {
				if (exitC - pC > 0) { // 사람이 출구 왼쪽 아래
					len = max(pR - exitR, exitC - pC) + 1;
					r = pR - len + 1;
					c = exitC - len + 1;
					while (r < 1) r++;
					while (c < 1) c++;
				}
				else if (exitC - pC < 0) { //사람이 출구 오른쪽 아래
					len = max(pR - exitR, pC - exitC) + 1;
					r = pR - len + 1;
					c = pC - len + 1;
					while (r < 1) r++;
					while (c < 1) c++;
				}
			}
		}

		pq.push({ len, {r,c} });
//		cout << "pq push " << pR << " " << pC << " " << len << " " << r << " " << c << '\n';
	}

	//pq.top에 있는 정사각형을 시계방향 90도 회전시키기
	int len = pq.top().first;
	int r = pq.top().second.first;
	int c = pq.top().second.second;
//	cout << "len, r, c " << len << " " << r << " " << c << '\n';

	int tmp[11][11];
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= N; j++) {
			tmp[i][j] = mat[i][j];
		}
	}

	// (i, j) -> (r+(j-c), c + (r+len-1-i)) 이렇게 바뀜

	//출구 회전
	int tmpR = exitR;
	int tmpC = exitC;
	exitR = r + (tmpC - c);
	exitC = c + (r + len - 1 - tmpR);

	//사람 회전
	for (int p = 0; p < player.size(); p++) {
		if (player[p].first >= r && player[p].first < r + len && player[p].second >= c && player[p].second < c + len) {
			int pr = player[p].first;
			player[p].first = r + (player[p].second - c);
			player[p].second = c + (r + len - 1 - pr);
		}
	}

	//벽 회전 및 내구도 감소
	for (int i = r; i < r + len; i++) {
		for (int j = c; j < c + len; j++) {
			tmp[r + (j - c)][c + (r + len - 1 - i)] = max(0, mat[i][j] - 1);
		}
	}

	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= N; j++) {
			mat[i][j] = tmp[i][j];
		}
	}
}

void print() {
	cout << "exit " << exitR << " " << exitC << '\n';
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= N; j++) {
			cout << mat[i][j] << " ";
		} cout << '\n';
	}
	for (auto &p : player) {
		cout << p.first << " " << p.second << '\n';
	}
	cout << "\n";
}

int main() {

	init();

	for (int i = 1; i <= K; i++) {

//		cout << "K: " << i << '\n';
//		cout << "before move\n"; print();

		move();
		if (player.empty()) break;
//		cout << "after move\n"; print();

		rotate();
//		cout << "after rotate\n"; print();
	}

	cout << sum << '\n' << exitR << " " << exitC;

	return 0;
}