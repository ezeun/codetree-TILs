#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;
typedef pair<int, int> pii;

int L, N, Q;
int mat1[41][41]; // 0:빈칸, 1:함정, 2:벽
int mat2[41][41]; //기사 번호
int trap[41][41]; //함정 누적합
int stamina[31]; //초기 체력

struct Knight{
    int r, c; //좌상단 위치
    int h, w; //세로, 가로 길이
    int k; //체력
    bool alive;
}knight[31];

int dr[4] = {-1,0,1,0}; //위, 오, 아, 왼
int dc[4] = {0, 1,0,-1};

int tmp_mat2[41][41];
Knight tmp_knight[31];

vector<int> pushed_knight;
/////////////////////////////////////////////

void init(){
    cin>>L>>N>>Q;

    for(int i=1; i<=L; i++){
        for(int j=1; j<=L; j++){
            cin>>mat1[i][j];
            if(mat1[i][j]==1) trap[i][j]=1;
        }
    }

    for(int i=1; i<=L; i++){
        for(int j=1; j<=L; j++){
            trap[i][j] = trap[i-1][j]+trap[i][j-1]-trap[i-1][j-1]+trap[i][j];
        }
    }

    for(int i=1; i<=N; i++){
        cin>> knight[i].r >> knight[i].c >> knight[i].h >> knight[i].w >> knight[i].k;
        knight[i].alive = true;
        stamina[i]=knight[i].k;
        for(int x=knight[i].r; x<knight[i].r+knight[i].h; x++){
            for(int y=knight[i].c; y<knight[i].c+knight[i].w; y++){
                mat2[x][y]=i;
            }
        }
    }
}
int calc_trap(int r, int c, int h, int w){
    // 좌상단 r,c
    // 우하단 r+h-1, c+w-1
    int rx = r+h-1, ry = c+w-1;
    return trap[rx][ry] - trap[rx-1][ry] - trap[r][c-1] + trap[r-1][c-1];
}

void copy(){
    for(int i=1; i<=L; i++){
        for(int j=1; j<=L; j++){
            tmp_mat2[i][j]=mat2[i][j];
        }
    }
    for(int i=1; i<=N; i++){
        tmp_knight[i]=knight[i];
    }
}
void restore(){
    for(int i=1; i<=L; i++){
        for(int j=1; j<=L; j++){
            mat2[i][j]=tmp_mat2[i][j];
        }
    }
    for(int i=1; i<=N; i++){
        knight[i]=tmp_knight[i];
    }
}

bool move(int i, int d){

    // i번 기사를 d방향으로 밀기
    for(int x=knight[i].r; x<knight[i].r+knight[i].h; x++){
        for(int y=knight[i].c; y<knight[i].c+knight[i].w; y++){
            mat2[x][y]=0;
        }
    }
    knight[i].r+=dr[d];
    knight[i].c+=dc[d];
    for(int x=knight[i].r; x<knight[i].r+knight[i].h; x++){
        for(int y=knight[i].c; y<knight[i].c+knight[i].w; y++){
            if(mat1[x][y]==2 || x<1 || x>L || y<1 || y>L){
                return false; //끝에 벽이 있으면 모든 기사는 이동할 수 없음
            }
            if(mat2[x][y]){
                pushed_knight.push_back(mat2[x][y]);
                return move(mat2[x][y], d); //이동하려는 칸에 다른 기사 있으면 연쇄적으로 한칸 밀려남
            }
            mat2[x][y]=i;
        }
    }
    return true;
}

int calc_damage(){
    int sum=0;
    for(int i=1; i<=N; i++){
        if(knight[i].alive==false) continue;
        sum+=stamina[i]-knight[i].k;
    }
    return sum;
}

int main() {

    init();

    for(int q=0; q<Q; q++){
        int i, d; cin>> i >> d; //i번 기사에게 d방향으로 한 칸 이동하라는 명령
        if(knight[i].alive==false) continue;

        copy();
        pushed_knight.clear();
        if(move(i, d)==false){ //벽을 만나면 mat2, knight 원상복구
            restore();
        }
        else{ //밀린 기사들 체력 깎기
            for(auto &kn : pushed_knight){
                knight[kn].k = calc_trap(knight[kn].r,knight[kn].c,knight[kn].h,knight[kn].w);
                if(knight[kn].k<=0) {
                    knight[kn].alive=false;
                    for(int x=knight[kn].r; x<knight[kn].r+knight[kn].h; x++){
                        for(int y=knight[kn].c; y<knight[kn].c+knight[kn].w; y++){
                            mat2[x][y]=0;
                        }
                    }
                }
            }
        }
    }

    cout<< calc_damage();
    return 0;
}