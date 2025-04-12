#include <iostream>
#include <vector>
#include <queue>
#include <tuple>
#include <algorithm>
using namespace std;

int n, m;

int mat[16][16];

int dx[4] = {-1, 0, 0, 1};
int dy[4] = {0, -1, 1, 0};

struct PERSON {
    int r = -1, c = -1; //사람 위치(배정되기 전에는 -1)
    int pr, pc; //가고자 하는 편의점 위치
};
vector<PERSON> person(31); //1~m

int numOfArrive = 0;

struct NODE {
    int x, y;
};
vector<NODE> canNotGo;

bool inRange(int x, int y) {
    if (0 <= x && x < n && 0 <= y && y < n) return true;
    else return false;
}

void init() {
    cin >> n >> m;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cin >> mat[i][j];
        }
    }
    for (int i = 1; i <= m; i++) {
        cin >> person[i].pr >> person[i].pc;
        mat[--person[i].pr][--person[i].pc] = 2;
    }
}

int dist[16][16];
vector<tuple<int, int, int>> basecamp; //최단거리, 행, 열
void initBfs() {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) dist[i][j] = 0;
    }
    basecamp.clear();
}
void bfs(int r, int c) { //(r,c)에서 출발하여 각 격자까지의 거리 체크
    queue<NODE> q;
    q.push({ r,c });
    dist[r][c] = 1;
    while (!q.empty()) {
        NODE cur = q.front(); q.pop();
        if (mat[cur.x][cur.y] == 1) {
            basecamp.push_back({ dist[cur.x][cur.y], cur.x, cur.y });
        }
        for (int d = 0; d < 4; d++) {
            int nx = cur.x + dx[d];
            int ny = cur.y + dy[d];
            if (!inRange(nx, ny)) continue;
            if (dist[nx][ny]) continue;
            if (mat[nx][ny] == -1) continue;
            q.push({ nx, ny });
            dist[nx][ny] = dist[cur.x][cur.y] + 1;
        }
    }
}
bool cmp(const tuple<int, int, int>& a, const tuple<int, int, int>& b) {
    if (get<0>(a) != get<0>(b)) return get<0>(a) < get<0>(b);
    else if (get<1>(a) != get<1>(b)) return get<1>(a) < get<1>(b);
    else if (get<2>(a) != get<2>(b)) return get<2>(a) < get<2>(b);
}
void moveToBasecamp(int num) { //num번 사람이 원하는 편의점과 가까운 베이스캠프로 이동, 지나갈 수 없음 처리

    initBfs();

    bfs(person[num].pr, person[num].pc);

    sort(basecamp.begin(), basecamp.end(), cmp);

    //가장 가까운 베이스캠프
    int br = get<1>(basecamp[0]);
    int bc = get<2>(basecamp[0]);

    person[num].r = br;
    person[num].c = bc;
    mat[br][bc] = -1;
}

void moveToStore(int num) { //num번 사람을 편의점 방향으로 1칸 움직

    if (person[num].r == -1) return; //아직 격자 밖(배정 전)
    if (person[num].r == person[num].pr && person[num].c == person[num].pc) return; //이미 도착

    initBfs();

    bfs(person[num].pr, person[num].pc);

    int minDir = 4;
    int minDist = 9876;
    for (int d = 0; d < 4; d++) {
        int nx = person[num].r + dx[d];
        int ny = person[num].c + dy[d];
        if (!inRange(nx, ny)) continue;
        if (mat[nx][ny] == -1) continue;
        if (dist[nx][ny] < minDist) {
            minDist = dist[nx][ny];
            minDir = d;
        }
    }
    person[num].r += dx[minDir];
    person[num].c += dy[minDir];

    if (person[num].r == person[num].pr && person[num].c == person[num].pc) {
        canNotGo.push_back({ person[num].pr, person[num].pc });
        numOfArrive++;
    }
}
void checkCanNotGo() {
    for (NODE c : canNotGo) {
        mat[c.x][c.y] = -1;
    }
}
int main() {
    ios_base::sync_with_stdio(false); cin.tie(NULL);

    init();

    for (int t = 1; ; t++) {
        canNotGo.clear();
        
        for (int i = 1; i <= m; i++) {
            moveToStore(i); //배정된 사람은 편의점으로 이동
        }
        
        if (t <= m) {
            moveToBasecamp(t);
        }
        checkCanNotGo();

        if (numOfArrive == m) {
            cout << t;
            break;
        }
    }

    return 0;
}