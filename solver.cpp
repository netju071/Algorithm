#include <iostream>
#include <string>
#include <queue>
#include <stack>
#include <fstream>
#include <cstdlib>
using namespace std;


struct point{
    int x, y, val;
    point(int _x, int _y, int _val){
        x = _x;
        y = _y;
        val = _val;
    }
};

int H, W, ans;
// 현재 입력될 퍼즐의 높이와 폭의 변수이고 ans는 탐색해야 할 경로의 수를 의미 합니다.
int puzz[100][100];
// 퍼즐의 최대 크기는 100 * 100 입니다.
point start(0,0,0);
// 퍼즐에서 1값이 입력 되었을 때 즉 DFS의 시작점을 의미 합니다.
FILE *Output;
int dir[8][8] = {{0,1},{1,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1},{1,0}};
int solve(point cur){
// 시작 점에서 부터 DFS 탐색을 진행하는 합수 입니다.
// 가능한 해의 모든 경우의 수를 구할 수 있고 출력할 수 있습니다.
    if (cur.val == ans) {
        Output = fopen("output", "w");
        fprintf(Output, "%d %d\n", H, W);
        for(int i=0; i<H; i++){
            for(int k=0; k<W; k++){
                // cout << puzz[i][k] << " " ;
                printf("%02d ", puzz[i][k]);
                fprintf(Output, "%02d ", puzz[i][k]);
                
            }
            cout << endl;
            fprintf(Output, "\n");
        }
        return 1;
    }
    bool chk = (puzz[cur.x][cur.y] == cur.val);
    int ret = 0;
    puzz[cur.x][cur.y] = cur.val;
    for(int i=0; i<8; i++){
        int nextx = cur.x + dir[i][0], nexty = cur.y + dir[i][1];
        if(nextx<0 || nextx >= H || nexty<0 || nexty >= W) continue;
        if(puzz[nextx][nexty] != 0 && puzz[nextx][nexty] != cur.val+1) continue;
        ret += solve(point(nextx, nexty, cur.val+1));
    }
    if(!chk) puzz[cur.x][cur.y] = 0;
    return ret;
}

int main(int argc, char* argv[]){
    if(argc == 1){
        cout << "Please input puzzle file...";
        exit(0);
    }
    FILE *input = fopen(argv[1], "r");
    if(input==NULL){
        cout << "Not exist puzzle file...";
        exit(0);
    }
    freopen(argv[1], "r", stdin);
    cin >> H >> W;
    ans = H * W;
    
    for(int i=0; i<H; i++){
        for(int k=0; k<W; k++){
            cin >> puzz[i][k];
            if(puzz[i][k] == 1)
                start = point(i, k, 1);
            else if(puzz[i][k] == -1)
                ans--;
        }
    }
    cout << solve(start) << "\n";
}