/* Copyright (C) 2024, Valkai-Németh Béla-Örs */

#include <stdio.h>
#include <stdlib.h>

typedef struct Neighbours
{
	char xDiff;
	char yDiff;
} Neighbours;

Neighbours neighbours[8] = {
	{ -1, 0 }, // up
	{ 0, 1 }, // right
	{ 1, 0 }, // down
	{ 0, -1 }, // left
	{ -1, -1 }, // up and left
	{ 1, 1 }, // down and right
	{ -1, 1 }, // up and right
	{ 1, -1 } // down and left
};

typedef struct Point2D {
	int x;
	int y;
} Point2D;

void Fill(const Point2D* pPoint, unsigned char* pData, const int N, const int M)
{
	int curId = (pPoint->y * N) + pPoint->x;

	if (pPoint->y < 0 || pPoint->x < 0)
	{
		return;
	}

	if (pPoint->y >= M || pPoint->x >= N)
	{
		return;
	}

	if (!pData[curId])
	{
		pData[curId] = 1; //fill

		for (int i = 0; i < 4; i++)
		{
			Point2D neigbour;

			neigbour.x = pPoint->x + neighbours[i].xDiff;
			neigbour.y = pPoint->y + neighbours[i].yDiff;

			Fill(&neigbour, pData, N, M);
		}
	}
}

void FillObj(const Point2D* pPoint, unsigned char* pData, const int nrObj, const int N, const int M)
{
	int curId = (pPoint->y * N) + pPoint->x;

	if (pPoint->y < 0 || pPoint->x < 0)
	{
		return;
	}

	if (pPoint->y >= M || pPoint->x >= N)
	{
		return;
	}

	if (1 == pData[curId])
	{
		pData[curId] = nrObj;

		for (int i = 0; i < 8; i++)
		{
			Point2D neigbour;

			neigbour.x = pPoint->x + neighbours[i].xDiff;
			neigbour.y = pPoint->y + neighbours[i].yDiff;

			FillObj(&neigbour, pData, nrObj, N, M);
		}
	}
}

void Search(const Point2D* pPoint, unsigned char* pData, FILE* pOut, const int N, const int M)
{
	int curId = (pPoint->y * N) + pPoint->x;

	if (pPoint->y < 0 || pPoint->x < 0)
	{
		return;
	}

	if (pPoint->y >= M || pPoint->x >= N)
	{
		return;
	}

	if (0 != pData[curId])
	{
		// Already visited or wall
		return;
	}

	// Mark as already visited
	pData[curId] = 1;

	if (pPoint->x == 0 || pPoint->x == N - 1 || pPoint->y == 0 || pPoint->y == M - 1)
	{
		fprintf(pOut, "(%d,%d)", pPoint->x, pPoint->y);
	}
	else
	{
		for (int i = 0; i < 8; i++)
		{
			Point2D neigbour;

			neigbour.x = pPoint->x + neighbours[i].xDiff;
			neigbour.y = pPoint->y + neighbours[i].yDiff;

			Search(&neigbour, pData, pOut, N, M);
		}
	}
}

int SearchForPath(const Point2D* pPoint, const Point2D* pEndPoint, unsigned char* pData, FILE* pOut, const int N, const int M, const int curFillVal)
{
	int curId = (pPoint->y * N) + pPoint->x;
	int isWall;

	if (pPoint->y < 0 || pPoint->x < 0)
	{
		return 0;
	}

	if (pPoint->y >= M || pPoint->x >= N)
	{
		return 0;
	}

	// Check after the bounds were checked!
	isWall = pData[curId] & 0x01;

	if (curFillVal == (pData[curId] >> 1))
	{
		// Already visited on this run
		return 0;
	}

	// Mark as already visited
	pData[curId] = (unsigned char)curFillVal << 1;

	// This was a wall, we marked it
	// as visited and deleted it by
	// doing so, but we need to exit
	if (isWall)
		return 0;

	if (pPoint->x == pEndPoint->x && pPoint->y == pEndPoint->y)
	{
		pData[curId] = 255;
		return 1;
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			Point2D neigbour;

			neigbour.x = pPoint->x + neighbours[i].xDiff;
			neigbour.y = pPoint->y + neighbours[i].yDiff;

			if (SearchForPath(&neigbour, pEndPoint, pData, pOut, N, M, curFillVal))
				return 1;
		}
	}

	return 0;
}

