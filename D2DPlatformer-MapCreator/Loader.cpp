#include "Loader.h"

Graphics* Loader::m_pGraphics;

void Loader::Init(Graphics *pGraphics)
{
	m_pGraphics = pGraphics;
}

Map* Loader::LoadMap()
{
	tinyxml2::XMLDocument doc;

	if (doc.LoadFile("Map.xml") != XML_SUCCESS)
	{
		printf("Error loading xml file \n");
		return NULL;
	}

	XMLElement *p_Root = doc.RootElement();
	if (!p_Root)
	{
		printf("Error getting root element \n");
		return NULL;
	}

	XMLElement* xMap = p_Root->FirstChildElement("map");
	if (xMap == NULL)
	{
		printf("Error getting map element\n");
		return NULL;
	}

	Map *pMap = new Map();
	XMLElement* xObjectgroup = xMap->FirstChildElement("objectgroup");

	//main-loop
	while (xObjectgroup != NULL)
	{
		if (xObjectgroup->IntAttribute("index") == OT_BACKGROUND)
			BackgroundRoutine(pMap, xMap, xObjectgroup);
		else if (xObjectgroup->IntAttribute("index") == OT_BRICK)
			BrickRoutine(pMap, xObjectgroup);
		else if (xObjectgroup->IntAttribute("index") == OT_DUMMY)
			DummyRoutine(pMap, xObjectgroup);

		xObjectgroup = xObjectgroup->NextSiblingElement("objectgroup");
	}

	return pMap;
}

void Loader::BackgroundRoutine(Map* pMap, XMLElement* xMap, XMLElement* xObjectgroup)
{
	float fWidth = 0.0f;
	float fHeight = 0.0f;
	int bIndex = 0;
	const char* pSrc = NULL;
	float fSpeedX = 0.0f;
	float fSpeedY = 0.0f;

	XMLElement* xBackground = xObjectgroup->FirstChildElement("background");

	pMap->m_fMaxY = xMap->FloatAttribute("MaxY");
	pMap->m_fMaxX = xMap->FloatAttribute("MaxX");
	pMap->m_fMinX = xMap->FloatAttribute("MinX");

	xBackground = xObjectgroup->FirstChildElement("background");

	Background* pBackground;
	fWidth = xBackground->FloatAttribute("width");
	fHeight = xBackground->FloatAttribute("height");

	pBackground = new Background(fWidth, fHeight);
	bIndex = xMap->IntAttribute("index");
	fWidth = xMap->FloatAttribute("width");
	fHeight = xMap->FloatAttribute("height");

	pSrc = xMap->Attribute("src");

	std::wstring temp = Loader::CharToWChar(pSrc);

	pBackground->m_pSpriteSheet = new SpriteSheet((wchar_t*)temp.c_str(), m_pGraphics, fWidth, fHeight, 0, false);
	XMLElement* xLayer = xBackground->FirstChildElement("layer");

	while (xLayer != NULL)
	{
		bIndex = xLayer->IntAttribute("index");
		fSpeedX = xLayer->FloatAttribute("speedX");
		fSpeedY = xLayer->FloatAttribute("speedY");

		pBackground->AddLayer(bIndex, fSpeedX, fSpeedY);
		xLayer = xLayer->NextSiblingElement("layer");
	}
	pMap->Add(pBackground);
}

void Loader::BrickRoutine(Map* pMap, XMLElement* xObjectgroup)
{
	const char* pSrc = NULL;
	float fX = 0.0f;
	float fY = 0.0f;
	float fWidth = 0.0f;
	float fHeight = 0.0f;
	float fAngle = 0.0f;

	Brick *pBrick;
	XMLElement* xImg = xObjectgroup->FirstChildElement("image");

	while (xImg != NULL)
	{
		pSrc = xImg->Attribute("src");
		XMLElement* xObject = xImg->FirstChildElement("object");

		while (xObject != NULL)
		{
			fX = xObject->FloatAttribute("x");
			fY = xObject->FloatAttribute("y");
			fWidth = xObject->FloatAttribute("width");
			fHeight = xObject->FloatAttribute("height");
			fAngle = xObject->FloatAttribute("angle");

			pBrick = new Brick(fX, fY, fWidth, fHeight, fAngle);

			std::wstring temp = Loader::CharToWChar(pSrc);
			pBrick->m_pSpriteSheet = new SpriteSheet((wchar_t*)temp.c_str(), m_pGraphics);
			pMap->Add(pBrick);

			xObject = xObject->NextSiblingElement("object");
		}
		xImg = xImg->NextSiblingElement("image");
	}
}

void Loader::DummyRoutine(Map* pMap, XMLElement* xObjectgroup)
{
	const char* pSrc = NULL;
	float fX = 0.0f;
	float fY = 0.0f;

	Dummy* pDummy;
	XMLElement* xImg = xObjectgroup->FirstChildElement("image");

	while (xImg != NULL)
	{
		pSrc = xImg->Attribute("src");
		XMLElement* xObject = xImg->FirstChildElement("object");

		while (xObject != NULL)
		{
			fX = xObject->FloatAttribute("x");
			fY = xObject->FloatAttribute("y");

			pDummy = new Dummy(fX, fY);
			std::wstring temp = Loader::CharToWChar(pSrc);
			pDummy->m_pSpriteSheet = new SpriteSheet((wchar_t*)temp.c_str(), m_pGraphics);
			pMap->Add(pDummy);

			xObject = xObject->NextSiblingElement("object");
		}
		xImg = xImg->NextSiblingElement("image");
	}
}

std::wstring Loader::CharToWChar(const char* ptr)
{
	std::string temp = ptr;

	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &temp[0], (int)temp.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &temp[0], (int)temp.size(), &wstrTo[0], size_needed);

	return wstrTo;
}