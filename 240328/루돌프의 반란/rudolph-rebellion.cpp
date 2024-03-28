#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;
typedef pair<int, int> pii;

int N, M, P, C, D;
int Rr, Rc;
int santa_matrix[51][51]; //1~P 산타 번호 저장
pair<pii, pii> santa_status[31]; //{{0:생존/1:기절/2:탈락, 점수}, {r,c}}
int turn;
int santa_faint[31]; //산타가 기절한 turn을 저장

void init() {
    cin >> N >> M >> P >> C >> D;
    cin >> Rr >> Rc;
    for (int i = 0; i < P; i++) {
        int num, r, c; cin >> num >> r >> c;
        santa_matrix[r][c] = num;
        santa_status[num] = { {0,0},{r,c} };
    }
}

struct cmp {
    bool operator()(pair<pii, pii> x, pair<pii, pii> y) { //거리작음>r큼>c큼 순으로 정렬
        if (x.first.first != y.first.first)
            return x.first.first > y.first.first; //오름차순
        else if (x.second.first != y.second.first)
            return x.second.first < y.second.first;
        else return x.second.second < y.second.second;
    }
};

void interaction(int santa_num, int curX, int curY, int dirX, int dirY) { //santa_num이 (curX,curY)로 오고, (curX,curY)에 있던 산타는 dirX, dirY 방향으로 한 칸씩 밀려남
    int pushed_santa_num = santa_matrix[curX][curY];
    santa_matrix[curX][curY] = santa_num;
    santa_status[santa_num].second.first=curX;
    santa_status[santa_num].second.second=curY;
    //밀려날 곳: [curX+dirX][curY+dirY]
    if (santa_matrix[curX + dirX][curY + dirY]) {
        interaction(pushed_santa_num, curX + dirX, curY + dirY, dirX, dirY);
    }
    else if (curX + dirX<1|| curX + dirX>N|| curY + dirY<1|| curY + dirY>N) { //밀려날곳이 격자판 넘어가면
        santa_status[pushed_santa_num].first.first = 2;
    }
    else { //밀려난 곳에 다른 산타 없음
        santa_matrix[curX + dirX][curY + dirY] = pushed_santa_num;
        santa_status[pushed_santa_num].second.first=curX + dirX;
        santa_status[pushed_santa_num].second.second=curY + dirY;
    }
}

void crashByRudolph(int Mr, int Mc) { //루돌프가 움직여서 충돌난 경우
    int santa_num = santa_matrix[Rr][Rc];
    santa_status[santa_num].first.second += C; //산타는 C만큼 점수 얻음
    santa_status[santa_num].first.first = 1; //기절
    santa_faint[santa_num] = turn;
    santa_matrix[Rr][Rc] = 0; //밀려남

    if (santa_matrix[Rr + C * Mr][Rc + C * Mc]) { //밀려날 곳:[Rr+C*Mr][Rc+C*Mc]에 다른 산타가 있다면 상호작용
        interaction(santa_num, Rr + C * Mr, Rc + C * Mc, Mr, Mc); //[Rr+C*Mr][Rc+C*Mc]에 원래있던 산타는 1칸 밀리고, [Rr+C*Mr][Rc+C*Mc]에는 santa_num이 위치함
    }
    else if (Rr + C * Mr<1 || Rr + C * Mr>N || Rc + C * Mc<1 || Rc + C * Mc>N) {
        santa_status[santa_num].first.first = 2; //탈락
    }
    else { //밀려난 곳에 다른 산타 없음
        santa_matrix[Rr + C * Mr][Rc + C * Mc] = santa_num;
        santa_status[santa_num].second.first=Rr + C * Mr;
        santa_status[santa_num].second.second=Rc + C * Mc;
    }
}
void rudolph_move() {
    //가장 가까운(우선순위 높은) 산타 찾기
    priority_queue<pair<pii, pii>, vector<pair<pii, pii>>, cmp> pq;
    for (int p = 1; p <= P; p++) {
        if (santa_status[p].first.first == 2) continue;
        int Sr = santa_status[p].second.first;
        int Sc = santa_status[p].second.second;
        int dist = (Rr - Sr)*(Rr - Sr) + (Rc - Sc)*(Rc - Sc);
        pq.push({ {dist, p}, {Sr,Sc} });
    }
    //가장 가까운 산타로 돌진하기
    int near_Sr = pq.top().second.first;
    int near_Sc = pq.top().second.second;

    int Mr = 0, Mc = 0; //루돌프가 움직일 방향
    if (near_Sr - Rr < 0) Mr = -1;
    else if (near_Sr - Rr > 0) Mr = 1;
    if (near_Sc - Rc < 0) Mc = -1;
    else if (near_Sc - Rc > 0) Mc = 1;
    //루돌프 움직이기
    Rr += Mr, Rc += Mc;
    //루돌프가 움직여서 충돌난 경우
    if (santa_matrix[Rr][Rc]) {
        crashByRudolph(Mr, Mc);
    }
}