void PrintAndReset(FILE* pOut, unsigned char* pData, const int N, const int M)
{
	size_t nrEntries = N * M;

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < M; j++)
		{
			int curId = (j * N) + i;

			fprintf(pOut, "%d ", pData[curId]);
			pData[curId] = pData[nrEntries + curId];
		}

		fprintf(pOut, "\n");
	}
}

int main(int argc, char* argv[])
{
	unsigned char* pData;
	Point2D mazeStart, mazeEnd, fillStart;
	errno_t result;
	size_t mazeReadOffset;
	FILE* pIn = NULL, * pOut = NULL;
	char readChar;
	int N, M;

	// Open input file
	result = fopen_s(&pIn, "in.txt", "r");
	if (!pIn)
	{
		printf("Error reading in.txt, file not found, aborting!\n");
		return 0;
	}

	// Open output file
	result = fopen_s(&pOut, "out.txt", "w");
	if (!pOut)
	{
		fclose(pIn);
		printf("Could not create out.txt, aborting!\n");
		return 0;
	}

	if (fscanf_s(pIn, "Fill start: %d %d", &fillStart.x, &fillStart.y) > 0)
	{
		fscanf_s(pIn, "\n");
	}

	if (fscanf_s(pIn, "Maze start: %d %d", &mazeStart.x, &mazeStart.y) > 0)
	{
		fscanf_s(pIn, "\n");
	}

	if (fscanf_s(pIn, "Maze end: %d %d", &mazeEnd.x, &mazeEnd.y) > 0)
	{
		fscanf_s(pIn, "\n");
	}

	mazeReadOffset = ftell(pIn);

	N = 1;
	M = 0;
	while (fscanf_s(pIn, "%c", &readChar, 1) > 0)
	{
		if ('\n' == readChar)
		{
			N++;
			continue;
		}

		if ('0' == readChar || '1' == readChar)
		{
			if (1 == N)
			{
				M++;
			}

			continue;
		}

		if (' ' == readChar)
		{
			continue;
		}

		fclose(pOut);
		fclose(pIn);
		printf("Error reading in.txt, file has wrong format, aborting!\n");

		return 0;
	}

	pData = (unsigned char*)malloc(N * M * 2);
	if (NULL != pData)
	{
		size_t nrEntries = N * M;
		int nrObj = 2;

		fseek(pIn, mazeReadOffset, SEEK_SET);

		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < M; j++)
			{

				int curId = (j * N) + i;
				int readVal;

				if (fscanf_s(pIn, "%d", &readVal) > 0)
				{
					pData[nrEntries + curId] = (unsigned char)readVal;
					pData[curId] = (unsigned char)readVal;
				}
			}
		}

		// Start fill
		Fill(&fillStart, pData, N, M);

		// Print fill result and reset input to original
		fprintf(pOut, "After fill starting from %d %d:\n", fillStart.x, fillStart.y);

		PrintAndReset(pOut, pData, N, M);

		// Start object recognition
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < M; j++)
			{
				int curId = (j * N) + i;

				if (1 == pData[curId])
				{
					Point2D coord;
					coord.x = i;
					coord.y = j;

					FillObj(&coord, pData, nrObj, N, M);
					nrObj++;
				}
			}
		}

		// Print object recognition result and reset input to original
		fprintf(pOut, "\nAfter object recognition:\n");

		PrintAndReset(pOut, pData, N, M);

		// Print exit points from maze
		fprintf(pOut, "\nPossible exit points from maze starting at %d %d:\n", mazeStart.x, mazeStart.y);

		Search(&mazeStart, pData, pOut, N, M);

		// Reset
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < M; j++)
			{
				int curId = (j * N) + i;

				pData[curId] = pData[nrEntries + curId];
			}
		}

		fprintf(pOut, "\n");

		int curFillVal = 1;
		while (1)
		{
			int result = SearchForPath(&mazeStart, &mazeEnd, pData, pOut, N, M, curFillVal);

			fprintf(pOut, "\nAfter search with fill value %d:\n", curFillVal);

			// Print only
			for (int i = 0; i < N; i++)
			{
				for (int j = 0; j < M; j++)
				{
					int curId = (j * N) + i;

					fprintf(pOut, "%d ", pData[curId]);
				}

				fprintf(pOut, "\n");
			}

			if (result)
				break;

			curFillVal++;
		}

		free(pData);
	}

	fclose(pOut);
	fclose(pIn);

	return 0;
}
