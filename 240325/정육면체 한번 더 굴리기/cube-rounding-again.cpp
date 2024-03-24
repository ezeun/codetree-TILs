#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;
typedef pair<int,int> pii;

int n, m;
int mat[21][21];
int score[21][21];
int answer = 0;
int dice[6] = {6, 5, 2 ,4 ,3, 1}; //바닥, 상, 하, 좌, 우, 천장
int direction = 4; //초기방향 (1:상, 2:하, 3:좌, 4:우)
int dice_x=1, dice_y=1;

void init(){
    cin >> n >> m;
    for(int i=1; i<=n; i++){
        for(int j=1; j<=n; j++){
            cin >> mat[i][j];
        }
    }
}

void bfs(){
    int dx[4] = {0,0,1,-1};
    int dy[4] = {-1,1,0,0};
    for(int i=1; i<=n; i++){
        for(int j=1; j<=n; j++){

            //score[i][j] : (i,j)로 이동하면 얻는 점수
            int vis[21][21]={0,};
            queue<pii> q;
            int sum=0;

            q.push({i,j});
            vis[i][j]=1;

            while(!q.empty()){
                pii cur = q.front(); q.pop();
                sum+=mat[cur.first][cur.second];
                for(int d=0; d<4; d++){
                    int nx = cur.first+dx[d];
                    int ny = cur.second+dy[d];
                    if(nx<1||nx>n||ny<1||ny>n) continue;
                    if(vis[nx][ny]) continue;
                    if(mat[nx][ny]==mat[cur.first][cur.second]){
                        q.push({nx,ny});
                        vis[nx][ny]=1;
                    }
                }
            }

            score[i][j] = sum;
        }
    }
}

void print_score(){
    for(int i=1; i<=n; i++){
        for(int j=1; j<=n; j++){
            cout<<score[i][j]<<" ";
        }
        cout<<'\n';
    }
}

bool move(int direction){
    //주사위 위치 변경, 주사위 배열 변경
    if(direction==1){ //상
        if(dice_x-1<1) return false;
        else{
            dice_x -=1;
            int tmp[6]={dice[1],dice[5],dice[0],dice[3],dice[4],dice[2]};
            for(int i=0; i<6; i++) dice[i]=tmp[i];
            return true;
        }
    }
    else if(direction==2){ //하
        if(dice_x+1>n) return false;
        else{
            dice_x +=1;
            int tmp[6]={dice[2],dice[0],dice[5],dice[3],dice[4],dice[1]};
            for(int i=0; i<6; i++) dice[i]=tmp[i];
            return true;
        }
    }
    else if(direction==3){ //좌
        if(dice_y-1<1) return false;
        else{
            dice_y -=1;
            int tmp[6]={dice[3],dice[1],dice[2],dice[5],dice[0],dice[4]};
            for(int i=0; i<6; i++) dice[i]=tmp[i];
            return true;
        }
    }
    else if(direction==4){ //우
        if(dice_y+1>n) return false;
        else{
            dice_y +=1;
            int tmp[6]={dice[4],dice[1],dice[2],dice[0],dice[5],dice[3]};
            for(int i=0; i<6; i++) dice[i]=tmp[i];
            return true;
        }
    }
}

int main() {

    init();

    bfs();
//    print_score();

    while(m--){

        //direction 방향으로 한 칸 이동
        int res = move(direction);
        if(res==false){ //이동할 수 없다면 방향 반사 후 이동
            if(direction==1) direction=2;
            else if(direction==2) direction=1;
            else if(direction==3) direction=4;
            else if(direction==4) direction=3;
            move(direction);
        }

        //점수 획득
        answer += score[dice_x][dice_y];

        //주사위 아랫면을 참고하여 방향 재설정
        if(dice[0]>mat[dice_x][dice_y]){
            if(direction==1) direction=4;
            else if(direction==2) direction=3;
            else if(direction==3) direction=1;
            else if(direction==4) direction=2;
        }
        else if(dice[0]<mat[dice_x][dice_y]){
            if(direction==1) direction=3;
            else if(direction==2) direction=4;
            else if(direction==3) direction=2;
            else if(direction==4) direction=1;
        }
    }

    cout<<answer;
    return 0;
}