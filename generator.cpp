#include <cstdio>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <algorithm>
#include <set>

#define MAX_NUMBER 99            // 입력받는 N은 MAX_NUMBER 보다 작거나 같고
#define MIN_NUMBER 5             // MIN_NUMBER 보다 크거나 같아야합니다.
#define UNIQUE_SOLUTION 1        // 해의 개수가 1인 경우를 유일해로 정의합니다. 
#define LIMIT_DISTANCE 13        // 알려진 숫자들의 차이는 LIMIT_DISTANCE를
                                 // 초과할 수 없습니다.
#define MAX_CALL_NUMBER 10000000 // 최대 재귀호출 횟수를 제한합니다.
#define START_POS 1              // START_POS에서 탐색을 시작합니다.

using namespace std;
struct xy { int x, y; };
int dx[] = { 1, -1, 0, 0, 1, 1, -1, -1 };
int dy[] = { 0, 0, 1, -1, 1, -1, -1, 1 };

// Solution과 Puzzle은 입출력을 위해 사용되는 변수입니다.
// minx, maxx, miny, maxy는 퍼즐의 바운더리를 의미합니다.
// N은 퍼즐에서 최대 숫자를 의미합니다.
// cnt는 재귀호출 횟수를 카운트하는 변수입니다.
// pos는 1~n 순서로 각 위치를 저장하는 배열입니다.
// remain_pos는 남아있는(지우지 않은) 숫자들의 집합입니다.
FILE *Solution, *Puzzle;
int minx, maxx, miny, maxy;
int N, cnt; 
vector<vector<int> > puzzle_map;
xy pos[MAX_NUMBER + 1];
set<int> remain_pos;

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
   // N * N 배열의 퍼즐 판을 생성하고, 프로그램 수행에 필요한 변수들을 초기화 합니다.

   cnt = 0;
   puzzle_map.clear();
   puzzle_map.assign(N, vector<int>(N, -1));
   minx = maxx = miny = maxy = N / 2;
   for (int i = 1; i <= N; i++)
   {
      remain_pos.insert(i);
   }
}

bool create_solution(int x, int y, int c) 
{
   // 정답파일을 만드는 함수입니다. 랜덤한 방향으로 DFS 탐색을 수행하여 경로를 생성합니다.
   // 퍼즐의 중심에서 시작하여 패턴이 단조로워지는 것을 방지하였습니다. 
   // minx maxx miny maxy는 생성된 정답파일의 바운더리를 의미합니다.
   // chk는 현위치에서 인접한 8방에 대한 인덱스를 갖는 배열이며,
   // 탐색하지 않은 인덱스 -> 탐색한 인덱스 순으로 정렬상태를 유지합니다.
   // remain은 chk에서 탐색하지 않은 방향의 개수를 의미합니다.
   // 이 함수를 통해 1~N 까지의 경로를 퍼즐 판에 만들게 됩니다.

   if (c > N) return true; // 1~N까지 모든 숫자를 채운 경우, true를 반환합니다.
   // 반면, 현위치가 바운더리를 벗어났거나 이미 값이 채워진 경우, false를 반환합니다. 
   if (x < 0 || x >= N || y < 0 || y >= N || puzzle_map[x][y] > 0) return false; 

   pos[c] = { x, y };    // puzzle_map에 현재 숫자를 저장하고, 그 위치를 pos에 저장합니다
   puzzle_map[x][y] = c;
   minx = min(minx, x);  // 바운더리를 재설정 합니다.
   maxx = max(maxx, x);
   miny = min(miny, y);
   maxy = max(maxy, y);

   int chk[8]; // 인접한 8방에 대해 인덱스를 부여합니다.
   for (int i = 0; i < 8; i++)
      chk[i] = i;

   int remain = 8;
   int dir;

   // 랜덤한 방향으로 DFS 탐색을 실시합니다.
   do
   {
      if (remain == 0) // 인접한 8방을 모두 탐색한 경우, false를 반환합니다.
      {
         puzzle_map[x][y] = 0;
         return false;
      }

      dir = rand() % remain; // 탐색하지 않은 방향중에서 랜덤한 방식으로
                             // 한 방향에 대한 인덱스를 선택합니다.
      swap(chk[dir], chk[remain - 1]); // 선택한 방향은 탐색한 방향으로 표시하고
      dir = chk[--remain];             // 다음 위치를 탐색합니다.
   } while (!create_solution(x + dx[dir], y + dy[dir], c + 1));

   return true;
}

