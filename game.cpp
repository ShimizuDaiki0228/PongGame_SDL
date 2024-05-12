#include "game.h"
#include <iostream>
using namespace std;


Game::Game()
	: mWindow(nullptr), mIsRunning(true), mPaddlePos_1(30, 384),mPaddlePos_2(WINDOW_WIDTH - THICKNESS * 2, 384), mTicksCount(0)
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

	ball_1.ballPos.x = 0, ball_1.ballPos.y = 0;
	ball_1.ballVel.x = 1, ball_1.ballVel.y = 1;
	ball_2.ballPos.x = 0, ball_1.ballPos.y = 50;
	ball_2.ballVel.x = -1, ball_1.ballVel.y = -1;
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

	mPaddleDir_1 = 0;
	mPaddleDir_2 = 0;
	mPaddleDir_1 = ChangePaddleDir(state, SDL_SCANCODE_W, SDL_SCANCODE_S);
	mPaddleDir_2 = ChangePaddleDir(state, SDL_SCANCODE_I, SDL_SCANCODE_K);
	
}

int ChangePaddleDir(const Uint8* state, SDL_Scancode UpKey, SDL_Scancode DownKey)
{
	int dir = 0;
	if (state[UpKey])
	{
		dir -= 1;
	}
	if (state[DownKey])
	{
		dir += 1;
	}
	return dir;
}

void Game::UpdateGame()
{
	//前のフレームから16msが経過するまで待つ
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));

	float deltaTime = CalculateDeltaTime(&mTicksCount);

	MovePaddle(deltaTime, &mPaddlePos_1, mPaddleDir_1);
	MovePaddle(deltaTime, &mPaddlePos_2, mPaddleDir_2);
	MoveBall(deltaTime, &ball_1);
	MoveBall(deltaTime, &ball_2);
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

void Game::MovePaddle(float deltaTime, Vector2* mPaddlePos, int mPaddleDir)
{
	mPaddlePos->y += mPaddleDir * 300.0f * deltaTime;
	if (mPaddlePos->y < (PADDLE_H / 2))
	{
		mPaddlePos->y = PADDLE_H / 2;
	}
	else if (mPaddlePos->y > (WINDOW_HEIGHT - PADDLE_H / 2 - THICKNESS))
	{
		mPaddlePos->y = WINDOW_HEIGHT - PADDLE_H / 2 - THICKNESS;
	}
}

void Game::MoveBall(float deltaTime, Ball* ball)
{
	if ((ball->ballPos.y < THICKNESS && ball->ballVel.y < 0) ||
		 ball->ballPos.y > WINDOW_HEIGHT - THICKNESS / 2 && ball->ballVel.y > 0) ball->ballVel.y *= -1;

	if ((ball->ballPos.x < THICKNESS && ball->ballVel.x < 0) ||
		ball->ballPos.x > WINDOW_WIDTH - THICKNESS / 2 && ball->ballVel.x > 0) ball->ballVel.x *= -1;

	
	ball->ballVel.x *= CheckPaddlePosition(mPaddlePos_1, *ball);
	ball->ballVel.x *= CheckPaddlePosition(mPaddlePos_2, *ball);
	

	ball->ballPos.x += ball->ballVel.x * 300 * deltaTime;
	ball->ballPos.y += ball->ballVel.y * 300 * deltaTime;
}

float Game::CheckPaddlePosition(Vector2 paddlePos, Ball ball)
{
	int diff = abs(paddlePos.y - ball.ballPos.y);
	//if (ball.ballVel.x < 0)
	//{
		if (diff <= PADDLE_H / 2 &&
			ball.ballPos.x <= paddlePos.x + THICKNESS && ball.ballPos.x >= paddlePos.x - THICKNESS)
		{
			return -1;
		}
	//}
	//else
	//{
	//	///要修正
	//	if (diff <= PADDLE_H / 2 &&
	//		ball.ballPos.x <= ball.ballPos.x + THICKNESS && ball.ballPos.x >= ball.ballPos.x - THICKNESS)
	//	{
	//		return -1;
	//	}
	//}
	

	return 1;
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

	///壁の作成
	GenerateRect(
		mRenderer,
		0,
		0,
		WINDOW_WIDTH,
		THICKNESS
	);

	//1つめのボールの作成
	GenerateRect(
		mRenderer,
		static_cast<int>(ball_1.ballPos.x - THICKNESS / 2),
		static_cast<int>(ball_1.ballPos.y - THICKNESS / 2),
		THICKNESS,
		THICKNESS);

	//2つめのボールの作成
	GenerateRect(
		mRenderer,
		static_cast<int>(ball_2.ballPos.x - THICKNESS / 2),
		static_cast<int>(ball_2.ballPos.y - THICKNESS / 2),
		THICKNESS,
		THICKNESS
	);

	//1つめのパドルの作成
	GenerateRect(
		mRenderer,
		static_cast<int>(mPaddlePos_1.x - THICKNESS / 2),
		static_cast<int>(mPaddlePos_1.y - THICKNESS / 2),
		THICKNESS,
		PADDLE_H);

	//2つめのパドルの作成
	GenerateRect(
		mRenderer,
		static_cast<int>(mPaddlePos_2.x - THICKNESS / 2),
		static_cast<int>(mPaddlePos_2.y - THICKNESS / 2),
		THICKNESS,
		PADDLE_H);

	SDL_RenderPresent(mRenderer);

}

/// <summary>
/// 長方形のオブジェクトを作成する
/// </summary>
/// <param name="mRenderer"></param>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="width"></param>
/// <param name="height"></param>
void GenerateRect(SDL_Renderer* mRenderer, 
				  int x,
			      int y,
				  int width,
	              int height)
{
	SDL_Rect object{
		x,
		y,
		width,
		height
	};
	SDL_RenderFillRect(mRenderer, &object);
}