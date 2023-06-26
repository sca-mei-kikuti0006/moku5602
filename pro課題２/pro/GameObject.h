#pragma once
/**
 * @file GameObject
 */

#include <dxlib.h>
#include <vector>

 //----------------------------------------------------
 // オブジェクトのリスト
 //----------------------------------------------------
class GameObject;
std::vector<GameObject*> objects;

//----------------------------------------------------
// オブジェクト作成
//----------------------------------------------------
template<typename T>
T& CreateGameObject()
{
	objects.push_back(new T());
	auto* obj = (T*)objects[objects.size() - 1];

	obj->Init();
	return *obj;
}

//! ゲームオブジェクトの取得
template<typename T>
T* GetGameObject()
{
	for (auto* obj : objects)
	{
		T* ptr = dynamic_cast<T*>(obj);
		if (ptr)
		{
			return (T*)obj;
		}
	}
	return nullptr;
}

//----------------------------------------------------
// 現在存在しているオブジェクトを取得する
//----------------------------------------------------
template<typename T>
std::vector<T*> GetGameObjects()
{
	std::vector<T*> objs;
	for (auto* obj : objects)
	{
		T* ptr = dynamic_cast<T*>(obj);
		if (ptr)
		{
			objs.push_back(ptr);
		}
	}
	return objs;
}

//----------------------------------------------------
// 現在何個GameObjectが存在しているか確認用
//----------------------------------------------------
size_t CountGameObjects()
{
	return objects.size();
}

//----------------------------------------------------
// 基底のオブジェクトクラス
//----------------------------------------------------
class GameObject
{
public:
	GameObject()
	{
	}

	// virtual必要
	virtual ~GameObject()
	{
	}

	// img を隠蔽するために用意
	void Load(const TCHAR* name)
	{
		img = LoadGraph(name);
		// イメージのサイズを取得する
		int size_x, size_y;
		GetImageSize_File(name, &size_x, &size_y);
		size.x = (float)size_x;
		size.y = (float)size_y;
	}

	virtual void Init()
	{
	}

	// 更新
	virtual void Update()
	{
		pos.x += move.x;
		pos.y += move.y;
	}

	// キャラを表示する
	virtual void Draw()
	{
		if (alpha < 1.0f)
		{
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(255 * alpha));
			DrawGraphF(pos.x, pos.y, img, TRUE);
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
		else
		{
			DrawGraphF(pos.x, pos.y, img, TRUE);
		}
	}

	// 終了時に呼ぶとキャラを消す
	void Exit()
	{
		// TODO: 同じものを使ったら解放しないようにする
		DeleteGraph(img);
	}

	// 消去する
	void TheEnd() { is_end = true; }

	// 位置
	VECTOR pos = { 0.0f, 0.0f };
	// 移動量
	VECTOR move = { 0.0f, 0.0f };
	// キャラサイズ
	VECTOR size = { 0.0f, 0.0f };

	// 内部用
	bool _isEnd() { return is_end; }

protected:
	// LoadGraphのイメージが入る
	int   img = -1;
	// 半透明度
	float alpha = 1.0f;

	// 終了しているか?
	bool is_end = false;

};

//----------------------------------------------------
// 全オブジェクトの更新と描画と生存確認(消去処理)
//----------------------------------------------------
void ProcessGameObjects()
{
	for (auto& obj : objects)
		obj->Update();

	for (auto& obj : objects)
		obj->Draw();

	// 終了フラグが立っている場合は削除する
	for (int i = (int)objects.size() - 1; i >= 0; --i)
	{
		if (objects[i]->_isEnd())
		{
			delete objects[i];
			objects.erase(objects.begin() + i);
		}
	}
}

//----------------------------------------------------
// 全オブジェクト消去 (基本的に終了時に呼びます)
//----------------------------------------------------
void DeleteGameObjects()
{
	for (auto* obj : objects)
	{
		delete obj;
	}
	objects.clear();
}
