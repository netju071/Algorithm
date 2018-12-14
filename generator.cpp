#include <cstdio>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <algorithm>
#include <set>

#define MAX_NUMBER 99 //N의 최대 최소
#define MIN_NUMBER 5
#define UNIQUE_SOLUTION 1 // 해가 1개 Check
#define NO_SOLUTION 0 
#define LIMIT_DISTANCE 13 //최초 수에 대해서 가장 먼 거리의 숫자
#define MAX_CALL_NUMBER 10000000 //
#define START_POS 1 // 숫자

using namespace std;
struct xy { int x, y; };
int dx[] = { 1, 1, -1, -1, 1, -1, 0, 0 };
int dy[] = { 1, -1, -1, 1, 0, 0, 1, -1 };

FILE *Solution, *Puzzle; //입출력

int minx, maxx, miny, maxy; //퍼즐 바운더리를 설정 -> 초기 퍼즐 n * n (데이터가 들어가지 않는 곳을 압축)
int N, cnt; 

vector<vector<int> > puzzle_map; 
xy pos[MAX_NUMBER + 1]; // 각 숫자의 위치를 저장하는 배열 (각 숫자의 위치를 표시)
set<int> remain_pos; // 현재 남아있는 숫자를 저장하는 set -> 13칸 숫자를 알기 위해서 (하나 하나씩 뺌 한칸 갈때마다)

int input()
{
   int ret;
   do
   {
      printf("%d ~ %d 사이의 값을 넣어주세요 : ", MIN_NUMBER, MAX_NUMBER);
      scanf("%d", &ret);
   } while (ret<MIN_NUMBER || ret>MAX_NUMBER);
   return ret;
}

void init(int N)
{
   // N * N 배열의 퍼즐 판을 만드는 함수 입니다. 전체 값을 -1로 초기화 합니다.

   //기존 퍼즐 맵을 클리어하고, -1로 할당(1~n까지를 set)
   
   cnt = 0;
   puzzle_map.clear();
   puzzle_map.assign(N, vector<int>(N, -1));
   minx = maxx = miny = maxy = N / 2;
   for (int i = 1; i <= N; i++)
   {
      remain_pos.insert(i);
   }
}

bool create_solution(int x, int y, int c) //재귀 함수
{
   // 정답파일을 만드는 함수 입니다. 랜덤방향 DFS 탐색으로 n개의 경로를 생성합니다.
   // 시작점은 퍼즐의 중심으로 하며 그 이유는 퍼즐이 치우쳐서 경로 탐색을 하는 경우를 막기 위해서입니다.
   // minx maxx miny maxy는 생성된 정답파일의 boundary를 의미합니다.
   // chk는 현재 남아 있는 방향(remain) 만큼의 방향을 가지고 있는 배열입니다.
   // 해당 방향으로 탐색된 후에는 해당 방향을 맨 끝으로 보내고 remain의 크기를 줄이기 때문에 더 이상 탐색하지 않습니다.
   // 이 함수를 통해 1~N 까지의 경로를 퍼즐 판에 만들게 됩니다.


   if (c > N) return true; //다 채워진 경우(N까지 다 채운 경우)
      if (x < 0 || x >= N || y < 0 || y >= N || puzzle_map[x][y] > 0) return false; //바운더리를 벗어나거나 퍼즐 맵이 채워져있으면 

   pos[c] = { x, y }; //포지션을 저장
   puzzle_map[x][y] = c; //퍼즐 맵에 저장
   minx = min(minx, x); //바운더라 재설정
   maxx = max(maxx, x);
   miny = min(miny, y);
   maxy = max(maxy, y);

   int chk[8];
   for (int i = 0; i < 8; i++) //8개의 방향을 탐색 하는데에 백트래킹을 위해 (이미 한번 선택한 방향은 끝 점이라 교체)
      chk[i] = i;

   int remain = 8;
   int dir;

   do
   {
      if (remain == 0)
      {
         puzzle_map[x][y] = 0;
         return false;
      }

      dir = rand() % remain;
      swap(chk[dir], chk[remain - 1]);
      dir = chk[--remain];
   } while (!create_solution(x + dx[dir], y + dy[dir], c + 1)); //
   return true;
}

int find_solution(int x, int y, int c)
{
   // 정답 파일이 만들어진 상태에서 단계별로 빈칸을 만들어갈 때에 
   // 빈칸을 생성할 때마다 유일한 해를 가지는지를 판단하는 함수 입니다.
   // 이를 위해 앞서 구현한 solver함수를 사용해 가능한 정답 개수를 반환하여 유일해를 판단할 수 있습니다.


   if (cnt++ > MAX_CALL_NUMBER) throw 0;   // 재귀 호출 때 마다 카운트를 해서 그 수가 임계값을 넘으면 종료

   if (c == N) return 1; //찾았다
   bool chk = (puzzle_map[x][y] == c); // 현재 위치한 칸이 0인지 아닌지 기억하는 변수 -> 현재 위치가 0인지 그 이상인지 기억해야함 (들렷는지 안들렷는지 알아야함) 
   int ret = 0; 
   puzzle_map[x][y] = c;
   for (int i = 8; i--;)
   {
      int nx = x + dx[i], ny = y + dy[i];
      if (nx<minx || nx>maxx || ny<miny || ny>maxy || (puzzle_map[nx][ny] != 0 && puzzle_map[nx][ny] != c + 1)) continue;
      ret += find_solution(nx, ny, c + 1);
   }

   if (!chk) puzzle_map[x][y] = 0;
   return ret;
}