void crashBySanta(int santa_num, int Mr, int Mc) {
    santa_status[santa_num].first.second += D;
    santa_status[santa_num].first.first = 1; //기절
    santa_faint[santa_num] = turn;

    if (santa_matrix[Rr + D * Mr][Rc + D * Mc]) { //밀려날 곳:[Rr+D*Mr][Rc+D*Mc]에 다른 산타가 있다면 상호작용
        interaction(santa_num, Rr + D * Mr, Rc + D * Mc, Mr, Mc); //[Rr+C*Mr][Rc+C*Mc]에 원래있던 산타는 1칸 밀리고, [Rr+C*Mr][Rc+C*Mc]에는 santa_num이 위치함
    }
    else if (Rr + D * Mr<1 || Rr + D * Mr>N || Rc + D * Mc<1 || Rc + D * Mc>N) {
        santa_status[santa_num].first.first = 2; //탈락
    }
    else { //밀려난 곳에 다른 산타 없음
        santa_matrix[Rr + D * Mr][Rc + D * Mc] = santa_num;
        santa_status[santa_num].second.first=Rr + D * Mr;
        santa_status[santa_num].second.second=Rc + D * Mc;
    }
}
void santa_move() {
    for(int i = 1; i <= P; i++) {
        if (santa_status[i].first.first == 2) continue;
        if (santa_status[i].first.first == 1) {
            if (santa_faint[i] + 2 == turn) santa_status[i].first.first = 0;
            else continue;
        }

        int Sr = santa_status[i].second.first;
        int Sc = santa_status[i].second.second;
        int dr[4] = {0,1,0,-1}; //좌,하,우,상(우선순위 역순)
        int dc[4] = {-1,0,1,0};
        int direction=-1, distance=5001; //움직일 방향, 움직인 칸에서 루돌프와의 거리
        for (int d = 0; d < 4; d++) {
            if (santa_matrix[Sr + dr[d]][Sc + dc[d]]) continue;
            if(Sr + dr[d]<1 || Sr + dr[d]>N || Sc + dc[d]<1 || Sc + dc[d]>N) continue;
            int cur_dist = (Sr + dr[d] - Rr)*(Sr + dr[d] - Rr) + (Sc + dc[d] - Rc)*(Sc + dc[d] - Rc);
            if (distance >= cur_dist) {
                distance = cur_dist;
                direction = d;
            }
        }
        if (direction == -1) continue;
        if (distance > (Sr - Rr)*(Sr - Rr) + (Sc - Rc)*(Sc - Rc)) continue;

        santa_matrix[Sr][Sc] = 0;
        if (Rr == Sr + dr[direction] && Rc == Sc + dc[direction]) { //산타가 움직여서 루돌프와 충돌
            crashBySanta(i, dr[(direction + 2) % 4], dc[(direction + 2) % 4]); //산타번호, 반대방향
        }
        else {
            santa_matrix[Sr + dr[direction]][Sc + dc[direction]] = i;
            santa_status[i].second.first=Sr + dr[direction];
            santa_status[i].second.second=Sc + dc[direction];
        }
    }
}

bool give_score() {
    int alive = 0;
    for (int i = 1; i <= P; i++) {
        if (santa_status[i].first.first != 2) {
            alive++;
            santa_status[i].first.second++;
        }
    }
    if (alive == 0) {
        return false; //게임 종료
    }
    else return true;
}

void print_score() {
    for (int i = 1; i <= P; i++) {
        cout<< santa_status[i].first.second<<" ";
    }
    cout<<'\n';
}
void print_status(){
    cout<<"rudloph "<<Rr<<" "<<Rc<<'\n';
    for(int i=1; i<=P; i++){
        cout<<"#"<<i<<" "<<santa_status[i].second.first<<" "<<santa_status[i].second.second<<'\n';
    }
}

int main() {

    init();

    for (turn = 1; turn <= M; turn++) {

        rudolph_move();

        santa_move();

        bool game = give_score();
        if (game == false) break;

//        print_status();
    }

    print_score();

    return 0;
}