#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;
typedef pair<int,int> pii;

queue<int> monster[5][5]; //해당 위치 몬스터들의 '방향' 저장
int mSize[5][5]; //이동한 몬스터가 또 이동하는 것을 방지하기위해 1단계에서 각 칸의 몬스터 개수를 저장해둠
queue<pair<pii,int>> egg; //알의 {{r, c}, 방향}}
queue<int> dead[5][5]; //시체가 생성된 '턴' 저장

int m, t; //몬스터 마리수, 턴수
int pr, pc; //팩맨의 위치 (행, 열)

int dr[8] = {-1, -1, -1, 0, 1, 1, 1, 0};
int dc[8] = {1, 0, -1, -1, -1, 0, 1, 1};

string pacman[4] = {"u", "l", "d", "r"};
vector<string> pacMove; // {uuu, uul, uud, uur, ulu, ull, uld, ulr, udu, udl, udd, udr, ... }


void init(){
    //팩맨 이동할 때 사용할 64개의 이동방법 만들기
    for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
            for(int r=0; r<4; r++){
                pacMove.push_back(pacman[i]+pacman[j]+pacman[r]);
            }
        }
    }

    //입력받기
    cin >> m >> t;
    cin >> pr >> pc;

    for(int i=0; i<m; i++){
        int r, c, d;
        cin >> r >> c >> d;
        if(d==8) d=0; //나중에 d%8 해줄거라서 8은 0으로 변경
        monster[r][c].push(d);
    }
}

void copyMonster(){
    for(int i=1; i<=4; i++){
        for(int j=1; j<=4; j++){
            mSize[i][j] = monster[i][j].size();
            for(int k=0; k<mSize[i][j]; k++){
                egg.push({{i,j},monster[i][j].front()});
                monster[i][j].push(monster[i][j].front());
                monster[i][j].pop();
            }
        }
    }
}

void moveMonster(){
    for(int i=1; i<=4; i++){
        for(int j=1; j<=4; j++){
            for(int k=0; k<mSize[i][j]; k++){

                int cur_d = monster[i][j].front();
                int can_move=0;
                for(int d=0; d<8; d++){ //현재 방향부터 시작해서 8방향 체크
                    int ni=i+dr[(cur_d+d)%8]; //몬스터가 갈 다음 칸의 행
                    int nj=j+dc[(cur_d+d)%8]; //몬스터가 갈 다음 칸의 열
                    if(ni<1||ni>4||nj<1||nj>4 || !dead[ni][nj].empty() || ni==pr && nj==pc) {
                        //불가능
                        continue;
                    }
                    else{
                        //가능
                        monster[ni][nj].push((cur_d+d)%8);
                        monster[i][j].pop();
                        can_move=1;
                        break;
                    }
                }
                if(!can_move){
                    monster[i][j].push(cur_d);
                    monster[i][j].pop();
                }
            }
        }
    }
}

void movePacman(int turn){
    string mxPath="";
    int mxEat = 0;
    for(int pm=63; pm>=0; pm--){ //우선순위 낮은거->높은거 순으로 갱신
        int vis[5][5]={0,};
        string move = pacMove[pm]; //예시: move="uld"
        int eat=0;
        int tmp_pr = pr, tmp_pc = pc; //현재 팩맨 위치
        int nxt_pr, nxt_pc; //팩맨 다음 위치

        int can_go=1;
        for(int m=0; m<move.length(); m++){
            if(move[m]=='u'){
                nxt_pr=tmp_pr-1;
                nxt_pc=tmp_pc;
            }
            else if(move[m]=='l'){
                nxt_pr=tmp_pr;
                nxt_pc=tmp_pc-1;
            }
            else if(move[m]=='d'){
                nxt_pr=tmp_pr+1;
                nxt_pc=tmp_pc;
            }
            else if(move[m]=='r'){
                nxt_pr=tmp_pr;
                nxt_pc=tmp_pc+1;
            }

            if(nxt_pr<1||nxt_pr>4||nxt_pc<1||nxt_pc>4){
                can_go=0;
                break;
            }
            if(can_go){
                if(!vis[nxt_pr][nxt_pc]){
                    eat+=monster[nxt_pr][nxt_pc].size();
                    vis[nxt_pr][nxt_pc]=1;
                }
                tmp_pr=nxt_pr, tmp_pc=nxt_pc;
            }
        }
        if(can_go && eat>=mxEat){
            mxEat = eat;
            mxPath=move;
        }
    }
    //mxPath로 이동하며 시체 생성하기
    for(int m=0; m<mxPath.size(); m++){
        if(mxPath[m]=='u'){
            pr=pr-1;
            pc=pc;
        }
        else if(mxPath[m]=='l'){
            pr=pr;
            pc=pc-1;
        }
        else if(mxPath[m]=='d'){
            pr=pr+1;
            pc=pc;
        }
        else if(mxPath[m]=='r'){
            pr=pr;
            pc=pc+1;
        }
        // 이동한 칸인 (pr, pc)에 몬스터 있으면, 죽이고 시체로 만들자
        int kill = monster[pr][pc].size(); //죽인 몬스터 개수
        while(!monster[pr][pc].empty()){
            monster[pr][pc].pop();
        }
        while(kill--){
            dead[pr][pc].push(turn); //죽인만큼 시체 생성
        }
    }
}

void removeDead(int turn){
    for(int i=1; i<=4; i++){
        for(int j=1; j<=4; j++){
            // 큐는 접근하기 전에 empty체크 꼭!!!
            while((!dead[i][j].empty()) && (turn-2 >= dead[i][j].front())) { // ==라고 해도 됨
                dead[i][j].pop();
            }
        }
    }
}

void bornMonster(){
    while(!egg.empty()){
        pair<pii,int> born = egg.front();
        egg.pop();
        monster[born.first.first][born.first.second].push(born.second);
    }
}

int main() {

    init();

    //턴 시작!
    for(int turn=1; turn<=t; turn++){

        //1. 몬스터 복제 시도
        copyMonster();

        //2. 몬스터 이동
        moveMonster();


        //3. 팩맨 이동 (vis체크 해야함..)
        movePacman(turn);

        //4. 몬스터 시체 소멸
        removeDead(turn);

        //5. 몬스터 복제 완성
        bornMonster();

//        cout<<"5\n";
//        for(int i=1; i<=4; i++){
//            for(int j=1; j<=4; j++){
//
//                cout<<monster[i][j].size()<<" ";
//            }
//            cout<<'\n';
//        }
    }



    //정답 출력
    int answer = 0;
    for(int i=1; i<=4; i++) {
        for (int j = 1; j <= 4; j++) {
            answer += monster[i][j].size();
        }
    }
    cout<<answer;
    return 0;
}

//6 1
//2 3
//1 3 5
//2 2 7
//2 4 1
//3 4 6
//4 1 3
//4 2 2