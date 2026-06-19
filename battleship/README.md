# Battleship Console Game

10x10 바다 맵에서 플레이어와 컴퓨터가 서로의 배를 공격하는 콘솔 게임입니다.


## Game Rules

- 좌표는 `row col` 형식으로 입력합니다.
- row와 col은 모두 `0`부터 `9`까지 사용할 수 있습니다.
- 플레이어는 게임 시작 전에 직접 배를 배치합니다.
- 컴퓨터의 배는 게임 시작 시 랜덤으로 배치됩니다.
- 플레이어가 먼저 공격하고, 이후 컴퓨터가 랜덤으로 공격합니다.
- 이미 공격한 좌표는 다시 공격할 수 없습니다.
- 상대 배를 맞히면 `Hit`, 비어 있는 칸이면 `Miss`로 처리됩니다.

## Ships

| Ship | Size | Placement Input |
| --- | --- | --- |
| Scout | 1 cell | `row col` |
| Destroyer | 2 cells | `row col direction` |
| Battleship | 4 cells | `row col direction` |

방향은 다음과 같이 입력합니다.

- `H`: 오른쪽으로 가로 배치
- `V`: 아래쪽으로 세로 배치

## Board Symbols

| Symbol | Meaning |
| --- | --- |
| `.` | Water or unknown cell |
| `S` | Your ship |
| `X` | Hit |
| `O` | Miss |

## Score And Ranking

게임이 끝나면 이름을 입력하고 점수를 저장합니다.

```text
score = 1000 - turn_count * 10 + hit_count * 50
```

- 랭킹 파일은 실행 위치에 `rankings.txt`로 생성됩니다.
- 상위 10명의 이름과 점수만 저장하고 출력합니다.
- 랭킹 저장 형식은 `name score`입니다.

## Files

- `battleship.c`: 게임 소스 코드
- `rankings.txt`: 게임 실행 후 생성되는 랭킹 파일
