#include <iostream>
#include <vector>
#include <queue>
using namespace std;

int n, m, h, k; //n은 홀수

struct Tagger {
	int x, y;
	int d;
};
Tagger tagger; //술래

struct Runner {
	int x, y;
	int d;
};
queue<Runner> runner; //도망자

int dx[4] = {-1, 0, 1, 0};
int dy[4] = {0, 1, 0, -1};

int tree[100][100];

int taggerWay[100][100]; //술래가 가는 칸 순서 저장(중앙부터 달팽이 방향으로 1~n^2)
/*
25 10 11 12 13
24  9  2  3 14
23  8  1  4 15
22  7  6  5 16
21 20 19 18 17
*/
int goLarger = true; //술래가 중심에서 (1,1)로 가고있으면 true(문제에서 빨간 화살표방향) 

int score = 0;

void init() {
	cin >> n >> m >> h >> k;

	tagger.x = n / 2 + 1;
	tagger.y = n / 2 + 1;
	tagger.d = 0;

	for (int i = 0; i < m; i++) {
		Runner r;
		cin >> r.x >> r.y >> r.d;
		runner.push(r);
	}

	for (int i = 0; i < h; i++) {
		int x, y; cin >> x >> y;
		tree[x][y] = 1;
	}

	//taggerWay 채우기
	taggerWay[tagger.x][tagger.y] = 1;
	int len = 0;
	int cx = tagger.x;
	int cy = tagger.y;
	while (true) { //(1,1) 도착시 종료
		for (int d = 0; d < 4; d++) {
			if (d == 0) len++;
			if (d == 2) len++;
			for (int l = 0; l < len; l++) {
				int tmp = taggerWay[cx][cy];
				cx += dx[d];
				cy += dy[d];
				taggerWay[cx][cy] = tmp + 1; 
				if (cx <= 1 && cy <= 1) break;
			}
			if (cx <= 1 && cy <= 1) break;
		}
		if (cx <= 1 && cy <= 1) break;
	}
}

void runnerMove() {
	int rSize = runner.size();
	for (int i = 0; i < rSize; i++) {
		Runner r = runner.front();
		runner.pop();
		
		int nx = r.x + dx[r.d];
		int ny = r.y + dy[r.d];

		if (abs(r.x - tagger.x) + abs(r.y - tagger.y) <= 3) { //술래와의 거리가 3 이하인 도망자만 움직임
			if (1 <= nx && nx <= n && 1 <= ny && ny <= n) { //현재 방향으로 1칸 움직일 때 격자 안벗어나면,
				if (!(nx == tagger.x && ny == tagger.y)) { //움직이려는 칸에 술래 없다면 이동
					r.x = nx;
					r.y = ny;
					runner.push(r);
					continue;
				}
			}
			else { //현재 방향으로 1칸 움직일 때 격자 벗어나면,
				r.d = (r.d + 2) % 4; //방향을 반대로 틂
				nx = r.x + dx[r.d];
				ny = r.y + dy[r.d];
				if (!(nx == tagger.x && ny == tagger.y)) {
					r.x = nx;
					r.y = ny;
					runner.push(r);
					continue;
				}
			}
		}
		runner.push(r);
	}
}

void taggerMove() {
/*
25 10 11 12 13
24  9  2  3 14
23  8  1  4 15
22  7  6  5 16
21 20 19 18 17
*/
	//술래 이동
	tagger.x += dx[tagger.d];
	tagger.y += dy[tagger.d];

	if (goLarger) { //바깥쪽으로 이동(taggerWay숫자 커지는 방향)		

		//이동후의 위치가 이동방향 틀어지는 지점이면 방향을 바로 틀어줌
		for (int d = 0; d < 4; d++) {
			if ((taggerWay[tagger.x + dx[d]][tagger.y + dy[d]]) == (taggerWay[tagger.x][tagger.y] + 1)) {
				tagger.d = d;
				break;
			}
		}
	}
	else { //안쪽으로 이동(taggerWay숫자 작아지는 방향)
		//이동후의 위치가 이동방향 틀어지는 지점이면 방향을 바로 틀어줌
		for (int d = 0; d < 4; d++) {
			if ((taggerWay[tagger.x + dx[d]][tagger.y + dy[d]]) == (taggerWay[tagger.x][tagger.y] - 1)) {
				tagger.d = d;
				break;
			}
		}
	}

	//끝점일 때 방향 틀기
	if (tagger.x == 1 && tagger.y == 1) {
		tagger.d = 2;
		goLarger = false;
	}	
	if (tagger.x == (n / 2 + 1) && tagger.y == (n / 2 + 1)) {
		tagger.d = 0;
		goLarger = true;
	}
}

void catchRunner(int turn) {
	//범위체크
	int canCatch[100][100] = { 0, }; //술래가 잡을 수 있는 칸을 1로 체크
	Tagger tmp = tagger;
	for (int grid = 1; grid <= 3; grid++) {
		canCatch[tmp.x][tmp.y] = 1; 
		tmp.x += dx[tmp.d];
		tmp.y += dy[tmp.d];
		if (tmp.x<1 || tmp.x>n || tmp.y<1 || tmp.y>n) break;
	}

	//도망자 잡기
	int rSize = runner.size();
	int sum = 0; //잡힌 도망자의 수

	for (int i = 0; i < rSize; i++) {
		Runner tmp = runner.front();
		runner.pop();

		if (tree[tmp.x][tmp.y] || canCatch[tmp.x][tmp.y]==0) {
			runner.push(tmp);
		}
		else {
			sum++;
		}
	}
	score += turn * sum;
}

//출력테스트
void printRunner() {
	int rSize = runner.size();
	for (int i = 0; i < rSize; i++) {
		cout << runner.front().x << " " << runner.front().y << " " << runner.front().d<<'\n';
		runner.push(runner.front());
		runner.pop();
	}
}
void printTaggerWay() {
	for (int i = 1; i <= n; i++) {
		for (int j = 1; j <= n; j++) {
			cout << taggerWay[i][j] << " ";
		}cout << '\n';
	}
}
void printTagger() {
	cout << "tagger "<<tagger.x << " " << tagger.y << " " << tagger.d << '\n';
}
int main() {

	init();

	for (int turn = 1; turn <= k; turn++) {

		runnerMove(); //printRunner();

		taggerMove(); //printTaggerWay(); 
					  //printTagger();

		catchRunner(turn);
	}

	cout << score;
	return 0;
}