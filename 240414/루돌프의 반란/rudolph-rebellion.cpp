#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cmath>
using namespace std;
typedef pair<int, int> pii;

int N, M, P, C, D; //격자, 턴수, 산타 수, 루돌프 힘, 산타 힘
int Rr, Rc; //루돌프 위치

int mat[51][51]; //산타 번호 저장
struct Santa {
	int r, c; //위치
	int t=0; // 기절한 턴
	bool alive = true; //false면 탈락
	int s=0; // 점수
}santa[31];

int dx8[8] = {-1,-1,0,1,1,1,0,-1}; //8방향
int dy8[8] = {0,1,1,1,0,-1,-1,-1};
int dx[4] = {-1,0,1,0}; //상우하좌
int dy[4] = {0,1,0,-1};

void init() {
	cin >> N >> M >> P >> C >> D;
	cin >> Rr >> Rc;

	for (int i = 1; i <= P; i++) {
		int num, r, c;
		cin >> num >> r >> c;
		santa[num].r = r;
		santa[num].c = c;
		mat[r][c] = num;
	}
}

void interaction(int santaNum, int r, int c, int dr, int dc) {
	//santaNum이 mat[r][c]에 있었는데 dr,dc 방향으로 밀자
	santa[santaNum].r += dr;
	santa[santaNum].c += dc;
	if (r + dr<1 || r + dr>N || c + dc<1 || c + dc>N) { //게임판 밖으로 밀려나오게 된 산타는 게임 탈락
		santa[santaNum].alive = false;
	}
	else if (!mat[r + dr][c + dc]) { //밀려난 칸에 다른 산타가 없으면
		mat[r + dr][c + dc] = santaNum;
	}
	else { //밀려난 곳에 또 산타가 있으면 연쇄적으로 1칸씩 밀려나기 반복
		interaction(mat[r + dr][c + dc], r + dr, c + dc, dr, dc);
		mat[r + dr][c + dc] = santaNum;
	}
}

void pushed(int santaNum, int dr, int dc, int q) { //santaNum번 산타가 {dr,dc} 방향으로 q칸 밀려남
	//격자랑 산타배열 둘 다 밀기
	mat[santa[santaNum].r][santa[santaNum].c] = 0;
	santa[santaNum].r += dr * q;
	santa[santaNum].c += dc * q;

	if (santa[santaNum].r<1 || santa[santaNum].r>N || santa[santaNum].c<1 || santa[santaNum].c>N) { //밀려난 위치가 게임판 밖이라면 산타는 게임 탈락
		santa[santaNum].alive = false;
	}
	else if (mat[santa[santaNum].r][santa[santaNum].c]) { //밀려난 칸에 다른 산타가 있으면 상호작용 발생
		interaction(mat[santa[santaNum].r][santa[santaNum].c], santa[santaNum].r, santa[santaNum].c, dr, dc);
		mat[santa[santaNum].r][santa[santaNum].c] = santaNum; 
	}
	else { //밀려난 칸에 다른 산타없으면
		mat[santa[santaNum].r][santa[santaNum].c] = santaNum;
	}
}

bool cmp(const pii &a, const pii &b) {
	if (a.first != b.first) return a.first < b.first;
	else if (santa[a.second].r != santa[b.second].r) return santa[a.second].r > santa[b.second].r;
	else return santa[a.second].c > santa[b.second].c;
}
void Rmove(int turn) { 

//1. 루돌프 움직임
	//탈락하지 않은 산타 중 가장 가까운 산타 찾기
	vector<pii> nearSanta; //{거리, 산타번호}
	for (int i = 1; i <= P; i++) {
		if (!santa[i].alive) continue;
		int dist = pow(Rr-santa[i].r, 2) + pow(Rc-santa[i].c, 2);
		nearSanta.push_back({dist,i});
	}
	sort(nearSanta.begin(), nearSanta.end(), cmp);

	//가장 가까운 산타의 위치
	int Sr = santa[nearSanta[0].second].r;
	int Sc = santa[nearSanta[0].second].c; 

	//가장 가까워지는 방향 찾기
	int nearestDist=5001, nearestDir;
	for (int d = 0; d < 8; d++) {
		int rr = Rr + dx8[d];
		int rc = Rc + dy8[d];
		if (rr<1||rr>N || rc<1 || rc>N) continue;
		int dist = pow(rr - Sr, 2) + pow(rc - Sc, 2);
		if (dist < nearestDist) {
			nearestDist = dist,
				nearestDir = d;
		}
	}
	//가장 가까워지는 방향으로 한 칸 돌진
	Rr += dx8[nearestDir];
	Rc += dy8[nearestDir];

//3-1. 루돌프가 움직여서 산타와 충돌
	if (mat[Rr][Rc]) {
		santa[mat[Rr][Rc]].s += C; 
		santa[mat[Rr][Rc]].t = turn; //산타는 루돌프와 충돌 후 기절함
		pushed(mat[Rr][Rc], dx8[nearestDir], dy8[nearestDir], C); //산타는 루돌프가 이동해온 방향으로 C칸 밀려남
	}
}

