#include <dxlib.h>
#include "GameObject.h"

//---------------------------------------------------
// 当たり判定を行う関数
//---------------------------------------------------
// 
// 緑のキャラ 左   x1
// 　　　　　 上   y1
// 　　　　　 横幅 w1
// 　　　　　 縦幅 h1
// 黄のキャラ 左   x2
// 　　　　　 上   y2
// 　　　　　 横幅 w2
// 　　　　　 縦幅 h2
//
int CheckHit(float x1, float y1, float w1, float h1,
	float x2, float y2, float w2, float h2)
{
	float L1 = x1;      // 左
	float R1 = x1 + w1; // 右(左+横幅)
	float L2 = x2;      // 左
	float R2 = x2 + w2; // 右(左+横幅)

	if (R1 <= L2) return 0; //< 線上も当たらないように
	if (R2 <= L1) return 0; //< 線上も当たらないように

	float U1 = y1;      // 上
	float D1 = y1 + h1; // 下(上+縦幅)
	float U2 = y2;      // 上
	float D2 = y2 + h2; // 下(上+縦幅)

	if (D1 <= U2) return 0; //< 線上も当たらないように
	if (D2 <= U1) return 0; //< 線上も当たらないように

	// それ以外の場合は当たっている
	return 1;
}

//----------------------------------------------------
// 画面サイズ
//----------------------------------------------------
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//----------------------------------------------------
// シーンに使うENUM
//----------------------------------------------------
enum class SCENE : unsigned int
{
	NONE = 0,
	TITLE,
	GAME,
	GAMEOVER,

	EXIT,
	NUM, //< ENUMの数を管理したい場合のために用意
};


//----------------------------------------------------
// タイトルのクラス
//----------------------------------------------------
class Title : public GameObject
{
public:
	Title()
	{
		// コンストラクタでは、
		// ロードなどはここで行うべきではない
	}

	void Init() override
	{
		// ロードはここで行う
		Load("space_rocket2.png");
		pos.x = 180.0f;
		pos.y = 480.0f;
	}

	void Update() override
	{
		DrawString(250, 240 - 32, "Meteorite Fall", GetColor(0, 0, 0));
		SetFontSize(28);
		__super::Update();

		move.y = (180 - pos.y) * 0.05f;

		if (CheckHitKey(KEY_INPUT_RETURN))
		{
			fade = true;
		}

		// タイトルフェード
		if (fade)
		{
			alpha -= 0.02f;
			if (alpha < 0.0f)
			{
				alpha = 0.0f;
				// タイトルを消去する
				TheEnd();
			}
		}
	}
protected:
	bool fade = false;

};

//----------------------------------------------------
// GameOverのクラス
//----------------------------------------------------
class GameOver : public GameObject
{
public:
	GameOver()
	{
		// コンストラクタでは、
		// ロードなどはここで行うべきではない
	}

	void Init() override
	{
		// ロードはここで行う
		Load("space_rocket22.png");
		pos.x = 180.0f;
		pos.y = 50.0f;
	}

	void Update() override
	{
		DrawString(250, 240 - 32, "GameOver", GetColor(255, 255, 255));

		__super::Update();
		move.y = (200 + pos.y)* 0.02f;
	}
};


//----------------------------------------------------
// プレイヤーのクラス
//----------------------------------------------------
class Player : public GameObject
{
public:
	Player()
	{
	}

	void Init()
	{
		// ロードはここで行う
		Load("space_rocket2.png");
		pos.x = 320;
		pos.y = 340;
	}

	void Update() override
	{
		if (CheckHitKey(KEY_INPUT_RIGHT))
			pos.x += 4;
		if (CheckHitKey(KEY_INPUT_LEFT))
			pos.x -= 4;

		if (pos.x < 0) pos.x = 0;
		if (pos.x > 640) pos.x = 640;
	}

};

//----------------------------------------------------
// エネミーのクラス
//----------------------------------------------------
class Enemy : public GameObject
{
public:
	Enemy()
	{
	}

