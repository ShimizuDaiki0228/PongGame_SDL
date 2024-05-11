#include "game.h"
#include <iostream>
using namespace std;


Game::Game()
	: mWindow(nullptr), mIsRunning(true), mPaddlePos(30, 384), mBallPos(512, 384), mTicksCount(0), mBallVel(1, 1)
{
	
}

bool Game::Initialize()
{
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);

	if (sdlResult != 0)
	{
		SDL_Log("SDLを初期化できません : %s", SDL_GetError());
		return false;
	}


	mWindow = SDL_CreateWindow(
		"Game Programming in C++",
		100,
		100,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		0
	);

	if (!mWindow)
	{
		SDL_Log("ウインドウの作成に失敗しました : %s", SDL_GetError());
		return false;
	}

	mRenderer = SDL_CreateRenderer(
		mWindow,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!mRenderer)
	{
		SDL_Log("レンダラーの作成に失敗しました : %s", SDL_GetError());
		return false;
	}
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ShutDown()
{
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			mIsRunning = false;
			break;
		}
	}

	//キーボードの状態を取得する
	const Uint8* state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	mPaddleDir = 0;
	if (state[SDL_SCANCODE_W])
	{
		mPaddleDir -= 1;
	}
	if (state[SDL_SCANCODE_S])
	{
		mPaddleDir += 1;
	}
}

void Game::UpdateGame()
{
	//前のフレームから16msが経過するまで待つ
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));

	float deltaTime = CalculateDeltaTime(&mTicksCount);

	MovePaddle(deltaTime);
	MoveBall(deltaTime);
}

float CalculateDeltaTime(Uint32* mTicksCount)
{
	float deltaTime = (SDL_GetTicks() - *mTicksCount) / 1000.0f;
	*mTicksCount = SDL_GetTicks();

	//デルタタイムを最大値で制限する
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	return deltaTime;
}

void Game::MovePaddle(float deltaTime)
{
	mPaddlePos.y += mPaddleDir * 300.0f * deltaTime;
	if (mPaddlePos.y < (PADDLE_H / 2))
	{
		mPaddlePos.y = PADDLE_H / 2;
	}
	else if (mPaddlePos.y > (WINDOW_HEIGHT - PADDLE_H / 2 - THICKNESS))
	{
		mPaddlePos.y = WINDOW_HEIGHT - PADDLE_H / 2 - THICKNESS;
	}
}

void Game::MoveBall(float deltaTime)
{
	if ((mBallPos.y < THICKNESS && mBallVel.y < 0) ||
		 mBallPos.y > WINDOW_HEIGHT - THICKNESS / 2 && mBallVel.y > 0) mBallVel.y *= -1;

	if ((mBallPos.x < THICKNESS && mBallVel.x < 0) ||
		mBallPos.x > WINDOW_WIDTH - THICKNESS / 2 && mBallVel.x > 0) mBallVel.x *= -1;

	int diff = abs(mPaddlePos.y - mBallPos.y);
	if (diff <= PADDLE_H / 2 &&
		mBallPos.x <= 45 && mBallPos.x >= 15 &&
		mBallVel.x < 0)
	{
		mBallVel.x *= -1;
	}

	mBallPos.x += mBallVel.x * 300 * deltaTime;
	mBallPos.y += mBallVel.y * 300 * deltaTime;
}

void Game::GenerateOutput()
{
	SDL_SetRenderDrawColor(
		mRenderer,
		0,
		0,
		255,
		255
	);

	SDL_RenderClear(mRenderer);

	SDL_SetRenderDrawColor(
		mRenderer,
		255,
		255,
		255,
		255
	);

	SDL_Rect wall{
		0,
		0,
		WINDOW_WIDTH,
		THICKNESS
	};

	SDL_RenderFillRect(mRenderer, &wall);

	SDL_Rect ball{
		static_cast<int>(mBallPos.x - THICKNESS / 2),
		static_cast<int>(mBallPos.y - THICKNESS / 2),
		THICKNESS,
		THICKNESS,
	};

	SDL_RenderFillRect(mRenderer, &ball);

	SDL_Rect paddle{
		static_cast<int>(mPaddlePos.x - THICKNESS / 2),
		static_cast<int>(mPaddlePos.y - THICKNESS / 2),
		THICKNESS,
		PADDLE_H
	};

	SDL_RenderFillRect(mRenderer, &paddle);

	SDL_RenderPresent(mRenderer);

}

