#include "Map.h"

Map::~Map()
{
	for (auto o : m_objects)
		delete o;
}

void Map::Add(Player* pPlayer)
{
	m_objects.push_back(pPlayer);
	m_players.push_back(pPlayer);
}

void Map::Add(Brick* pBrick)
{
	m_objects.push_back(pBrick);
	m_bricks.push_back(pBrick);
}

void Map::DeleteFromObjects(Object* pObject)
{
	auto it = find_if(m_objects.begin(), m_objects.end(), [&](const Object* obj) {return obj->m_nID == pObject->m_nID; });

	if (it != m_objects.end())
		m_objects.erase(it);
}

void Map::DeleteFromBricks(Brick* pBrick)
{
	auto it = find_if(m_bricks.begin(), m_bricks.end(), [&](const Brick* obj) {return obj->m_nID == pBrick->m_nID; });

	if (it != m_bricks.end())
		m_bricks.erase(it);
}

void Map::Add(Background* pBackground)
{
	m_objects.push_back(pBackground);
	m_pBackground = pBackground;
}

void Map::Add(Dummy* pDummy)
{
	m_objects.push_back(pDummy);
	m_dummies.push_back(pDummy);
}

Object* Map::GetGround(Object* pObject, double dt)
{
	if (!Object::instanceof<Player>(pObject)) return nullptr;

	auto pPlayer = (Player*)pObject;
	if (!pPlayer->m_fSpeedY) return nullptr;

	for (auto o : m_objects)
	{
		if (pPlayer->m_fSpeedY > 0)
		{
			if (o->IsOverlap(pPlayer->m_fX, pPlayer->m_fY, pPlayer->m_fX, pPlayer->m_fY + pPlayer->m_fSpeedY * (float)dt)) return o;
		}
		else
		{
			if (o->IsOverlap(pPlayer->m_fX, pPlayer->m_fY + pPlayer->m_fSpeedY * (float)dt, pPlayer->m_fX, pPlayer->m_fY)) return o;
		}
	}

	return nullptr;
}

Brick * Map::GetGround(Player * pPlayer, double dt)
{
	float fPlayerY = pPlayer->m_fSpeedY * (float)dt + pPlayer->m_fY;

	for (auto pBrick : m_bricks)
	{
		if (pBrick->m_fX <= pPlayer->m_fX && pBrick->m_fX + pBrick->m_fWidth >= pPlayer->m_fX)
		{
			float fBrickY = pBrick->GetHeight(pPlayer->m_fX);
			if (fPlayerY >= fBrickY && pPlayer->m_fY <= fBrickY)
			{
				return pBrick;
			}
		}
	}

	return NULL;
}

bool Map::IsFalling(Object* pObject, double dt)
{
	if (!Object::instanceof<Player>(pObject)) return false;

	auto pPlayer = (Player*)pObject;
	if (!pPlayer->m_fSpeedY) return false;

	for (auto o : m_objects)
	{
		if (pPlayer->m_fSpeedY > 0)
		{
			if (o->IsOverlap(pPlayer->m_fX, pPlayer->m_fY, pPlayer->m_fX, pPlayer->m_fY + pPlayer->m_fSpeedY * (float)dt)) return false;
		}
		else
		{
			if (o->IsOverlap(pPlayer->m_fX, pPlayer->m_fY + pPlayer->m_fSpeedY * (float)dt, pPlayer->m_fX, pPlayer->m_fY)) return false;
		}
	}

	return true;
}

bool Map::CanMoveLeft(Object* pObject, double dt)
{
	return true;
}

bool Map::CanMoveRight(Object* pObject, double dt)
{
	return true;
}

Object* Map::FindObject(float fX1, float fY1, float fX2, float fY2)
{
	for (auto o : m_objects)
	{
		if (o->IsOverlap(fX1, fY1, fX2, fY2))
			return o;
	}

	return nullptr;
}