bool create_puzzle(int no_remain)
{  
   // 정답경로가 만들어진 상황에서 빈칸을 만드는 함수 즉 generator의 핵심적인 부분입니다.
   // 시작점(1), 끝점(N)은 고정시키기 위하여 remain  = N-2;  입니다.
   // select 배열은 지워야할 점들의 index를 가지고 있습니다.
   // 지워야 할 index가 정해 졌으면 해당 인덱스의 pos.x, pos.y = 0  즉 빈칸을 생성한 후에
   // 위의 find_solution 함수를 통해 유일해의 가능성을 판단합니다.
   // 유일해가 불가능 한 경우 즉 find_solution() > 1 인 경우에 puzzle_map 의 값은 원래의 값으로 되돌립니다.
   // erase_pos_cur - erase_pos > 13 인 경우 즉 현재 빈칸을 만들 점과 이전 빈칸의 점의 경로의 차이 값이 13 초과인 경우에는 빈칸을 생성하지 않습니다.
   // 최대 N/3개 만큼 빈칸을 만들도록 설계하였으며
   // erase 배열은 현재 빈칸의 개수를 의미합니다. 
   // 초기에는 2*N/3개를 뚫는게 목표이고 erase * 2 > n즉 N/2 보다 뚫지 못하였을 경우
   // 다시 문제를 생성하게 됩니다.

   
      // 적어도 시작점과 끝점 두 개는 남겨야 하기 때문에 조치
   no_remain = max(no_remain, 2); 
   int erase = 0; // 지운 칸의 수
   int remain = N - 2; // 남은 지울 수 있는 칸의 수

   vector<int> select(N + 1);  // 랜덤하게 선택하기 위한 배열
   for (int i = 1; i <= N; i++)
      select[i] = i;

   while (remain + 2 > no_remain) //지울 놈만큼 반복
   {
      int erase_pos = rand() % remain + 2;   // 지울 칸 선택

       // 현재 남아있는 숫자중 현재 지울 예정인 칸의 앞 뒤 숫자를 보고 그 차이가 13 보다 큰지 판단한다.
      auto upper_it = remain_pos.upper_bound(select[erase_pos]); //바꾼 넘버를 선택
      int upper_number, lower_number;
      upper_number = *upper_it;
      lower_number = *(--(--upper_it)); //upper넘버는 다음 숫자를 받아오고 iterator를 두번 빼서 -> 전수와 앞수의 비교를 통해 13보다 큰지 아닌지 check
      //////////////////////////////////////////////////////////////////////////////////////////

      ////////////********************REMAIN은 남아있는 놈들 select는 지워야 하는 놈들

      swap(select[erase_pos], select[remain + 1]); // 확인한 숫자는 더 이상 확인 할 필요 없기 때문에 맨 뒤로 옮겨준다.
      remain--; // 크기 줄임 -> 이미 확인한 놈은 배제시킴
      if (upper_number - lower_number > LIMIT_DISTANCE) continue;

      erase_pos = select[remain + 2]; //erase_pos에 해당 값을 빼냄
      puzzle_map[pos[erase_pos].x][pos[erase_pos].y] = 0;   // 선택한 숫자 
      remain_pos.erase(erase_pos);//erasepos값을 지워버림 -> 숫자 차이를 

      if (find_solution(pos[1].x, pos[1].y, START_POS) != UNIQUE_SOLUTION)  // 퍼즐을 풀어본 결과 유일해가 나오지 않는 경우
      {
         puzzle_map[pos[erase_pos].x][pos[erase_pos].y] = erase_pos; // 지웠던 칸을 다시 채운다.
         remain_pos.insert(erase_pos); //백트래킹 과정을 통해 다시 로드함
      }

      else erase++; //유니크 솔루션이 나와서 erase를 하나 더해줌
   }

   return erase * 2 > N; //지운 수가 절반 이상인가? 
}

void make_solution_file()
{
   // 정답 경로를 가지고 있는 퍼즐 판을 파일 출력하는 함수 입니다.


   Solution = fopen("solution", "w");
   fprintf(Solution, "%d %d\n", maxx - minx + 1, maxy - miny + 1);
   for (int i = minx; i <= maxx; i++)
   {
      for (int j = miny; j <= maxy; j++)
      {
         fprintf(Solution, "%02d ", puzzle_map[i][j]);
      }
      fprintf(Solution, "\n");
   }
   fclose(Solution);
}

void make_puzzle_file()
// hidato 문제를 파일 출력하는 함수 입니다.
{
   Puzzle = fopen("puzzle", "w");
   fprintf(Puzzle, "%d %d\n", maxx - minx + 1, maxy - miny + 1);
   for (int i = minx; i <= maxx; i++)
   {
      for (int j = miny; j <= maxy; j++)
      {
         fprintf(Puzzle, "%02d ", puzzle_map[i][j]);
      }
      fprintf(Puzzle, "\n");
   }
   fclose(Puzzle);
}

void print_puzzle()
{
   for (int i = minx; i <= maxx; i++)
   {
      for (int j = miny; j <= maxy; j++)
      {
         if (puzzle_map[i][j] == -1) printf("   ");
         else printf("%02d ", puzzle_map[i][j]);
      }
      printf("\n");
   }
}

int main()
{
   srand(time(NULL));
   N = input();
   puts("퍼즐을 만드는 중입니다...");

   while (1)
   {
      init(N); //N값을 넣어주고 반환
      create_solution(N / 2, N / 2, 1);
      make_solution_file();
      try // find_solution에서 해를 찾는데 오래 걸려는 경우 다시 만든다.
      {
         if (create_puzzle(N / 3))
         {
            make_puzzle_file();
            puts("퍼즐을 만들었습니다!");
            print_puzzle();
            return 0;
         }
      }
      catch (int exception)
      {
         continue;
      }

   }
}