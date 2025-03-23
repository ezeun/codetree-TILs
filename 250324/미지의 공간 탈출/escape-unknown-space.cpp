#include <cmath>
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

typedef pair<int, int> pii;
typedef tuple<int, int, int> tp;

int N, M, F;
int floorMat[21][21]; // 미지의 공간 바닥 평면도
int wallMat[31][31]; //시간의 벽 단면도 합쳐서 평면으로 나타낸 것

struct STRANGE { //이상현상
    int r, c, d, v;
};
vector<STRANGE> strange;

pii st; //타임머신 초기 위치(출발점) => wallMat 기준
pii ed; //탈출구(목적지) => floorMat 기준

pii floorMid; //시간의 벽과 맞닿은 미지의 공간의 바닥 중 빈공간인 칸 => floorMat 기준
pii wallMid; //바닥으로 가기위한 시간의 벽에서의 목적지 => wallMat 기준

int wallMidTime; //st에서 wallMid까지 가는데 걸리는 최단 시간

int dx[4] = {0, 0, 1, -1}; //동 서 남 북
int dy[4] = {1, -1, 0, 0};

int turn;

void input() {
    cin >> N >> M >> F;

    //미지의 공간 평면도 N줄
    for(int i=0; i<N; i++) {
        for(int j=0; j<N; j++) {
            cin >> floorMat[i][j];
            if(floorMat[i][j] == 4) ed = {i, j};
        }
    }

    //시간의 벽 동,서,남,북,윗면 단면도 각각 M줄
    for(int j=M*2; j<M*3; j++) {
        for(int i=M*2-1; i>=M; i--) cin>>wallMat[i][j];
    }
    for(int j=M-1; j>=0; j--) {
        for(int i=M; i<M*2; i++) cin>>wallMat[i][j];
    }
    for(int i=M*2; i<M*3; i++) {
        for(int j=M; j<M*2; j++) cin>>wallMat[i][j];
    }
    for(int i=M-1; i>=0; i--) {
        for(int j=M*2-1; j>=M; j--) cin>>wallMat[i][j];
    }
    for(int i=M; i<M*2; i++) {
        for(int j=M; j<M*2; j++) {
            cin>>wallMat[i][j];
            if(wallMat[i][j] == 2) st = {i, j};
        }
    }

    //이상현상 초기위치, 확산방향, 확산상수 F줄
    for(int i=0; i<F; i++) {
        int r,c,d,v; cin>>r>>c>>d>>v;
        strange.push_back({r,c,d,v});
        floorMat[r][c] = 5;
    }
}

void findMid() {
    wallMid = {5, 8}; //test
    floorMid = {4,5}; //test
}

pii findNext(pii cur, int d) {
    int cr = cur.first, cc = cur.second;
    pii next = {cr+dx[d], cc+dy[d]};
    int nr = next.first, nc = next.second;

    //범위를 벗어나는 경우 -1 리턴
    if(nr<0 || nr>=M*3 || nc<0 || nc>=M*3) next.first = -1;
    //단면도 내부 이동일 경우 그냥 리턴
    else if((M<=nr&&nr<M*2)||(M<=nc&&nc<M*2)) {}
    //범위 벗어나지않는데 단면도 외부인 경우 -> 변환과정 거쳐야함
    else {
        if(nr<M) {
            if(nc<M) { //좌상단
                if(d==1) next = {cr+(M-cr), cc-(M-cr)}; //서
                if(d==3) next = {cr-(M-cc), cc+(M-cc)}; //북
            }
            else { //우상단
                if(d==0) next = {cr+(M-cr), cc+(M-cr)}; //동
                if(d==3) next = {cr-(cc-M*2+1), cc-(cc-M*2+1)}; //북
            }
        }
        else {
            if(nc<M) { //좌하단
                if(d==1) next = {cr-(cr-M*2+1), cc-(cr-M*2+1)}; //서
                if(d==2) next = {cr+(M-cc), cc+(M-cc)}; //남
            }
            else { //우하단
                if(d==0) next = {cr-(cr-M*2+1), cc+(cr-M*2+1)}; //동
                if(d==2) next = {cr+(cc-M*2+1), cc-(cc-M*2+1)}; //남
            }
        }
    }
    return next;
}

int vis[31][31];
int wallMidBfs() {
    queue<pii> q;
    q.push(st);
    vis[st.first][st.second] = 1;
    while(!q.empty()) {
        pii cur = q.front(); q.pop();
        for(int d=0; d<4; d++) {
            pii next = findNext(cur, d);
            if(next.first == -1) continue; //범위를 벗어나는 경우
            if(vis[next.first][next.second]) continue; //이미 방문한 경우
            if(wallMat[next.first][next.second]) continue; //장애물이 있는 경우
            vis[next.first][next.second] = vis[cur.first][cur.second] + 1;
            q.push(next);
        }
    }
    return vis[wallMid.first][wallMid.second] - 1;
}

void strangeMove(int t) { //현재 t번째 턴일때 이상현상 번지기
    for(STRANGE &s : strange) {
        if(t % s.v == 0) { //v의 배수 턴마다 방향 d로 확산
            if(floorMat[s.r+dx[s.d]][s.c+dy[s.d]] == 0) {
                floorMat[s.r+dx[s.d]][s.c+dy[s.d]] = 5;
                s.r += dx[s.d];
                s.c += dy[s.d];
            }
        }
    }
}
void wallMidTimeStrangeMove() {
    for(int t=1; t<=wallMidTime; t++) {
        strangeMove(t);
    }
}

int fVis[21][21];
void floorMidBfs() {
    turn = wallMidTime+1;
    strangeMove(turn);

    queue<pii> q;
    q.push(floorMid);
    fVis[floorMid.first][floorMid.second] = wallMidTime+1;
    while(!q.empty()) {
        pii cur = q.front(); q.pop();
        if(fVis[cur.first][cur.second]>turn) {
            turn++;
            strangeMove(turn);
        }
        if(floorMat[cur.first][cur.second]==5) continue;
        if(floorMat[cur.first][cur.second]==4) {
            break;
        }
        for(int d=0; d<4; d++) {
            int nr = cur.first+dx[d];
            int nc = cur.second+dy[d];
            if(nr<0||nr>=N||nc<0||nc>=N) continue;
            if(fVis[nr][nc]) continue;
            if(floorMat[nr][nc]==0||floorMat[nr][nc]==4) {
                fVis[nr][nc] = fVis[cur.first][cur.second]+1;
                q.push({nr, nc});
            }
        }
    }
}

void printWallMat() { //테스트용
    for(int i=0; i<M*3; i++) {
        for(int j=0; j<M*3; j++) {
            cout<<wallMat[i][j]<<" ";
        } cout<<'\n';
    }
}
int main() {

    input();

    findMid(); // floorMid와 wallMid 찾기

    wallMidTime = wallMidBfs(); //타임머신 시작점(st)에서 wallMat 목적지(wallMid)까지 가는 최단시간 구하기
    if(wallMidTime == -1) { //wallMid까지 못가면 종료
        cout<< -1;
        return 0;
    }

    wallMidTimeStrangeMove(); // wallMidTime동안 이상현상 번지기

    // floorMat에서 턴 이어서 진행 (floorMid에서 ed까지 가는 최단시간 구하기)
    floorMidBfs();

    cout<<turn;
    // printWallMat();

    return 0;
}