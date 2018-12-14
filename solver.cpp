#include <iostream>
#include <string>
#include <queue>
#include <stack>
#include <fstream>
#include <cstdlib>
using namespace std;

struct point
// point는 구조체는 (x, y)위치에 value를 저장하는 것을 의미합니다.
{
   int x, y, val;
   point(int _x, int _y, int _val)
   {
      x = _x;
      y = _y;
      val = _val;
   }
};

// H와 W는 퍼즐의 높이와 폭을, ans는 탐색해야 할 경로의 수를 의미합니다.
// 퍼즐의 최대 크기는 100 * 100 입니다.
// start는 DFS의 시작점을 의미 합니다.
int H, W, ans;
int puzz[100][100];
point start(0,0,0);
FILE *Output;
int dir[8][2] = {{0,1},{1,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1},{1,0}};

int solve(point cur)
{
// 시작 점에서 부터 DFS 탐색을 진행하는 합수 입니다.
// 가능한 해의 모든 경우의 수를 구할 수 있으며, 출력할 수 있습니다.

   if (cur.val == ans) // 1~ans를 모두 채웠으면, 풀이파일을 저장하고 1을 반환합니다.
   {
      Output = fopen("output", "w");
      fprintf(Output, "%d %d\n", H, W);
      for(int i=0; i<H; i++)
      {
         for(int k=0; k<W; k++)
         {
            printf("%02d ", puzz[i][k]);
            fprintf(Output, "%02d ", puzz[i][k]);          
         }
         cout << endl;
         fprintf(Output, "\n");
      }
      return 1;
   }
   
   bool chk = (puzz[cur.x][cur.y] == cur.val); // 현위치의 상태를 저장하고,
   puzz[cur.x][cur.y] = cur.val;               // cur.val을 저장합니다.

   // 다음 값(cur.val+1)을 탐색하여, 재귀적으로 해의 개수를 반환받습니다.
   int ret = 0;
   for(int i=0; i<8; i++)
   {
      int nextx = cur.x + dir[i][0], nexty = cur.y + dir[i][1];
      if(nextx<0 || nextx >= H || nexty<0 || nexty >= W) continue;
      if(puzz[nextx][nexty] != 0 && puzz[nextx][nexty] != cur.val+1) continue;
      ret += solve(point(nextx, nexty, cur.val+1));
   }
   
   if(!chk) puzz[cur.x][cur.y] = 0; // 현위치의 값을 복원하고,
   return ret;                      // 해의 개수를 반환합니다.
}

int main(int argc, char* argv[])
{
   if(argc == 1)
   {
      cout << "Please input puzzle file...";
      exit(0);
   }
   
   FILE *input = fopen(argv[1], "r");
   if(input == NULL)
   {
      cout << "Not exist puzzle file...";
      exit(0);
   }
   
   freopen(argv[1], "r", stdin);
   cin >> H >> W;
   ans = H * W;
    
   for(int i=0; i<H; i++)
   {
      for(int k=0; k<W; k++)
      {
         cin >> puzz[i][k];
         if(puzz[i][k] == 1)
            start = point(i, k, 1);
         else if(puzz[i][k] == -1)
            ans--;
        }
   }
   cout << solve(start) << "\n";
}
