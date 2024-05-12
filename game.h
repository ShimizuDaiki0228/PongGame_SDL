#pragma once
#include "SDL.h"

struct Vector2
{
	float x;
	float y;

	Vector2(float a = 0.0f, float b = 0.0f) : x(a), y(b) {}
};

struct Paddle
{
	int direction;
	Vector2 position;
};

struct Ball
{
	Vector2 ballPos;
	Vector2 ballVel;
};


class Game
{
public:
	Game();
	bool Initialize();
	void RunLoop();
	void ShutDown();

private:
	void Reset();

	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	void MovePaddle(float deltaTime, Vector2* mPaddlePos, int mPaddleDir);
	void MoveBall(float deltaTime, Ball* ball);

	float CheckPaddlePosition(Vector2 paddlePos, Ball ball);


	/// <summary>
	/// ウインドウ周り
	/// </summary>
	SDL_Window* mWindow;
	bool mIsRunning;
	const int WINDOW_WIDTH = 1024;
	const int WINDOW_HEIGHT = 768;

	/// <summary>
	/// 表示物
	/// </summary>
	SDL_Renderer* mRenderer;
	const int THICKNESS = 15;


	/// <summary>
	/// フレーム管理
	/// </summary>
	Uint32 mTicksCount;

	/// <summary>
	/// パドル
	/// </summary>
	const int PADDLE_H = 40;
	//int mPaddleDir_1 = 0;
	//Vector2 mPaddlePos_1;
	//int mPaddleDir_2 = 0;
	//Vector2 mPaddlePos_2;
	Paddle paddle_1;
	Paddle paddle_2;

	/// <summary>
	/// ボール
	/// </summary>
	Ball ball_1;
	Ball ball_2;
};


float CalculateDeltaTime(Uint32* mTicksCount);
int ChangePaddleDir(const Uint8* state, SDL_Scancode UpKey, SDL_Scancode DownKey);
void GenerateRect(SDL_Renderer* mRenderer, int x, int y, int width, int height);