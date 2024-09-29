#include <iostream>
#include <vector>
#include <climits>
#include <algorithm>
#include <queue>
#include <map>
#include <list>
using namespace std;
typedef pair<int,int> pii;
typedef long long ll;

int K, M; //탐사의 반복 횟수 K와 벽면에 적힌 유물 조각의 개수 M
int relic[5][5]; //유적지에 있는 유물조각 번호
int wall[301]; //벽면에 적힌 유물조각 번호
int wallIdx=0;

int rotatedRelic[5][5];

void init() {
    cin>> K>>M;

    for(int i=0; i<5; i++) {
        for(int j=0; j<5; j++) {
            cin>>relic[i][j];
        }
    }

    for(int i=0; i<M; i++) {
        cin>>wall[i];
    }
}

struct Selected {
    int r,c; //회전 중심좌표 행,열
    int value; //유물 1차 획득 가치
    int degree; //회전한 각도

};
void copyRelic() {
    for(int i=0; i<5; i++) {
        for(int j=0; j<5; j++) {
            rotatedRelic[i][j] = relic[i][j];
        }
    }
}
void rotate(int r, int c) {
    // 시계방향으로 두칸씩 이동하면 됨
    int leftTop = rotatedRelic[r-1][c-1];
    rotatedRelic[r-1][c-1] = rotatedRelic[r+1][c-1];
    rotatedRelic[r+1][c-1] = rotatedRelic[r+1][c+1];
    rotatedRelic[r+1][c+1] = rotatedRelic[r-1][c+1];
    rotatedRelic[r-1][c+1] = leftTop;
    int midTop = rotatedRelic[r-1][c];
    rotatedRelic[r-1][c] = rotatedRelic[r][c-1];
    rotatedRelic[r][c-1] = rotatedRelic[r+1][c];
    rotatedRelic[r+1][c] = rotatedRelic[r][c+1];
    rotatedRelic[r][c+1] = midTop;
}
bool vis[5][5];
int dx[4]={0,-1,0,1};
int dy[4]={1,0,-1,0};
int bfs(int r, int c) {
    int numOfConnectedRelics = 0;

    queue<pii> q;
    q.push({r,c});
    vis[r][c]=true;

    while(!q.empty()) {
        pii cur = q.front(); q.pop();
        numOfConnectedRelics++;

        for(int d=0; d<4; d++) {
            int nx=cur.first+dx[d];
            int ny=cur.second+dy[d];
            if(!(0<=nx&&nx<5&&0<=ny&&ny<5)) continue;
            if(vis[nx][ny]) continue;
            if(rotatedRelic[r][c]!=rotatedRelic[nx][ny]) continue;
            vis[nx][ny] = true;
            q.push({nx,ny});
        }
    }

    if(numOfConnectedRelics<3) return 0;
    return numOfConnectedRelics;
}
int getRelicValue(){ //rotatedRelic의 유물1차획득 결과 유물의 가치 리턴
    int sum=0;
    for(int i=0; i<5; i++) {
        for(int j=0; j<5; j++) {
            vis[i][j]=false;
        }
    }
    for(int i=0; i<5; i++) {
        for(int j=0; j<5; j++) {
            if(!vis[i][j])
                sum+=bfs(i,j); //3이상일때만 리턴, 3미만이면 0을 리턴
        }
    }
    return sum;
}
bool cmp(const Selected &a, const Selected &b) {
    if(a.value!=b.value) return a.value>b.value;
    else if(a.degree!=b.degree) return a.degree<b.degree;
    else if(a.c!=b.c) return a.c<b.c;
    else return  a.r<b.r;
}
void rotateOrigin(int r, int c, int degree) {
    for(int k=1; k<=degree; k++) {
        int leftTop = relic[r-1][c-1];
        relic[r-1][c-1] = relic[r+1][c-1];
        relic[r+1][c-1] = relic[r+1][c+1];
        relic[r+1][c+1] = relic[r-1][c+1];
        relic[r-1][c+1] = leftTop;
        int midTop = relic[r-1][c];
        relic[r-1][c] = relic[r][c-1];
        relic[r][c-1] = relic[r+1][c];
        relic[r+1][c] = relic[r][c+1];
        relic[r][c+1] = midTop;
    }
}
int explore() {
    vector<Selected> v; //모든 회전 경우를 저장

    for(int i=1; i<=3; i++) {
        for(int j=1; j<=3; j++) {
            copyRelic(); //원본유적지 relic을 rotatedRelic에 복사
            for(int k=1; k<=3; k++) {
                Selected s = {i,j,0,k};
                rotate(i,j); //(i,j)를 중심으로 90도씩 회전
                s.value = getRelicValue();
                v.push_back(s);
            }
        }
    }

    sort(v.begin(), v.end(), cmp); //우선순위에따라 회전경우 정렬
    if(v[0].value==0) return 0;
    rotateOrigin(v[0].r, v[0].c, v[0].degree); //가장 우선순위 높은것 대로 원본배열 relic을 회전
    return 1;
    // cout<<v[0].r<<" "<<v[0].c<<"\n";
}


