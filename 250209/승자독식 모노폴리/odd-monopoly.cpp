#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

/* 전역 변수 */
int n, m, k; //격자크기n, 플레이어수 m, 독점 계약 턴 수 k

struct GRID{ //격자 정보
    int owner; //해당 칸의 주인
    int time; //독점 계약한 시점
} grid[21][21];

struct PLAYER{ //플레이어 정보
    int direction; //바라보고있는 방향
    int r; //행 위치 (사라진 플레이어는 -1)
    int c; //열 위치 (사라진 플레이어는 -1)
} player[21];

int priority[21][5][4]; //각 플레이어의 방향에 따른 이동 우선순위
// priority[플레이어번호][1:위, 2:아, 3:왼, 4:오][우선순위 0,1,2,3 순위] = 1:위, 2:아, 3:왼, 4:오

int alive_player; //살아있는 플레이어 수

int dx[5] = {0, -1, 1, 0, 0}; //0:필요없음, 1:위, 2:아, 3:왼, 4:오
int dy[5] = {0, 0, 0, -1, 1};

int canNotMove;

bool exist[21][21]; //해당 칸에 번호가 작은 플레이어가 이미 존재하면 true

/* 입력받기 */
void init(){

    cin>> n >> m >> k;

    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            int x; cin>> x;
            if(x){
                //격자 정보
                grid[i][j].owner = x;
                grid[i][j].time = 1;
                //플레이어 위치 정보
                player[x].r = i;
                player[x].c = j;
            }
        }
    }

    for(int i=1; i<=m; i++){
        int x; cin>>x;
        player[i].direction = x; //플레이어 초기 방향
    }

    for(int p=1; p<=m; p++){ //플레이어 번호
        for(int i=1; i<=4; i++){ //위, 아래, 왼, 오
            for(int j=0; j<4; j++){ //우선순위 0~3순위
                int x; cin>> x;
                priority[p][i][j] = x; //플레이어의 방향에 따른 이동 우선순위
            }
        }
    }

    alive_player = m;
}


/* 플레이어 이동하기 */
void move_player(int turn){

    int whereToGo[21] = {};
    for(int p=1; p<=m; p++){ //각 플레이어를 어디로 이동시킬지 저장만 해두기!!(아직 이동 x)
        //player[p] 이동시키기

        if(player[p].r==-1) continue; //사라진 플레이어는 넘어가기

        int curR = player[p].r; //해당 플레이어의 현재 위치
        int curC = player[p].c;

        vector<int> noOwner; //아무도 독점계약 하지 않은 칸의 상하좌우 방향 숫자 담아두기
        int myOwn = -1; //본인이 독점계약한 칸의 방향

        // 상하좌우 탐색하면서 독점계약 하지 않은 칸이랑 본인이 독점계약한 칸 방향정보 수집
        for(int d=1; d<=4; d++){
            int nextR = curR + dx[d];
            int nextC = curC + dy[d];

            if(nextR < 0 || nextR >= n || nextC < 0 || nextC >= n) continue;
            if(grid[nextR][nextC].time == 0 || turn - grid[nextR][nextC].time > k){ //독점계약 없는 칸
                noOwner.push_back(d);
            }
            if(turn - grid[nextR][nextC].time < k && grid[nextR][nextC].owner == p){ //본인이 독점계약한 칸
                myOwn = d;
            }
        }

        int moveTo = -1; //해당 플레이어가 이동하게 될 방향

        if(noOwner.size() == 0){ //아무도 독점계약 하지 않은 칸이 없다면,
            if(myOwn != -1) moveTo = myOwn; //본인이 독점계약한 칸으로 이동
            else{ // 그것마저 없다면 이동할 수 없으므로 -1 출력
                canNotMove = true;
                return;
            }
        }
        else if(noOwner.size() == 1){ //아무도 독점계약 하지 않은 칸이 한개면, 해당 칸으로 이동
            moveTo = noOwner[0];
        }
        else{ //아무도 독점계약 하지 않은 칸이 여러개면, 이동 우선순위에 따라 움직일 칸 결정
            //이동 우선순위는 priority[p][player[p].direction][0~3]
            if(find(noOwner.begin(), noOwner.end(), priority[p][player[p].direction][0]) != noOwner.end())
                moveTo = priority[p][player[p].direction][0]; //noOwner에 첫번째 우선순위인 칸이 있으면 그 칸으로 이동
            else if(find(noOwner.begin(), noOwner.end(), priority[p][player[p].direction][1]) != noOwner.end())
                moveTo = priority[p][player[p].direction][1]; //없으면 두번째 우선순위
            else if(find(noOwner.begin(), noOwner.end(), priority[p][player[p].direction][2]) != noOwner.end())
                moveTo = priority[p][player[p].direction][2]; //없으면 세번째 (owner.size가 2 이상이므로 네번째는 할 필요 X)
        }

        whereToGo[p] = moveTo;
    }

    // 한방에 이동시키자
    for(int p=1; p<=m; p++) {
        // 그럼 이제 player[p]를 moveTo(= whereToGo[p]) 방향으로 한 칸 이동시키면 됨!
        if(player[p].r == -1) continue;

        player[p].direction = whereToGo[p];
        player[p].r += dx[whereToGo[p]];
        player[p].c += dy[whereToGo[p]];

        grid[player[p].r][player[p].c].owner = p;
        grid[player[p].r][player[p].c].time = turn;
    }
}


/* 겹치는 플레이어 제거하기 */
void remove_player(){
    //번호 작은 -> 큰 순서대로 exist 배열에 표시, 이미 표시되어있으면 해당 플레이어 제거
    for(int i=0; i<21; i++) {
        for(int j=0; j<21; j++) {
            exist[i][j] = false;
        }
    }
    for(int p=1; p<=m; p++) {
        if(player[p].r == -1) continue; //이미 제겨된 경우 넘어감

        if(exist[player[p].r][player[p].c]) {
            //player[p] 제거하기
            player[p].r = -1;
            player[p].c = -1;
            alive_player--;
        }
        else {
            exist[player[p].r][player[p].c] = true;
        }
    }
}

void print(int turn){
    cout<<"========turn: " << turn << " =========\n";
    for(int p=1; p<=m; p++){
        cout<< p << "th player: dir " << player[p].direction << " (" << player[p].r << ", " << player[p].c << ")\n";
    }
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            cout<<"[" << grid[i][j].owner << ", " << grid[i][j].time << "]";
        }
        cout<< '\n';
    }
    cout<<"\n";
}

int main() {

    init();

    int turn = 1;
    while(turn<1000){ //1000
        // print(turn); //테스트용

        turn++;

        move_player(turn); //우선순위를 고려하여, 모든 플레이어 이동

        if(canNotMove) break;

        remove_player(); //한 칸에 여러 플레이어가 있는 경우, 가장 작은 번호를 가진 플레이어를 제외하고 죽임

        if(alive_player == 1) break; //1번 플레이어만 남았다면 종료
    }

    if(canNotMove || turn == 1000) cout<< -1; //답이 1000 이상인 경우 -1을 출력
    else cout<< turn - 1; //1번 플레이어만 남게 되기까지 걸린 턴의 수 출력
// cout<<" "<<alive_player;
    // cout<<"p "<<priority[1][player[1].direction][0];
    // cout<<'\n';
    // for(int p=1; p<=m; p++){ //플레이어 번호
    //     for(int i=1; i<=4; i++){ //위, 아래, 왼, 오
    //         for(int j=0; j<4; j++){ //우선순위 0~3순위
    //             cout<<priority[p][i][j]<<" ";
    //         }cout<<'\n';
    //     }
    // }
    return 0;
}