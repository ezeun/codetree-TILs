#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;
typedef pair<int, int> pii;
typedef pair<int, pii> ipii;

int N, M, K;
int mat[11][11]; //포탑의 공격력 저장 //좌상단 (0,0) //0이하이면 부서진 포탑
int tmpMat[11][11]; //포탑정비할 때 피해입지않은 포탑 알아내기위해 비교군으로 사용

struct Turret{
    int power; //공격력
    int recentAttack; //가장 최근에 공격한 시점(k값)
    int x; //행 좌표
    int y; //열 좌표
};
vector<Turret> turret; //공격력이 1이상인 포탑들 저장

Turret attacker;
Turret target;

int dx[4] = {0,1,0,-1}; //우하좌상 우선순위
int dy[4] = {1, 0, -1, 0};
int dx8[8] = {0, 1, 1, 1, 0, -1, -1, -1}; //위쪽부터 시계방향
int dy8[8] = {-1, -1, 0, 1, 1, 1, 0, -1};

void init(){
    cin >> N >> M >> K;

    for(int i=0; i<N; i++){
        for(int j=0; j<M; j++){
            cin>>mat[i][j];
            if(mat[i][j]>=1){
                Turret tmp;
                tmp.power = mat[i][j];
                tmp.recentAttack =0;
                tmp.x=i;
                tmp.y=j;
                turret.push_back(tmp);
            }
        }
    }
}

bool cmp(const Turret &a, const Turret &b){
    if(a.power!=b.power) return a.power<b.power;
    else if(a.recentAttack!=b.recentAttack) return a.recentAttack>b.recentAttack;
    else if((a.x+a.y)!=(b.x+b.y)) return (a.x+a.y)>(b.x+b.y);
    else return a.y>b.y;
}

void selectAttacker(int k){
    //가장 약한 포탑(공격자) 구하기
    sort(turret.begin(), turret.end(), cmp);

    //N+M만큼 공격력 증가
    turret[0].power+=N+M;
    mat[turret[0].x][turret[0].y]+=N+M;

    //최근 공격 시점 갱신
    turret[0].recentAttack = k;

    //공격자 저장
    attacker = turret[0];
}
void selectTarget(){
    //공격대상 저장
    target = turret[turret.size()-1];
}
void printTurret(Turret t){
    cout<<t.power<<" "<<t.recentAttack<<" "<<t.x<<" "<<t.y<<"\n";
}

void copyMat(){
    for(int i=0; i<N; i++){
        for(int j=0; j<M; j++){
            tmpMat[i][j] = mat[i][j];
        }
    }
}

bool laserAttack(){
    //bfs로 공격자~공격대상 최단경로 구하기
    queue<pair<pii, vector<pii>>> q; //현재 좌표, 거쳐온 레이저 경로 좌표들
    int vis[11][11]={0,};
    vector<pii> laserPath; //레이저 경로에 있는 포탑들

    q.push({{attacker.x, attacker.y}, {}});
    vis[attacker.x][attacker.y]=1;

    while(!q.empty()){
        pair<pii, vector<pii>> cur = q.front();
        q.pop();

        if(cur.first.first==target.x && cur.first.second==target.y) {
            laserPath = cur.second;
            laserPath.pop_back(); //공격대상자는 제외
            break;
        }

        for(int d=0; d<4; d++){
            int nx = cur.first.first+dx[d];
            int ny = cur.first.second+dy[d];

            nx = (nx+4)%4;
            ny = (ny+4)%4;

            if(vis[nx][ny]) continue;
            if(mat[nx][ny]<=0) continue;

            vector<pii> path = cur.second;
            path.push_back({nx, ny});
            q.push({{nx,ny}, path});
            vis[nx][ny]=1;
        }
    }

    //경로가 없다면 flase리턴
    if(vis[target.x][target.y]==0) return false;
    //경로가 있다면 공격하기
    else{
        //공격대상 피해입음
        target.power -= attacker.power;
        mat[target.x][target.y] -= attacker.power;

        //레이저 경로에 있는 포탑들 피해입음
        for(pii &tur : laserPath){
            mat[tur.first][tur.second] -= attacker.power/2;
        }

        //turret벡터 power도 갱신
        for(Turret &tur : turret){
            tur.power = mat[tur.x][tur.y];
        }

        return true;
    }
}
void potanAttack(){
    //공격대상 피해입음
    target.power -= attacker.power;
    mat[target.x][target.y] -= attacker.power;

    //공격대상 주위 8방향 포탑들 피해입음(공격자 제외)
    for(int d=0; d<8; d++){
        int nx = target.x+dx8[d];
        int ny = target.y+dy8[d];

        nx = (nx+4)%4;
        ny = (ny+4)%4;

        if(nx==attacker.x && ny==attacker.y) continue;
        if(mat[nx][ny]<=0) continue;

        mat[nx][ny] -= attacker.power/2;
    }

    //turret벡터 power도 갱신
    for(Turret &tur : turret){
        tur.power = mat[tur.x][tur.y];
    }
}
void printMat(){
    for(int i=0; i<N; i++) {
        for (int j = 0; j < M; j++) {
            cout<<mat[i][j]<<" ";
        }
        cout<<'\n';
    }
}

void turretSetting(){
    //부서진 포탑, 공격자, 공격대상, 피해입은 포탑 제외하고 power++
    for(int i=0; i<N; i++) {
        for (int j = 0; j < M; j++) {
            if(mat[i][j]<=0) continue;
            if(i==attacker.x && j==attacker.y) continue;
            if(mat[i][j]!=tmpMat[i][j]) continue;

            mat[i][j]++;
            for(Turret &tur : turret){
                tur.power = mat[tur.x][tur.y];
            }
        }
    }
}

int calcRemain(){
    for(int i=0; i<turret.size(); i++){
        if(turret[i].power<=0){
            turret.erase(turret.begin()+i);
            i--;
        }
    }
    return turret.size();
}

void printStrongestTurret(){
    int mxPower=0;
    for(int i=0; i<N; i++) {
        for (int j = 0; j < M; j++) {
            mxPower = max(mxPower, mat[i][j]);
        }
    }
    cout<<mxPower;
}

int main() {

    init();

    for(int k=1; k<=K; k++){
        //공격자 선정
        selectAttacker(k);
        //공격대상 선정
        selectTarget();

        //포탑 백업
        copyMat();

        //공격
        if(laserAttack()==false){
            potanAttack();
        }

        //포탑 정비
        turretSetting();

        //남은 포탑이 1개가 되면 즉시 종료
        if(calcRemain()==1){
            break;
        }
    }

    printStrongestTurret();

    return 0;
}