int find_solution(int x, int y, int c)
{
   // 현재 퍼즐 맵으로 부터 가능한 모든 해의 개수를 반환하는 함수입니다.
   
   if (cnt++ > MAX_CALL_NUMBER) throw 0; // 재귀호출 횟수를 제한합니다.
   if (c == N) return 1;                 // 1~N을 모두 채웠으면, 1을 반환합니다.

   bool chk = (puzzle_map[x][y] == c); // 현위치의 상태를 저장하고,
   puzzle_map[x][y] = c;               // c값을 저장합니다.

   // 다음 값(c+1)을 탐색하여, 재귀적으로 해의 개수를 반환받습니다.
   int ret = 0;
   for (int i = 8; i--;)
   {
      int nx = x + dx[i], ny = y + dy[i];
      if (nx < minx || nx > maxx || ny < miny || ny > maxy ||
         (puzzle_map[nx][ny] != 0 && puzzle_map[nx][ny] != c + 1)) continue;
      ret += find_solution(nx, ny, c + 1);
   }

   if (!chk) puzzle_map[x][y] = 0; // 현위치의 값을 복원하고,
   return ret;                     // 해의 개수를 반환합니다.
}

bool create_puzzle(int no_remain)
{
   // 정답파일이 만들어진 상황에서 빈칸을 만드는 함수입니다.
   // 시작점(1)과 끝점(N)은 남겨두기 때문에 remain = N-2 입니다.
   // select 배열에 1~N까지의 숫자를 넣어두고 랜덤하게 선택하여
   // 선택한 값을 지울수 있는 지를 확인합니다. 확인한 값은 배열에서 뒤로 옮겨줍니다.  
   // 지워야 할 index가 정해지면 해당 인덱스의 pos.x, pos.y = 0(빈칸을 생성)하고
   // find_solution 함수를 통해 유일해 여부를 판단합니다.
   // 유일해가 아닌 경우, puzzle_map의 값은 원래의 값으로 되돌립니다.
   // remain_pos(지울 수 있는 집합)에서 랜덤하게 원소 하나를 선택하고,
   // 이웃한 원소들과의 차이가 13미만이면 해당 원소를 삭제합니다.
   // erase는 현재 빈칸의 개수를 의미하며, 최대 2*N/3개 만큼 빈칸을 만들도록 설계하였습니다.  
   // 빈칸의 개수가 N/2개 보다 작은 경우, 처음부터 다시 문제를 생성하게 됩니다.
   
   no_remain = max(no_remain, 2); // no_remain은 남겨둘 숫자의 개수를,
   int erase = 0;                 // erase는 지운 숫자의 개수를,
   int remain = N - 2;            // remain은 확인하지 않은 숫자의 개수를 의미합니다.

   vector<int> select(N + 1); // select배열에 1~N까지의 숫자를 넣어둡니다.
   for (int i = 1; i <= N; i++)
      select[i] = i;

   while (remain + 2 > no_remain) // N - no_remain번 반복 수행합니다.
   {
      int erase_pos = rand() % remain + 2; // 지울 숫자의 인덱스를 랜덤하게 선택합니다.

      auto upper_it = remain_pos.upper_bound(select[erase_pos]);
      int upper_number, lower_number;   // 남은 숫자중에서 지울 숫자에 이웃한 두 값에 대해
      upper_number = *upper_it;         // 큰 값을 upper_number에, 작은 값을 lower_number에
      lower_number = *(--(--upper_it)); // 저장합니다.

      swap(select[erase_pos], select[remain + 1]); // 확인한 숫자는 배열에서 뒤로 옮겨,
      remain--;                                    // 재확인을 방지합니다.

      // upper_number와 lower_number의 차이가 LIMIT_DISTANCE 보다 크면 지울 수 없습니다.
      if (upper_number - lower_number > LIMIT_DISTANCE) continue;

      erase_pos = select[remain + 2]; // 지울 값을 erase_pos에 저장합니다.
      puzzle_map[pos[erase_pos].x][pos[erase_pos].y] = 0; // 지울 위치를 빈칸으로 하고  
      remain_pos.erase(erase_pos);                        // 남은 숫자 집합에서 제거합니다.

      // 현재 퍼즐맵으로 부터 유일해가 아닌 경우, 지웠던 셀과 남은 숫자 집합을 복구합니다.
      if (find_solution(pos[START_POS].x, pos[START_POS].y, START_POS) != UNIQUE_SOLUTION)
      {
         puzzle_map[pos[erase_pos].x][pos[erase_pos].y] = erase_pos;
         remain_pos.insert(erase_pos);
      }

      else erase++; // 유일해인 경우, 지운 숫자의 개수를 증가시킵니다.
   }

   return erase * 2 > N; // 절반 이상을 지웠는지를 반환합니다.
}

void make_solution_file()
{
   // 정답파일을 텍스트 파일로 저장하는 함수입니다.

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
{
   // 퍼즐 맵을 텍스트 파일로 저장하는 함수입니다.
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
   // 퍼즐 맵을 화면에 출력하는 함수입니다
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
      init(N); // 입력받은 N에 대하여 초기화를 실시합니다.
      create_solution(N / 2, N / 2, 1);
      make_solution_file();
      try // find_solution에서 해를 찾는데 오래 걸려는 경우, 처음부터 다시 만듭니다.
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
