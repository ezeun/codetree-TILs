#include <iostream>
#include <vector>
#include <queue>
using namespace std;
typedef pair<int, int> pii;

int n, m, k; //격자의 크기 n, 팀의 개수 m, 라운드 수 k
int mat[21][21]; //0: 빈칸, 1: 머리사람, 2: 중간사람, 3: 꼬리사람, 4: 이동 선

struct Team {
	int hx, hy; //머리사람 위치
	int tx, ty; //꼬리사람 위치
};
vector<Team> team;

int score = 0;

int dx[4] = { 0,0,-1,1 };
int dy[4] = { -1,1,0,0 };

void init() {
	cin >> n >> m >> k;

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			cin >> mat[i][j];
			if (mat[i][j] == 1) { //머리사람을 발견하면 team에 저장
				Team t;
				t.hx = i;
				t.hy = j;
				team.push_back(t);
			}
		}
	}

	//team의 머리사람과 같은 팀인 꼬리사람 정보 저장
	//머리사람에서 bfs를 돌림
	for (int i = 0; i < m; i++) {
		//team[i].tx와 team[i].ty를 저장하자
		queue<pii> q;
		int vis[21][21] = { 0, };

		q.push({ team[i].hx, team[i].hy }); //머리사람을 시작으로 bfs돌림
		vis[team[i].hx][team[i].hy] = 1;

		while (!q.empty()) {
			pii cur = q.front(); 
			q.pop();

			if (mat[cur.first][cur.second] == 3) {
				team[i].tx = cur.first;
				team[i].ty = cur.second;
				break;
			}

			for (int d = 0; d < 4; d++) {
				int nx = cur.first + dx[d];
				int ny = cur.second + dy[d];

				if (nx < 0 || nx >= n || ny < 0 || ny >= n) continue;
				if (vis[nx][ny]) continue;
				if (mat[nx][ny] == 0 || mat[nx][ny] == 4) continue;

				q.push({ nx, ny });
				vis[nx][ny] = 1;

			}
		}
	}
}

void move() { //각 팀은 머리사람을 따라 한 칸 이동
	for (int i = 0; i < m; i++) {

		/* 머리사람과 꼬리사람만 이동하면 됨
		   ... 4 3 2 2 1 4 ... = > ... 4 4 3 2 2 1 ...
		   머리사람1은 2로 변경 후 4인 곳으로 이동
		   꼬리사람3은 4로 변경 후 2인 곳으로 이동*/

		//격자랑 team 둘다 갱신!!

		//꼬리먼저 이동하자.. 머리한테 먹히는 경우 제대로 동작안함 //수정1

		//꼬리사람 이동
		for (int d = 0; d < 4; d++) {
			int nx = team[i].tx + dx[d];
			int ny = team[i].ty + dy[d];

			if (nx < 0 || nx >= n || ny < 0 || ny >= n) continue;
			if (mat[nx][ny] == 2) {
				mat[team[i].tx][team[i].ty] = 4;
				mat[nx][ny] = 3;
				team[i].tx = nx;
				team[i].ty = ny;
				break;
			}
		}

		//머리사람 이동
		for (int d = 0; d < 4; d++) {
			int nx = team[i].hx + dx[d];
			int ny = team[i].hy + dy[d];

			if (nx < 0 || nx >= n || ny < 0 || ny >= n) continue;
			if (mat[nx][ny] == 4) {
				mat[team[i].hx][team[i].hy] = 2;
				mat[nx][ny] = 1;
				team[i].hx = nx;
				team[i].hy = ny;
				break;
			}
		}
	}
}