	void Init() override
	{
		// ロードはここで行う
		Load("space2.png");
		pos.x = (float)GetRand(640 - 20);
		pos.y = -64.0f;

		move.y = 1 + (GetRand(20) * 0.1f);
	}

	void Update() override
	{
		__super::Update();

		if (pos.y > 480)
			TheEnd();
	}
};

//-----------------------------------------------------------------

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	//======================================================
	// ウインドウ設定とDXライブラリの初期化
	//======================================================

	// Windowsモードにする
	ChangeWindowMode(TRUE);

	// 画面サイズの設定
	SetGraphMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32);

	// DXライブラリ初期化
	if (DxLib_Init() == -1)
	{
		//初期化に失敗
		return -1;
	}
	// ダブルバッファモード
	SetDrawScreen(DX_SCREEN_BACK);

	// シーンをタイトルに設定
	SCENE scene = SCENE::TITLE;

	// タイトルオブジェクトを作成
	CreateGameObject<Title>();
	// 初期タイトル中背景色は白
	SetBackgroundColor(255, 255, 255);

	// 共通タイマー
	int timer = 0;

	while (ProcessMessage() == 0)
	{
		//裏画面消去
		ClearDrawScreen();

		clsDx();

		// ゲームオブジェクトの更新と描画
		ProcessGameObjects();

		switch (scene)
		{
			//---------------------------------------------
			// タイトル処理
			//---------------------------------------------
		case SCENE::TITLE:
		{
			// タイトルが存在しない場合
			if (GetGameObject<Title>() == nullptr)
			{
				// 前の全オブジェクトを消去
				DeleteGameObjects();

				// ゲーム初期化する
				CreateGameObject<Player>();

				// ゲーム中は背景は黒に
				SetBackgroundColor(0, 0, 0);

				// シーンを「ゲーム」にする
				scene = SCENE::GAME;
			}
		}
		break;

		//---------------------------------------------
		// ゲーム中処理
		//---------------------------------------------
		case SCENE::GAME:
		{
			int gh;
			gh = LoadGraph("utyu3.png");
			SetDrawScreen(DX_SCREEN_BACK);
			DrawGraph(0, 0, gh, TRUE);

			// ここでゲームに必要な更新と描画を行う
			ProcessGameObjects();

			// 0.5秒に一回 (60フレームは一秒) 
			if ((++timer % 30) == 0)
			{
				// 敵を一体作成する
				CreateGameObject<Enemy>();
			}

			auto player = GetGameObject<Player>();
			auto enemies = GetGameObjects<Enemy>();
			if (player)
			{
				for (auto enemy : enemies)
				{
					if (CheckHit(player->pos.x, player->pos.y,
						player->size.x, player->size.y,
						enemy->pos.x, enemy->pos.y, enemy->size.x, enemy->size.y))
					{
						// 前の全オブジェクトを消去
						DeleteGameObjects();

						// 当ったのでゲームオーバーの準備をする
						CreateGameObject<GameOver>();

						// 今は180フレーム(180/60=約3秒)でタイトルへ行くように
						int timer = 180;

						scene = SCENE::GAMEOVER;
					}
				}
			}
		}
		break;

		//---------------------------------------------
		// ゲームオーバー処理
		//---------------------------------------------
		case SCENE::GAMEOVER:
		{
			// 仮でタイマーでタイトルへ
			if (--timer <= 0)
			{
				// 前の全オブジェクトを消去
				DeleteGameObjects();

				// タイトル処理へ
				CreateGameObject<Title>();
				// 初期タイトル中背景色は白
				SetBackgroundColor(255, 255, 255);

				scene = SCENE::TITLE;
			}
		}
		break;
		}

		//裏画面を表画面を入れ替える
		ScreenFlip();
	}
	DeleteGameObjects();

	//======================================================
	// DXライブラリ終了とプログラム終了
	//======================================================

	DxLib_End();
	return 0;
}
