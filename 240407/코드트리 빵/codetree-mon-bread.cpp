#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;
typedef pair<int, int> pii;
typedef pair<int, pii> ipii;

int n, m; //격자 크기, 사람 수

int mat[16][16]; //0: 빈칸, 1: 베이스캠프, -1: 못지나감
pii store[31]; //{x,y} : 편의점 위치
queue<int> person[16][16]; //1~m : 사람 번호

bool arrived[31]; //i번 사람이 편의점에 도착했다면 true

int dx[4] = {-1, 0, 0, 1}; //상좌우하
int dy[4] = {0, -1, 1, 0};

void init(){
    cin>> n >> m ;

    for(int i=1; i<=n; i++){
        for(int j=1; j<=n; j++){
            cin>> mat[i][j];
        }
    }

    for(int i=1; i<=m; i++){
        int x,y; cin>>x>>y;
        store[i]={x,y};
    }
}

void gotoStore(){
    //움직이기 전 사람 수 체크(해당 수만큼만 q.pop()하고 이동시킬예정)
    int personNum[16][16]={0,};
    for(int i=1; i<=n; i++){
        for(int j=1; j<=n; j++){
            personNum[i][j] = person[i][j].size();
        }
    }

    //각자 가고싶은 편의점 방향으로 1칸 움직
    for(int i=1; i<=n; i++){
        for(int j=1; j<=n; j++){
            for(int k=0; k<personNum[i][j]; k++){
                //현재 움직이려는 사람의 번호
                int curPerson = person[i][j].front();
                person[i][j].pop();

                //움직이려는 사람의 위치
                int px = i;
                int py = j;

                //가고싶은 편의점 위치
                int sx = store[curPerson].first;
                int sy = store[curPerson].second;

                //이미 편의점에 있는 사람이면 건너뜀
                if(px==sx && py==sy) continue;

                //최단경로 저장(구해서 그 경로로 한 칸 이동해야함)
                pii imfrom[16][16];

                //사람->편의점 최단경로 구하기
                int vis[16][16]={0,};
                queue<pii> q;

                q.push({px, py});
                vis[px][py]=1;

                while(!q.empty()){
                    int curX=q.front().first;
                    int curY=q.front().second;
                    q.pop();

                    if(curX==sx && curY==sy) break;

                    for(int d=0; d<4; d++){
                        int nx = curX+dx[d];
                        int ny= curY+dy[d];

                        if(nx<1||nx>n||ny<1||ny>n) continue;
                        if(vis[nx][ny]) continue;
                        if(mat[nx][ny]==-1) continue;

                        q.push({nx, ny});
                        vis[nx][ny]=1;

                        imfrom[nx][ny]={curX,curY};
                    }
                }
//                for(int i=1; i<=n; i++){
//                    for(int j=1; j<=n; j++){
//                        cout<<"{"<<imfrom[i][j].first<<" "<<imfrom[i][j].second<<"} ";
//                    }cout<<'\n';
//                }

                pii goBack = {sx,sy}; //사람<-편의점 최단경로 추적
                pii nextCell; //결과적으로 이동해아하는 칸
                while(true){
                    pii tmp = goBack;
                    goBack = imfrom[tmp.first][tmp.second];
                    if(goBack.first==px && goBack.second==py){
                        nextCell = tmp; //출발지 도착 직전 칸을 저장
                        break;
                    }
                }

                //해당 사람을 편의점방향으로 1칸 움직
                person[nextCell.first][nextCell.second].push(curPerson);
            }
        }
    }

    //원하는 편의점 도착한 사람이 있다면 이제 해당 칸 못지나감
    for(int i=1; i<=n; i++){
        for(int j=1; j<=n; j++){
            int qSize = person[i][j].size();
            for(int k=0; k<qSize; k++){
                int curPerson = person[i][j].front();
                person[i][j].pop(); person[i][j].push(curPerson);

                if(i==store[curPerson].first && j==store[curPerson].second){
                    mat[i][j]=-1;
                    arrived[curPerson]=true;
                }
            }
        }
    }
}

bool cmp(const pii&a, const pii&b){
    if(a.first!=b.first) return a.first<b.first;
    else return a.second<b.second;
}
void gotoBasecamp(int t){
    if(t<=m){
        //t번 사람이 가려는 편의점 위치
        int sx = store[t].first;
        int sy = store[t].second;

        // 편의점에서 출발해서 가장 가까운 베이스캠프 찾기
        vector<pii> nearBasecamp;
        int vis[16][16]={0,};
        queue<pii> q;

        q.push({sx, sy});
        vis[sx][sy]=1;

        bool first = true; //처음 만난 베이스캠프인지
        int dist =16; //처음만난 베이스캠프까지의 거리 저장
        while(!q.empty()){
            pii cur=q.front();
            q.pop();

            //베이스캠프에 도착한 경우
            if(mat[cur.first][cur.second]==1 && first){ //처음 도착
                first=false;
                dist = vis[cur.first][cur.second];
                nearBasecamp.push_back({cur.first, cur.second});
                continue;
            }
            else if(mat[cur.first][cur.second]==1 && !first){ //처음 도착은 아닌데 최단거리는 같음
                if(vis[cur.first][cur.second]==dist){
                    nearBasecamp.push_back({cur.first, cur.second});
                    continue;
                }
                else{
                    break;
                }
            }

            for(int d=0; d<4; d++){
                int nx = cur.first+dx[d];
                int ny = cur.second+dy[d];

                if(nx<1||nx>n||ny<1||ny>n) continue;
                if(vis[nx][ny]) continue;
                if(mat[nx][ny]==-1) continue;

                q.push({nx, ny});
                vis[nx][ny]=vis[cur.first][cur.second]+1;
            }
        }

        sort(nearBasecamp.begin(), nearBasecamp.end(), cmp);

        //t번사람은 자신이 가려는 편의점과 최단거리인 베이스캠프에 들어감
        int bx=nearBasecamp[0].first;
        int by=nearBasecamp[0].second;

        person[bx][by].push(t);
        mat[bx][by]=-1; //이제 여기 못지나감
    }
}

bool everyoneArrived(){
    int sum=0;
    for(int i=1; i<=m; i++){
        if(arrived[i]==true) sum++;
    }
    if(sum==m) return true; //모든 사람이 도착했음
    else return false;
}

int main() {

    init();

    int time;
    for(time=1; ; time++){

        //m명이 각자 가고싶은 편의점 방향으로 1칸 움직
        //편의점에 도착한다면 멈춤
        gotoStore();

        //현재 t분이고 t≤m이면, t번사람은 자신이 가려는 편의점과 최단거리인 베이스캠프에 들어감
        gotoBasecamp(time);

        //모든 사람이 편의점에 도착했으면 종료
        if(everyoneArrived()) break;

    }
    cout<<time;
    return 0;
}