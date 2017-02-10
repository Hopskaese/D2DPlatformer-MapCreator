#pragma once
#include <string>

#include "Graphics.h"
#include "Map.h"
#include "Background.h"
#include "Brick.h"
#include "Dummy.h"
#include "tinyxml2.h"
#include "Common.h"

using namespace tinyxml2;

class Loader {

	static Graphics * m_pGraphics;
public:

	static void Init(Graphics* pGraphics);
	static Map* LoadMap();
	static void BrickRoutine(Map* pMap, XMLElement* xObjectgroup);
	static void DummyRoutine(Map* pMap, XMLElement* xObjectgroup);
	static void BackgroundRoutine(Map* pMap, XMLElement* xMap, XMLElement* xObjectgroup);
	static std::wstring CharToWChar(const char* ptr);
};
