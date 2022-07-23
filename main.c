#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"

typedef enum {
	Blank,
	Wall,
	Start,
	Goal
} CellType;

typedef struct {
	int x, y;
} Point;

typedef Point Size;

typedef struct Cell_s {
	CellType t;
	struct Cell_s *earliest;
	Point p;
} Cell;

typedef struct {
	Size s;
	Cell *cells;
} Board;

Cell* getCell(Board *b, int x, int y) {
	return &b->cells[b->s.x * y + x];
}

Cell* getCellP(Board *b, Point p) {
	return getCell(b, p.x, p.y);
}

int isInside(Board *b, Point p) {
	return (p.x < b->s.x && 0 <= p.x && p.y < b->s.y && 0 <= p.y);
}

int isWall(Board *b, Point p) {
	return !isInside(b, p) || getCellP(b, p)->t == Wall;
}

typedef enum {
	NewCell,
	WellknownCell,
	ReachToGoal
} WalkResultType;

typedef enum {
	Upward,
	Downward,
	Leftward,
	Rightward
} Direction;

typedef struct {
	WalkResultType t;
	Point p;
} WalkResult;

WalkResult walk(Board *b, Point p, Direction d) {
	int x, y;

	switch (d) {
		case Upward:	y = -1, x =  0; break;
		case Downward:	y =  1, x =  0; break;
		case Leftward:  y =  0, x = -1; break;
		case Rightward: y =  0, x =  1; break;
	}

	while (1) {
		p.x += x, p.y += y;

		if (isWall(b, p)) {
			p.x -= x, p.y -= y;

			if (getCellP(b, p)->t == Start || getCellP(b, p)->earliest != NULL)
				return (WalkResult) { WellknownCell, p };
			else
				return (WalkResult) { NewCell, p };
		}

		if (isInside(b, p) && getCellP(b, p)->t == Goal)
			return (WalkResult) { ReachToGoal, p };
	}
}

typedef enum {
	StartFound = 1 << 0,
	GoalFound = 1 << 1
} ParseState;

Point parseBoard(Board *b) {
	ParseState s = 0;
	Point start;
	int x, y;

	if (fscanf(stdin, "%d %d\n", &b->s.x, &b->s.y) != 2) {
		fprintf(stderr, "Failed to read board size\n");
		exit(EXIT_FAILURE);
	}

	b->cells = (Cell*) calloc(b->s.x * b->s.y, sizeof(Cell));


	for (y = 0; y < b->s.y; ++y)
		for (x = 0; x < b->s.x; ++x) {
			getCell(b, x, y)->p = (Point) { x, y };
			getCell(b, x, y)->earliest = NULL;

INVALID:
			switch (getchar()) {
				case '#':
					getCell(b, x, y)->t = Wall;
					break;

				case '.':
					getCell(b, x, y)->t = Blank;
					break;

				case 'G':
					if (s & GoalFound) {
						fprintf(stderr, "Duplicate 'G'\n");
						exit(EXIT_FAILURE);
					}
					s |= GoalFound;
					getCell(b, x, y)->t = Goal;
					break;

				case 'S':
					if (s & StartFound) {
						fprintf(stderr, "Duplicate 'S'\n");
						exit(EXIT_FAILURE);
					}
	
					s |= StartFound;
					getCell(b, x, y)->t = Start;
					start = (Point) { x, y };
					break;

				case EOF:
					fprintf(stderr, "Unexpected EOF\n");
					exit(EXIT_FAILURE);

				default:
					goto INVALID;
			}
		}

	if (~s & GoalFound) {
		fprintf(stderr, "Goal is not found.\n");
		exit(EXIT_FAILURE);
	}

	if (~s & StartFound) {
		fprintf(stderr, "Strat is not found.\n");
		exit(EXIT_FAILURE);
	}

	return start;
}

char* cellTypeToS(CellType t) {
	switch (t) {
		case Blank: return "Cell";
		case Wall: return "Wall";
		case Start: return "Start";
		case Goal: return "Goal";
	}
}

void printBacktrace(Board *b, Point p) {
	Cell *c;

	c = getCellP(b, p);
	puts("|   Backtrace   |");
	puts("|~~~~~~~~~~~~~~~|");
	puts("|X |Y |Type     |");
	puts("|--|--|---------|");

	while (1) {
		printf("|%2d|%2d|%s\t|\n", c->p.x+1, c->p.y+1, cellTypeToS(c->t));
		if (c->t == Start) return;

		assert(c != c->earliest);
		c = c->earliest;
	}
}

