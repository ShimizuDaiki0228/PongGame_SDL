#pragma once
#include "SDL.h"

struct Vector2
{
	float x;
	float y;

	Vector2(float a = 0.0f, float b = 0.0f) : x(a), y(b) {}
};


class Game
{
public:
	Game();
	bool Initialize();
	void RunLoop();
	void ShutDown();

private:
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();

	void MovePaddle(float deltaTime);
	void MoveBall(float deltaTime);

	/// <summary>
	/// �E�C���h�E����
	/// </summary>
	SDL_Window* mWindow;
	bool mIsRunning;
	const int WINDOW_WIDTH = 1024;
	const int WINDOW_HEIGHT = 768;

	/// <summary>
	/// �\����
	/// </summary>
	SDL_Renderer* mRenderer;
	const int THICKNESS = 15;


	/// <summary>
	/// �t���[���Ǘ�
	/// </summary>
	Uint32 mTicksCount;

	/// <summary>
	/// �p�h��
	/// </summary>
	int mPaddleDir = 0;
	const int PADDLE_H = 40;
	Vector2 mPaddlePos;

	/// <summary>
	/// �{�[��
	/// </summary>
	Vector2 mBallPos;
	Vector2 mBallVel;
};


float CalculateDeltaTime(Uint32* mTicksCount);
