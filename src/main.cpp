#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>
#include "button.hpp"
#include <vector>
using namespace std;
Color green = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};
int cellsize = 30;
int cellcount = 25;
double lastUpdateTime = 0;
int offset = 75;
int temp_score;
int high_score = 0;
bool ElementInDeque(Vector2 element, deque<Vector2> deque)
{
	for (unsigned int i = 0; i < deque.size(); i++)
	{
		if (Vector2Equals(deque[i], element))
		{
			return true;
		}
	}
	return false;
}
bool EventTriggered(double interval)
{
	double currentTime = GetTime();
	if (currentTime - lastUpdateTime >= interval)
	{
		lastUpdateTime = currentTime;
		return true;
	}
	return false;
}
class Snake
{
public:
	bool addSegment = false;
	deque<Vector2> body = {{6, 9}, {5, 9}, {4, 9}};
	Vector2 direction = {1, 0};
	void Draw()
	{
		for (unsigned int i = 0; i < body.size(); i++)
		{
			float x = body[i].x;
			float y = body[i].y;
			Rectangle segment = Rectangle{offset + x * cellsize, offset + y * cellsize, (float)cellsize, (float)cellsize};
			DrawRectangleRounded(segment, 0.5, 6, darkGreen);
		}
	}
	void Update()
	{
		body.push_front(Vector2Add(body[0], direction));
		if (addSegment)
		{
			addSegment = false;
		}
		else
		{
			body.pop_back();
		}
	}
	void Reset()
	{
		body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
		direction = {1, 0};
	}
};
class Food
{
public:
	static Texture2D textures[4];
	static bool loaded;
	Vector2 position;
	int textureIndex;
	Food(deque<Vector2> snakeBody)
	{
		if (!loaded)
		{
			Image img1 = LoadImage("graphics/food1.png");
			Image img2 = LoadImage("graphics/food2.png");
			Image img3 = LoadImage("graphics/food3.png");
			Image img4 = LoadImage("graphics/food4.png");
			textures[0] = LoadTextureFromImage(img1);
			textures[1] = LoadTextureFromImage(img2);
			textures[2] = LoadTextureFromImage(img3);
			textures[3] = LoadTextureFromImage(img4);
			UnloadImage(img1);
			UnloadImage(img2);
			UnloadImage(img3);
			UnloadImage(img4);
			loaded = true;
		}
		textureIndex = GetRandomValue(0, 3);
		position = GenerateRandomPos(snakeBody);
	}
	Vector2 GenerateRandomCell()
	{
		float x = GetRandomValue(0, cellcount - 1);
		float y = GetRandomValue(0, cellcount - 1);
		return Vector2{x, y};
	}
	Vector2 GenerateRandomPos(deque<Vector2> snakeBody)
	{
		Vector2 newPos = GenerateRandomCell();
		while (ElementInDeque(newPos, snakeBody))
		{
			newPos = GenerateRandomCell();
		}
		return newPos;
	}
	void Draw()
	{
		DrawTexture(
			textures[textureIndex],
			offset + position.x * cellsize,
			offset + position.y * cellsize,
			WHITE);
	}
};
Texture2D Food::textures[4] = {0};
bool Food::loaded = false;
class Game
{
public:
	int score = 0;
	double speed = 0.2;
	bool running = false;
	bool game_over = false;
	Snake snake = Snake();
	vector<Food> fruits;
	Sound wall;
	Sound eat;
	Game()
	{
		InitAudioDevice();
		wall = LoadSound("sounds/wall.mp3");
		eat = LoadSound("sounds/eat.mp3");
		int fruitCount = 3;
		for (int i = 0; i < fruitCount; i++)
		{
			fruits.push_back(Food(snake.body));
		}
	}
	~Game()
	{
		for (int i = 0; i < 4; i++)
			UnloadTexture(Food::textures[i]);
		UnloadSound(eat);
		UnloadSound(wall);
		CloseAudioDevice();
	}
	void Draw()
	{
		snake.Draw();
		for (auto &f : fruits)
			f.Draw();
	}
	void CheckCollisionWithFood()
	{
		for (auto &f : fruits)
		{
			if (Vector2Equals(snake.body[0], f.position))
			{
				f.position = f.GenerateRandomPos(snake.body);
				f.textureIndex = GetRandomValue(0, 3);
				snake.addSegment = true;
				score++;
				if(speed>=0.07)
					speed *= 0.98;
				PlaySound(eat);
			}
		}
	}
	void CheckCollisionWithEdges()
	{
		if (snake.body[0].x == cellcount || snake.body[0].x == -1)
		{
			GameOver();
			PlaySound(wall);
		}
		if (snake.body[0].y == cellcount || snake.body[0].y == -1)
		{
			GameOver();
			PlaySound(wall);
		}
	}
	void CheckCollisionsWithTail()
	{
		deque<Vector2> headless = snake.body;
		headless.pop_front();
		if (ElementInDeque(snake.body[0], headless))
		{
			GameOver();
			PlaySound(wall);
		}
	}
	void Update()
	{
		if (running)
		{
			snake.Update();
			CheckCollisionWithFood();
			CheckCollisionWithEdges();
			CheckCollisionsWithTail();
		}
	}
	void GameOver()
	{
		game_over = true;
		snake.Reset();
		fruits.clear();
		for (int i = 0; i < 3; i++)
		{
			fruits.push_back(Food(snake.body));
		}
		speed = 0.2;
		running = false;
		if (score >= high_score)
		{
			high_score = score;
		}
		temp_score = score;
		score = 0;
		speed = 0.2;
	}
};
int main()
{
	InitWindow(2 * offset + cellsize * cellcount, 2 * offset + cellsize * cellcount, "Snake's world");
	SetTargetFPS(60);
	{
		Button startButton{"graphics/start_button.png", {350, 300}, 0.65};
		Button exitButton{"graphics/exit_button.png", {350, 450}, 0.65};
		Button restartButton{"graphics/restart.png", {350, 500}, 1.5};
		bool exit = false;
		Game game = Game();
		while (!WindowShouldClose() && exit == false)
		{
			BeginDrawing();
			ClearBackground(green);
			if (IsKeyPressed(KEY_ENTER) && game.running == false)
			{
				game.running = true;
				game.game_over = false;
			}
			if (game.game_over == true)
			{
				ClearBackground(green);
				DrawText("Game Over!", 220, 150, 90, darkGreen);
				Vector2 mousePosition = GetMousePosition();
				bool mousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
				DrawText(TextFormat("Score: %i", temp_score), 350, 300, 60, darkGreen);
				DrawText(TextFormat("High Score: %i", high_score), 280, 400, 60, darkGreen);
				restartButton.Draw();
				if (restartButton.isPressed(mousePosition, mousePressed))
				{
					game.game_over = false;
					game.running = true;
				}
			}
			else if ((!game.running) && (game.game_over == false))
			{
				DrawText("Snake's World", 180, 150, 80, darkGreen);
				Vector2 mousePosition = GetMousePosition();
				bool mousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
				startButton.Draw();
				exitButton.Draw();
				if (exitButton.isPressed(mousePosition, mousePressed))
				{
					exit = true;
				}
				else if (startButton.isPressed(mousePosition, mousePressed))
				{
					game.running = true;
				}
			}
			else
			{
				game.Draw();
				DrawText("Snake's World", offset - 5, 20, 40, darkGreen);
				DrawRectangleLinesEx(Rectangle{(float)offset - 5, (float)offset - 5, (float)cellsize * cellcount + 10, (float)cellsize * cellcount + 10}, 5, darkGreen);
				DrawText(TextFormat("Score: %i", game.score), offset - 10, offset + cellsize * cellcount + 10, 40, darkGreen);
				DrawText(TextFormat("High Score: %i", high_score), cellcount * cellsize - 185, offset + cellsize * cellcount + 10, 40, darkGreen);
				if (EventTriggered(game.speed))
				{
					game.Update();
				}
				if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && game.snake.direction.y != 1)
				{
					game.snake.direction = {0, -1};
					WaitTime(0.15);
					game.running = true;
				}
				if ((IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) && game.snake.direction.y != -1)
				{
					game.snake.direction = {0, 1};
					WaitTime(0.15);
					game.running = true;
				}
				if ((IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) && game.snake.direction.x != 1)
				{
					game.snake.direction = {-1, 0};
					WaitTime(0.15);
					game.running = true;
				}
				if ((IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) && game.snake.direction.x != -1)
				{
					game.snake.direction = {1, 0};
					WaitTime(0.15);
					game.running = true;
				}
			}
			EndDrawing();
		}
	}
	CloseWindow();
}