#include <iostream>
#include <vector>
#include <queue>
#include <deque>
#include <algorithm>
using namespace std;

struct NODE {
    int x,y;
};

int N, M; //마을 크기, 전사의 수
NODE st, ed; //집, 공원 좌표
NODE medusa; //현재 메두사 좌표

int roadMat[51][51]; //도로 있으면 0, 없으면 1 (1이면 메두사 이동 불가)
vector<int> warriorMat[51][51]; //전사들 위치 저장(전사 번호 1부터 M), 한 칸에 여러명 가능

struct WARRIOR {
    int r, c;
    bool isRock = false; //돌이 되면 다음 턴에는 false로 바꿔줘야함!
    bool isAlive = true;
};
vector<WARRIOR> warriors(301);

int dx[4] = {-1, 1, 0, 0}; //상 하 좌 우
int dy[4] = {0, 0, -1, 1};

int warriorMoveDistance; //모든 전사가 이동한 거리의 합
int rockWarriorNum; //메두사로 인해 돌이 된 전사의 수
int killWarriorNum; //메두사를 공격한 전사의 수

void init() {
    cin >> N >> M;
    cin >> st.x >> st.y >> ed.x >> ed.y;

    for(int i=1; i<=M; i++) {
        int r, c; cin>> r >> c;
        warriorMat[r][c].push_back(i);
        warriors[i] = {r, c};
    }

    for(int i=0; i<N; i++) {
        for(int j=0; j<N; j++) {
            cin >> roadMat[i][j];
        }
    }
}

bool vis[51][51];
NODE parent[51][51];
deque<NODE> path; //집-공원 경로 (메두사 이동 경로)
void bfs() {
    queue<NODE> q;
    q.push({st.x, st.y});
    vis[st.x][st.y] = true;

    while(!q.empty()) {
        NODE cur = q.front(); q.pop();
        if(cur.x==ed.x && cur.y==ed.y) break;

        for(int d=0; d<4; d++) {
            int nx = cur.x + dx[d];
            int ny = cur.y + dy[d];
            if(nx<0||nx>=N||ny<0||ny>=N) continue;
            if(vis[nx][ny]) continue;
            if(roadMat[nx][ny]) continue;
            vis[nx][ny] = true;
            parent[nx][ny] = {cur.x, cur.y};
            q.push({nx, ny});
        }
    }

    if(vis[ed.x][ed.y]==false) return; //도달 불가능

    //경로추적
    NODE cur = ed;
    while(!(cur.x == st.x && cur.y==st.y)) {
        path.push_front({cur.x, cur.y});
        cur = parent[cur.x][cur.y];
    }
}

void moveMedusa() {
    medusa.x = path[0].x;
    medusa.y = path[0].y;
    path.pop_front();

    if(!warriorMat[medusa.x][medusa.y].empty()) { //이동한 칸에 전사가 있을 경우, 전사가 사라짐
        for(int warriorNum : warriorMat[medusa.x][medusa.y]) {
            warriors[warriorNum].isAlive = false;
        }
        warriorMat[medusa.x][medusa.y].clear();
    }
}

/*
 * 메두사 시선처리
 */