pii ball(int round) { //공이 정해진 선을 따라 던져짐. 공에 맞은 사람의 위치 반환, 없으면 (-1, -1)반환
	
	pii ret = { -1, -1 };
					 
    /*
	좌->우 : ((round/n)%4)==0
	하->상 : ((round/n)%4)==1
	우->좌 : ((round/n)%4)==2
	상->하 : ((round/n)%4)==3

	방향 정해진 후 행/열의 idx는 n으로 나눈 나머지(방향에 따라 idx 시작위치 잘 보기)
	*/

	if (((round / n) %4) == 0) { 
		int row = round % n;
		for (int col = 0; col < n; col++) {
			if (mat[row][col] != 0 && mat[row][col] != 4) {
				ret.first = row;
				ret.second = col;
				break;
			}
		}
	}
	else if (((round / n) % 4) == 1) {
		int col = round % n;
		for (int row = n - 1; row >= 0; row--) {
			if (mat[row][col] != 0 && mat[row][col] != 4) {
				ret.first = row;
				ret.second = col;
				break;
			}
		}
	}
	else if (((round / n) % 4) == 2) {
		int row = n - 1 - (round % n);
		for (int col = n - 1; col >= 0; col--) {
			if (mat[row][col] != 0 && mat[row][col] != 4) {
				ret.first = row;
				ret.second = col;
				break;
			}
		}
	}
	else if (((round / n) % 4) == 3) {
		int col = n - 1 - (round % n);
		for (int row = 0; row < n; row++) {
			if (mat[row][col] != 0 && mat[row][col] != 4) {
				ret.first = row;
				ret.second = col;
				break;
			}
		}
	}

	return ret;
}

void getScore(pii personHitByBall) { //머리사람을 시작으로 k번째 사람이면 k의 제곱만큼 점수 얻음

	//공 맞은 사람 팀의 머리사람 찾기(거리 구하기_시작점 거리가 1)
	pii head;

	queue<pii> q;
	int vis[21][21] = { 0, };

	q.push({ personHitByBall.first, personHitByBall.second});
	vis[personHitByBall.first][personHitByBall.second] = 1;

	while (!q.empty()) {
		pii cur = q.front();
		q.pop();

		if (mat[cur.first][cur.second] == 1) {
			head.first = cur.first;
			head.second = cur.second;
			//거리의 제곱만큼 점수 획득
			score += vis[head.first][head.second] * vis[head.first][head.second];
			break;
		}

		for (int d = 0; d < 4; d++) {
			int nx = cur.first + dx[d];
			int ny = cur.second + dy[d];

			if (nx < 0 || nx >= n || ny < 0 || ny >= n) continue;
			if (vis[nx][ny]) continue;
			if (mat[nx][ny] == 0 || mat[nx][ny] == 3 || mat[nx][ny] == 4) continue; //수정2(==3 추가)
			if (mat[cur.first][cur.second] == 3 && mat[nx][ny] == 1) continue; //수정3

			q.push({ nx, ny });
			vis[nx][ny] = vis[cur.first][cur.second]+1;
		}
	}

	//점수 획득한 팀은 방향을 바꿈
	for (int i = 0; i < m; i++) {
		if (team[i].hx == head.first && team[i].hy == head.second) {
			//격자 갱신
			mat[team[i].hx][team[i].hy] = 3;
			mat[team[i].tx][team[i].ty] = 1;

			//team 머리,꼬리 위치 갱신
			int tmpX = team[i].hx;
			int tmpY = team[i].hy;
			team[i].hx = team[i].tx;
			team[i].hy = team[i].ty;
			team[i].tx = tmpX;
			team[i].ty = tmpY;
			break;
		}
	}
}


void printMat() {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			cout << mat[i][j]<<" ";
		}
		cout << '\n';
	}
}
void printTeams(){
	for (int i = 0; i < m; i++) {
		cout << team[i].hx << " " << team[i].hy << " " << team[i].tx << " " << team[i].ty<<'\n';
	}
}
int main() {

	init(); //printTeams();

	for (int round = 0; round < k; round++) {

		move(); //printMat();

		pii personHitByBall = ball(round); //공에 맞은 사람의 위치 반환, 맞은 사람 없으면 (-1, -1) 반환

		if (!(personHitByBall.first == -1 && personHitByBall.second == -1)) { //공에 맞은 사람 있으면
			getScore(personHitByBall);
		}
	}

	cout << score;

	return 0;
}