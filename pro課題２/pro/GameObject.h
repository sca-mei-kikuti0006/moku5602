#pragma once
/**
 * @file GameObject
 */

#include <dxlib.h>
#include <vector>

 //----------------------------------------------------
 // �I�u�W�F�N�g�̃��X�g
 //----------------------------------------------------
class GameObject;
std::vector<GameObject*> objects;

//----------------------------------------------------
// �I�u�W�F�N�g�쐬
//----------------------------------------------------
template<typename T>
T& CreateGameObject()
{
	objects.push_back(new T());
	auto* obj = (T*)objects[objects.size() - 1];

	obj->Init();
	return *obj;
}

//! �Q�[���I�u�W�F�N�g�̎擾
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
// ���ݑ��݂��Ă���I�u�W�F�N�g���擾����
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
// ���݉���GameObject�����݂��Ă��邩�m�F�p
//----------------------------------------------------
size_t CountGameObjects()
{
	return objects.size();
}

//----------------------------------------------------
// ���̃I�u�W�F�N�g�N���X
//----------------------------------------------------
class GameObject
{
public:
	GameObject()
	{
	}

	// virtual�K�v
	virtual ~GameObject()
	{
	}

	// img ���B�����邽�߂ɗp��
	void Load(const TCHAR* name)
	{
		img = LoadGraph(name);
		// �C���[�W�̃T�C�Y���擾����
		int size_x, size_y;
		GetImageSize_File(name, &size_x, &size_y);
		size.x = (float)size_x;
		size.y = (float)size_y;
	}

	virtual void Init()
	{
	}

	// �X�V
	virtual void Update()
	{
		pos.x += move.x;
		pos.y += move.y;
	}

	// �L������\������
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

	// �I�����ɌĂԂƃL����������
	void Exit()
	{
		// TODO: �������̂��g�����������Ȃ��悤�ɂ���
		DeleteGraph(img);
	}

	// ��������
	void TheEnd() { is_end = true; }

	// �ʒu
	VECTOR pos = { 0.0f, 0.0f };
	// �ړ���
	VECTOR move = { 0.0f, 0.0f };
	// �L�����T�C�Y
	VECTOR size = { 0.0f, 0.0f };

	// �����p
	bool _isEnd() { return is_end; }

protected:
	// LoadGraph�̃C���[�W������
	int   img = -1;
	// �������x
	float alpha = 1.0f;

	// �I�����Ă��邩?
	bool is_end = false;

};

//----------------------------------------------------
// �S�I�u�W�F�N�g�̍X�V�ƕ`��Ɛ����m�F(��������)
//----------------------------------------------------
void ProcessGameObjects()
{
	for (auto& obj : objects)
		obj->Update();

	for (auto& obj : objects)
		obj->Draw();

	// �I���t���O�������Ă���ꍇ�͍폜����
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
// �S�I�u�W�F�N�g���� (��{�I�ɏI�����ɌĂт܂�)
//----------------------------------------------------
void DeleteGameObjects()
{
	for (auto* obj : objects)
	{
		delete obj;
	}
	objects.clear();
}
