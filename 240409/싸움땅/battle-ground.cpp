#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;
typedef pair<int, int> pii;
typedef pair<int, pii> ipii;

int n, m, k; //격자 크기, 플레이어 수, 라운드 수
priority_queue<int> pq[21][21]; //pq[i][j] : (i,j)에 있는 총들의 공격력 내림차순

struct Player{
    int x,y; //위치
    int d; //방향
    int s; //초기 능력치
    int gun=0; //갖고있는 총의 공격력
    int point=0; //포인트
};
vector<Player> player;

int dx[4]={-1,0,1,0}; //상우하좌
int dy[4]={0,1,0,-1};

void init(){
    cin >> n >> m >> k;

    for(int i=1; i<=n; i++){ //격자판에 있는 총의 공격력 입력
        for(int j=1; j<=n; j++){
            int power; cin>>power;
            if(power>0)
                pq[i][j].push(power);
        }
    }

    for(int i=0; i<m; i++){ //1~n번 플레이어 입력
        Player tmp;
        cin >> tmp.x >> tmp.y >> tmp.d >> tmp.s;
        player.push_back(tmp);
    }
}

pii movePlayer(int p){
    //이동할 칸 구하기
    int nx = player[p].x+dx[player[p].d];
    int ny = player[p].y+dy[player[p].d];
    if(nx<1||nx>n||ny<1||ny>n){
        player[p].d = (player[p].d+2)%4; //반대방향(0<->2, 1<->3)
        nx = player[p].x+dx[player[p].d];
        ny = player[p].y+dy[player[p].d];
    }

    //이동하기
    player[p].x = nx;
    player[p].y = ny;

    return {nx, ny};
}

bool existPlayer(pii pos, int p){
    //pos위치에 p말고 다른 플레이어가 있다면 true를 반환
    for(int i=0; i<player.size(); i++){
        if(i==p) continue;
        if(player[i].x==pos.first && player[i].y==pos.second)
            return true;
    }
    return false;
}

void checkGun(pii pos, int p){
    if(!pq[pos.first][pos.second].empty()){ //총이 있다면
        pq[pos.first][pos.second].push(player[p].gun); //갖고있는 총을 내려두고(없으면 0을 push)
        player[p].gun = pq[pos.first][pos.second].top(); //놓여있는 총들 중 max공격력인 총을 가짐
        pq[pos.first][pos.second].pop();
    }
}

void fight(pii pos, int p){
    //2-2-1. 이동한 칸에 플레이어가 있다면, 싸움
    //싸울 플레이어 번호 구하기
    int fightA = p;
    int fightB;
    for(int i=0; i<player.size(); i++){
        if(i==p) continue;
        if(player[i].x==pos.first && player[i].y==pos.second)
            fightB = i;
    }

    //싸움의 승패
    int winner;
    int loser;

    //초기능력치+갖고있는 총의 공격력이 더 큰사람이 이김
    if(player[fightA].s+player[fightA].gun > player[fightB].s+player[fightB].gun){
        winner = fightA, loser = fightB;
    }
    else if(player[fightA].s+player[fightA].gun < player[fightB].s+player[fightB].gun){
        winner = fightB, loser = fightA;
    }
    else{ //같다면, 초기능력치 큰 사람이 이김
        if(player[fightA].s>player[fightB].s){
            winner = fightA, loser = fightB;
        }
        else{
            winner = fightB, loser = fightA;
        }
    }

    //이긴플레이어는 각 플레이어의 (초기능력치+갖고있는 총의 공격력) 차이만큼을 포인트로 획득
    player[winner].point += (player[winner].s+player[winner].gun)-(player[loser].s+player[loser].gun);


    //2-2-2. 진 플레이어
    //가지고있는 총을 해당 칸에 내려놓음.
    pq[pos.first][pos.second].push(player[loser].gun);
    player[loser].gun = 0;

    //가던 방향으로 한 칸 이동(못가면 90도 회전 반복) -----> 회전시 player[loser].d 갱신!!!
    for(int i=0; i<4; i++){
        int nx = pos.first + dx[(player[loser].d+i)%4];
        int ny = pos.second + dy[(player[loser].d+i)%4];

        int existPlayer=0;
        for(int j=0; j<player.size(); j++){
            if(player[j].x==nx && player[j].y==ny){
                existPlayer=1;
                break;
            }
        }
        //다른 플레이어가 있거나 격자 밖이면 오른쪽으로 90도씩 회전
        if(existPlayer) continue;
        if(nx<1||nx>n||ny<1||ny>n) continue;

        //이동할 수 있는 경우,
        // 방향 갱신
        player[loser].d = (player[loser].d+i)%4;

        //이동
        player[loser].x = nx;
        player[loser].y = ny;

        //이동한 칸에 총이 있다면, 가장 공격력 높은 총을 획득
        if(!pq[nx][ny].empty()){
            pq[nx][ny].push(player[loser].gun);
            player[loser].gun = pq[nx][ny].top();
            pq[nx][ny].pop();
        }
        break;
    }

    //2-2-3. 이긴 플레이어
    //떨어져 있는 총들과 원래 들고 있던 총 중 가장 공격력이 높은 총을 획득
    pq[pos.first][pos.second].push(player[winner].gun);
    player[winner].gun = pq[pos.first][pos.second].top();
    pq[pos.first][pos.second].pop();

}

void printPoint(){
    for(Player &p : player){
        cout<< p.point<<" ";
    }
    cout<<'\n';
}
void printGun(){
    for(int i=1; i<=n; i++){ //격자판에 있는 총의 공격력 입력
        for(int j=1; j<=n; j++){
            cout<<"{";
            int size = pq[i][j].size();
            for(int k=0; k<size; k++){
                cout<<pq[i][j].top()<<", ";
                pq[i][j].pop();
            }
            cout<<"} ";
        }
        cout<<'\n';
    }
}
void printPlayer(){
    for(Player &p : player){
        cout<<"{"<<p.x<<", "<<p.y<<"} 방향:"<<p.d<<" 초기능력치"<<p.s<<" 총"<<p.gun<<" 포인트"<<p.point<<'\n';
    }
}
int main() {

    init();

    for(int round=1; round<=k; round++){

        for(int p=0; p<player.size(); p++){

            //1. 본인이 향하고 있는 방향으로 한 칸 이동
            pii pos = movePlayer(p); //이동한 칸의 좌표 반환

            //2-1. 이동한 칸에 다른 플레이어가 없다면,
            if(existPlayer(pos, p)==false){
                checkGun(pos, p);
            }
            //2-2. 이동한 칸에 다른 플레이어가 있다면,
            else{
                fight(pos, p);
            }
        }
    }

    printPoint();

//    printGun();
//    printPlayer();
    return 0;
}