bool vis2[5][5];
void clearVis2() {
    for(int i=0; i<5; i++) {
        for(int j=0; j<5; j++) {
            vis2[i][j]=false;
        }
    }
}
int bfs2(int r, int c) {
    int numOfConnectedRelics = 0;

    queue<pii> q;
    q.push({r,c});
    vis2[r][c]=true;

    while(!q.empty()) {
        pii cur = q.front(); q.pop();
        numOfConnectedRelics++;

        for(int d=0; d<4; d++) {
            int nx=cur.first+dx[d];
            int ny=cur.second+dy[d];
            if(!(0<=nx&&nx<5&&0<=ny&&ny<5)) continue;
            if(vis2[nx][ny]) continue;
            if(relic[r][c]!=relic[nx][ny]) continue;
            vis2[nx][ny] = true;
            q.push({nx,ny});
        }
    }

    return numOfConnectedRelics;
}
void removeRelic(int r, int c) {
    queue<pii> q;
    q.push({r,c});
    vis2[r][c]=true;
    int originRelicValue = relic[r][c];

    while(!q.empty()) {
        pii cur = q.front(); q.pop();
        relic[cur.first][cur.second] = 0;

        for(int d=0; d<4; d++) {
            int nx=cur.first+dx[d];
            int ny=cur.second+dy[d];
            if(!(0<=nx&&nx<5&&0<=ny&&ny<5)) continue;
            if(vis2[nx][ny]) continue;
            if(relic[nx][ny]!=originRelicValue) continue;
            vis2[nx][ny] = true;
            q.push({nx,ny});
        }
    }

}
void printRelic() {
    cout<<"\n";
    for(int i=0; i<5; i++) {
        for(int j=0; j<5; j++) {
            cout<<relic[i][j];
        } cout<<'\n';
    }
    cout<<"\n";
    cout<<"\n";
}
int getRelic() {
    int ret = 0; //획득한 유믈의 가치 총합

    while(1) { //유물 없을때까지 유물 획득을 반복함
        //유물 획득하기
        clearVis2();
        vector<pii> relicPos; //유뮬 있는곳 bfs시작좌표
        for(int i=0; i<5; i++) {
            for(int j=0; j<5; j++) {
                if(!vis2[i][j]) {
                    int res = bfs2(i,j); //획득한 유믈의 가치
                    if(res>=3) {
                        ret+=res;
                        relicPos.push_back({i,j}); //유물있으면 시작좌표 저장
                        }
                }
            }
        }
        if(relicPos.size()==0) break; //더이상 유물 없으면 종료

        //획득한 유물 삭제
        clearVis2();
        for(pii &p : relicPos) {
            removeRelic(p.first, p.second); //유물있는곳 relic[i][j]=0으로 만듦
        }

        //유물 사라진 곳 채우기
        for(int c=0; c<5; c++) {
            for(int r=4; r>=0; r--) {
                if(relic[r][c]==0) {
                    relic[r][c]=wall[wallIdx++];
                }
            }
        }
    // printRelic();
    }

    return ret;
}

int main()
{
    init();

    for(int i=0; i<K; i++) { //3. 탐사반복

        if(explore()==0) return 0; //1. 탐사진행

        int relicValueSum = getRelic(); //2. 유물획득

        if(!relicValueSum) return 0; //유물 획득이 불가한 상황이면 즉시 종료
        cout<< relicValueSum<<" "; //해당 턴에서 획득한 유물 가치의 총합을 출력

    }


    return 0;
}