void Smove(int turn) {
//2. 1~P번 산타 움직임
	for (int p = 1; p <= P; p++) { 
		if (!santa[p].alive) continue; //탈락한 산타
		if (santa[p].t!=0 && santa[p].t + 2 > turn) continue; //기절한 상태
		
		//루돌프에게 가장 가까워지는 방향으로 1칸 이동
		int nearestDist = pow(Rr-santa[p].r,2)+pow(Rc-santa[p].c,2), nearestDir=-1;
		for (int d = 0; d < 4; d++) {
			int sr = santa[p].r + dx[d];
			int sc = santa[p].c + dy[d];
			if (sr<1 || sr>N || sc<1 || sc>N) continue;
			if (mat[sr][sc]) continue;
			int dist = pow(Rr - sr, 2) + pow(Rc - sc, 2);
			if (dist < nearestDist) {
				nearestDist = dist,
				nearestDir = d;
			}
		}
		if (nearestDir == -1 || nearestDist == pow(Rr - santa[p].r, 2) + pow(Rc - santa[p].c, 2)) {
			//움직일 수 있는 칸이 없거나, 있어도 루돌프와 가까워지는 방법이 없으면 안움직
			continue;
		}

		//상>하>좌>우 우선순위 고려
		for (int d = 0; d < 4; d++) {
			int sr = santa[p].r + dx[d];
			int sc = santa[p].c + dy[d];
			if (sr<1 || sr>N || sc<1 || sc>N) continue;
			if (mat[sr][sc]) continue;
			int dist = pow(Rr - sr, 2) + pow(Rc - sc, 2);
			if (dist == nearestDist) {
				nearestDir = d;
				break;
			}
		}

		//nearestDir방향으로 움직임
		mat[santa[p].r][santa[p].c] = 0;
		santa[p].r += dx[nearestDir];
		santa[p].c += dy[nearestDir];
		mat[santa[p].r][santa[p].c] = p;

//3-2. 산타가 움직여서 루돌프와 충돌
		if (santa[p].r == Rr && santa[p].c == Rc) {
			santa[p].s += D;

			if (nearestDir == 0) nearestDir = 2;
			else if (nearestDir == 1) nearestDir = 3;
			else if (nearestDir == 2) nearestDir = 0;
			else if (nearestDir == 3) nearestDir = 1;
			pushed(p, dx[nearestDir], dy[nearestDir], D); //산타는 자신이 이동해온 반대 방향으로 D칸 밀려남
			santa[p].t = turn;
		}
	}
}

bool allDead() {
	int sum = 0;
	for (int p = 1; p <= P; p++) {
		if (!santa[p].alive) sum++;
	}
	if (sum == P) return true;
	else return false;
}

void getScore() {
	for (int p = 1; p <= P; p++) {
		if (santa[p].alive) santa[p].s++;
	}
}
void printScore() {
	for (int p = 1; p <= P; p++) {
		cout<< santa[p].s<<" ";
	}
}

void print() {
	cout << Rr << " " << Rc << '\n';
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= N; j++) {
			cout << mat[i][j] << " ";
		}
		cout << '\n';
	}
	for (int p = 1; p <= P; p++) {
		cout << p << "번 ";
		cout << "(" << santa[p].r << ", " << santa[p].c << ") ";
		cout<< "점수: "<<santa[p].s << " 기절: " << santa[p].t<<" 생존: "<<santa[p].alive << '\n';
	}
}
int main() {

	init();

	for (int turn = 1; turn <= M; turn++) {

		Rmove(turn); 
		Smove(turn);
		if (allDead()) break;
		getScore();

	}
	//print();
	printScore();
	return 0;
}