int seeMat[4][51][51]; //메두사 위치: 2, 메두사의 시선이 닿는 곳: 1, 전사에 가려서 못보는 곳: -1, 나머지: 0
vector<int> rockWarrior[4]; //메두사 시선 방향별로 돌이 되는 전사의 번호
int mxDir; //메두사가 전사를 가장 많이 볼 수 있는 방향
int mxCnt; //볼 수 있는 전사 수
void notSeeBfs(int direction, int x, int y) {
    //메두사와 상대위치 비교 -> 해당 방향으로만 bfs 이동 가능
    //seeMat를 -1로 채우기
    int dx[2], dy[2];
    int r = medusa.x - x;
    int c = medusa.y - y;
    int size;
    if(r>0&&c>0) { //좌,상
        dx[0]=0, dx[1]=-1;
        dy[0]=-1, dy[1]=0;
        size=2;
    }
    else if(r>0&&c==0) { //상
        dx[0]=-1;
        dy[0]=0;
        size=1;
    }
    else if(r>0&&c<0) { //우,상
        dx[0]=0, dx[1]=-1;
        dy[0]=1, dy[1]=0;
        size=2;
    }
    else if(r==0&&c>0) { //좌
        dx[0]=0;
        dy[0]=-1;
        size=1;
    }
    else if(r==0&&c<0) { //우
        dx[0]=0;
        dy[0]=1;
        size=1;
    }
    else if(r<0&&c>0) { //좌,하
        dx[0]=0, dx[1]=1;
        dy[0]=-1, dy[1]=0;
        size=2;
    }
    else if(r<0&&c==0) { //하
        dx[0]=1;
        dy[0]=0;
        size=1;
    }
    else if(r<0&&c<0) { //우,하
        dx[0]=0, dx[1]=1;
        dy[0]=1, dy[1]=0;
        size=2;
    }

    queue<NODE> q;
    q.push({x,y});
    seeMat[direction][x][y] = -1;

    while(!q.empty()) {
        NODE cur = q.front(); q.pop();

        for(int d=0; d<size; d++) {
            int nx = cur.x+dx[d];
            int ny = cur.y+dy[d];
            if(nx<0||nx>=N||ny<0||ny>=N) continue;
            if(seeMat[direction][nx][ny]==-1) continue;
            if(direction<=1) { //상,하
                if(abs(nx-x) < abs(ny-y)) continue;
            }
            else { //좌,우
                if(abs(nx-x) > abs(ny-y)) continue;
            }
            seeMat[direction][nx][ny] = -1;
            q.push({nx, ny});
        }
    }
}
void seeBfs(int direction) {
    int dx[3], dy[3];
    if(direction==0) { //좌,상,우
        dx[0]=0, dx[1]=-1, dx[2]=0;
        dy[0]=-1, dy[1]=0, dy[2]=1;
    }
    else if(direction==1) { //좌,하,우
        dx[0]=0, dx[1]=1, dx[2]=0;
        dy[0]=-1, dy[1]=0, dy[2]=1;
    }
    else if(direction==2) { //상,좌,하
        dx[0]=-1, dx[1]=0, dx[2]=1;
        dy[0]=0, dy[1]=-1, dy[2]=0;
    }
    else if(direction==3) { //상,우,하
        dx[0]=-1, dx[1]=0, dx[2]=1;
        dy[0]=0, dy[1]=1, dy[2]=0;
    }

    queue<NODE> q;
    q.push({medusa.x, medusa.y});
    seeMat[direction][medusa.x][medusa.y] = 2;

    while(!q.empty()) {
        NODE cur = q.front(); q.pop();
        if(!warriorMat[cur.x][cur.y].empty()) {
            notSeeBfs(direction, cur.x, cur.y); //seeMat를 -1로 채우기
            for(int w : warriorMat[cur.x][cur.y]) {
                rockWarrior[direction].push_back(w);
            }
        }

        for(int d=0; d<3; d++) {
            int nx = cur.x+dx[d];
            int ny = cur.y+dy[d];
            if(nx<0||nx>=N||ny<0||ny>=N) continue;
            if(seeMat[direction][nx][ny]||seeMat[direction][nx][ny]==-1) continue;
            if(direction<=1) { //상,하
                if(abs(nx-medusa.x) < abs(ny-medusa.y)) continue;
            }
            else { //좌,우
                if(abs(nx-medusa.x) > abs(ny-medusa.y)) continue;
            }
            seeMat[direction][nx][ny] = 1;
            q.push({nx, ny});
        }
    }
}
void medusaSee() {
    // 배열 초기화
    for(int x=0; x<4; x++) {
        for(int y=0; y<51; y++) {
            for(int z=0; z<51; z++) {
                seeMat[x][y][z] = 0;
            }
        }
    }
    for(int i=0; i<4; i++)
        rockWarrior[i].clear();

    //메두사의 시선
    for(int d=0; d<4; d++) {
        seeBfs(d);
    }

    mxCnt = 0;
    for(int d=0; d<4; d++) {
        if(mxCnt<rockWarrior[d].size()) {
            mxCnt = rockWarrior[d].size();
            mxDir = d;
        }
    }
}

void warriorToRock() { //rockWarrior[mxDir]의 전사들을 돌로 만들기
    for(int num : rockWarrior[mxDir]) {
        warriors[num].isRock = true;
        rockWarriorNum++;
    }
}
void rockToWarrior() { //rockWarrior[mxDir]의 전사들을 돌로 만들기
    for(int num : rockWarrior[mxDir]) {
        warriors[num].isRock = false;
    }
}

