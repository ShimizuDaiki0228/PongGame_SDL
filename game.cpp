#include "game.h"
#include <iostream>
using namespace std;


Game::Game()
	: mWindow(nullptr), mIsRunning(true), mTicksCount(0)
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

	Reset();
}

void Game::Reset()
{
	ball_1.ballPos.x = 100, ball_1.ballPos.y = 100;
	ball_1.ballVel.x = 1, ball_1.ballVel.y = 1;
	ball_2.ballPos.x = 800, ball_2.ballPos.y = 300;
	ball_2.ballVel.x = -1, ball_2.ballVel.y = -1;

	paddle_1.position.x = 30, paddle_1.position.y = 384;
	paddle_2.position.x = WINDOW_WIDTH - THICKNESS * 2, paddle_2.position.y = 384;
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

	paddle_1.direction = 0;
	paddle_2.direction = 0;
	paddle_1.direction = ChangePaddleDir(state, SDL_SCANCODE_W, SDL_SCANCODE_S);
	paddle_2.direction = ChangePaddleDir(state, SDL_SCANCODE_I, SDL_SCANCODE_K);
	
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

	MovePaddle(deltaTime, &paddle_1.position, paddle_1.direction);
	MovePaddle(deltaTime, &paddle_2.position, paddle_2.direction);
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

	
	ball->ballVel.x *= CheckPaddlePosition(paddle_1.position, *ball);
	ball->ballVel.x *= CheckPaddlePosition(paddle_2.position, *ball);
	

	ball->ballPos.x += ball->ballVel.x * 300 * deltaTime;
	ball->ballPos.y += ball->ballVel.y * 300 * deltaTime;
}

float Game::CheckPaddlePosition(Vector2 paddlePos, Ball ball)
{
	int diff = abs(paddlePos.y - ball.ballPos.y);
	if (diff <= PADDLE_H / 2 &&
		ball.ballPos.x <= paddlePos.x + THICKNESS && ball.ballPos.x >= paddlePos.x - THICKNESS)
	{
		return -1;
	}

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
		THICKNESS);

	//1つめのパドルの作成
	GenerateRect(
		mRenderer,
		static_cast<int>(paddle_1.position.x - THICKNESS / 2),
		static_cast<int>(paddle_1.position.y - THICKNESS / 2),
		THICKNESS,
		PADDLE_H);

	//2つめのパドルの作成
	GenerateRect(
		mRenderer,
		static_cast<int>(paddle_2.position.x - THICKNESS / 2),
		static_cast<int>(paddle_2.position.y - THICKNESS / 2),
		THICKNESS,
		PADDLE_H);

	SDL_RenderPresent(mRenderer);

}

/// <summary>
/// 長方形のオブジェクトを作成する
/// </summary>
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