void swap(int *a, int *b) {
	*a = *a ^ *b;
	*b = *b ^ *a;
	*a = *a ^ *b;
}

char* cellTypeToShortS(CellType t) {
	switch (t) {
		case Blank: return " ";
		case Wall: return "#";
		case Start: return "S";
		case Goal: return "G";
	}
}

typedef enum {
	TopHand		= 1 << 0,
	RightHand	= 1 << 1,
	BottomHand	= 1 << 2,
	LeftHand	= 1 << 3,
} BoxOverrideState;

/* http://k-maeda.la.coocan.jp/code/uni/uni068.html */
char *BoxOverrideChars[] = {
	" ", /* L B R T */
	"╵", /* 0 0 0 1 */
	"╶", /* 0 0 1 0 */
	"└", /* 0 0 1 1 */
	"╷", /* 0 1 0 0 */
	"│", /* 0 1 0 1 */
	"┌", /* 0 1 1 0 */
	"├", /* 0 1 1 1 */
	"╴", /* 1 0 0 0 */
	"┘", /* 1 0 0 1 */
	"─", /* 1 0 1 0 */
	"┴", /* 1 0 1 1 */
	"┐", /* 1 1 0 0 */
	"┤", /* 1 1 0 1 */
	"┬", /* 1 1 1 0 */
	"┼", /* 1 1 1 1 */
};

void printResult(Board *b, Point p) {
	int x, y, start, end;
	BoxOverrideState *o;
	Cell *from, *to;

	o = calloc(sizeof(BoxOverrideState), b->s.x * b->s.y);

#define GETPOS(ARRAY, X, Y) ( ARRAY[b->s.x * Y + X] )

	for (
		from = getCellP(b, p), to = from->earliest;
		from->t != Start;
		from = to, to = from->earliest
	) {
		if (from->p.x == to->p.x) {
			/* vertical line */
			start = from->p.y;
			end = to->p.y;
			if (end < start) swap(&start, &end);
			GETPOS(o, from->p.x, start) |= BottomHand;
			for (++start; start < end; ++start)
				GETPOS(o, from->p.x, start) |= BottomHand | TopHand;
			GETPOS(o, from->p.x, end) |= TopHand;
		} else {
			/* horizontal line */
			start = from->p.x;
			end = to->p.x;
			if (end < start) swap(&start, &end);
			GETPOS(o, start, from->p.y) |= RightHand;
			for (++start; start < end; ++start)
				GETPOS(o, start, from->p.y) |= RightHand | LeftHand;
			GETPOS(o, end, from->p.y) |= LeftHand;
		}
	}


	printf("╔");
	for (x = 0; x < b->s.x; ++x)
		printf("═");
	puts("╗");

	for (y = 0; y < b->s.y; ++y) {
		printf("║");
		for (x = 0; x < b->s.x; ++x) {
			switch (getCell(b, x, y)->t) {
				case Blank:
					printf("%s", BoxOverrideChars[GETPOS(o, x, y)]);
					break;

				default:
					printf("%s", cellTypeToShortS(getCell(b, x, y)->t));
					break;
			}
		}
		puts("║");
	}

	printf("╚");
	for (x = 0; x < b->s.x; ++x)
		printf("═");
	puts("╝");
}

int main() {
	Board b;
	Point cursor;
	WalkResult r;
	size_t i;
	List cursors = { sizeof(Point), 0, 0, NULL };
	List cursorsNext = { sizeof(Point), 0, 0, NULL };

	cursor = parseBoard(&b);
	APPEND(Point, cursors, cursor)

	while (1) {
		for (i = 0; i < cursors.list_length; ++i) {
			cursor = ((Point*)cursors.data)[i];

#define WALK(DIRECTION) \
			r = walk(&b, cursor, DIRECTION); \
			switch (r.t) { \
				case WellknownCell: \
					break; \
				case NewCell: \
					getCellP(&b, r.p)->earliest = getCellP(&b, cursor); \
					APPEND(Point, cursorsNext, r.p) \
					break; \
				case ReachToGoal: \
					getCellP(&b, r.p)->earliest = getCellP(&b, cursor); \
					cursor = r.p; \
					goto FOUND; \
			}

			WALK(Upward)
			WALK(Downward)
			WALK(Leftward)
			WALK(Rightward)
		}

		if (!cursorsNext.list_length) {
			fprintf(stderr, "Route is not found.\n");
			exit(EXIT_FAILURE);
		}

		moveout(&cursorsNext, &cursors);
	}

FOUND:
	moveout(&cursorsNext, &cursors);
	printResult(&b, cursor);
	printBacktrace(&b, cursor);
}