void moveWarrior1() {
    for(int i=1; i<=M; i++)  {
        if(warriors[i].isRock || !warriors[i].isAlive) continue;
        int distance = abs(medusa.x-warriors[i].r)+abs(medusa.y-warriors[i].c); //전사와 메두사의 현재 거리
        for(int d=0; d<4; d++) {
            int nx=warriors[i].r+dx[d];
            int ny=warriors[i].c+dy[d];
            if(nx<0||nx>=N||ny<0||ny>=N) continue;
            if(seeMat[mxDir][nx][ny]==1) continue;
            if(abs(medusa.x-nx)+abs(medusa.y-ny)<distance) { //메두사와의 거리를 줄일 수 있는 방향으로 한 칸 이동
                warriorMat[warriors[i].r][warriors[i].c].erase(remove(warriorMat[warriors[i].r][warriors[i].c].begin(), warriorMat[warriors[i].r][warriors[i].c].end(), i), warriorMat[warriors[i].r][warriors[i].c].end());
                warriors[i].r = nx, warriors[i].c = ny;
                warriorMat[warriors[i].r][warriors[i].c].push_back(i);
                warriorMoveDistance++;
                break;
            }
        }

    }
}
void moveWarrior2() {
    int dx[4] = {0,0,-1,1};
    int dy[4] = {-1,1,0,0};
    for(int i=1; i<=M; i++)  {
        if(warriors[i].isRock || !warriors[i].isAlive) continue;
        int distance = abs(medusa.x-warriors[i].r)+abs(medusa.y-warriors[i].c); //전사와 메두사의 현재 거리
        for(int d=0; d<4; d++) {
            int nx=warriors[i].r+dx[d];
            int ny=warriors[i].c+dy[d];
            if(nx<0||nx>=N||ny<0||ny>=N) continue;
            if(seeMat[mxDir][nx][ny]==1) continue;
            if(abs(medusa.x-nx)+abs(medusa.y-ny)<distance) { //메두사와의 거리를 줄일 수 있는 방향으로 한 칸 이동
                warriorMat[warriors[i].r][warriors[i].c].erase(remove(warriorMat[warriors[i].r][warriors[i].c].begin(), warriorMat[warriors[i].r][warriors[i].c].end(), i), warriorMat[warriors[i].r][warriors[i].c].end());
                warriors[i].r = nx, warriors[i].c = ny;
                warriorMat[warriors[i].r][warriors[i].c].push_back(i);
                warriorMoveDistance++;
                break;
            }
        }
    }
}

void killWarrior() {
    for(int i=1; i<=M; i++) {
        if(warriors[i].isAlive && (warriors[i].r==medusa.x && warriors[i].c==medusa.y)) {
            warriors[i].isAlive = false;
            warriorMat[warriors[i].r][warriors[i].c].erase(remove(warriorMat[warriors[i].r][warriors[i].c].begin(), warriorMat[warriors[i].r][warriors[i].c].end(), i), warriorMat[warriors[i].r][warriors[i].c].end());
            killWarriorNum++;
        }
    }
}

void debug(int turn) {
    cout<<"============"<<turn<<"============\n";
    for(int i=1; i<=M; i++) {
        cout<<i<<"th warrior "<<"("<<warriors[i].r<<", "<<warriors[i].c<<") ";
        cout<<warriors[i].isAlive<<" "<<warriors[i].isRock<<'\n';
    }
}
int main() {

    init();

    bfs();
    if(path.empty()) { //집에서 공원까지 이어지는 도로가 없는 경우
        cout<< -1;
        return 0;
    }
    // for(auto p : path) cout<<p.x<<" "<<p.y<<"\n"; //메두사의 이동 경로 출력

    int turn = 0;
    while(true) {
        warriorMoveDistance = 0;
        rockWarriorNum = 0;
        killWarriorNum = 0;

        // debug(++turn);

        moveMedusa(); //1. 메두사의 이동
        if(medusa.x==ed.x && medusa.y==ed.y) { //메두사가 공원에 도착하는 턴
            cout<< 0;
            break;
        }

        medusaSee(); //2. 메두사의 시선

        warriorToRock();

        moveWarrior1(); //3. 전사들의 첫번째 이동
        moveWarrior2(); //3. 전사들의 두번째 이동

        killWarrior(); //4. 전사의 공격

        // debug(turn);

        rockToWarrior();

        cout<< warriorMoveDistance << " ";
        cout<< rockWarriorNum << " ";
        cout<< killWarriorNum << "\n";

    }

    return 0;
}
