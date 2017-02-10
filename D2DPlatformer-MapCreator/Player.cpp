#include "Player.h"
#include "Brick.h"


Player::Player(float fX, float fY)
	: Object(fX, fY, OT_PLAYER)
{
	m_bFalling = true;
	m_byJump = 0;
	m_bDirection = DIRECTION::RIGHT;
	m_n64PState = 0;

	for (int i = 0; i < PLAYER_SPRITE_LEN; i++)
		for (int j = 0; j < PLAYER_DIRECTIONS; j++)
		m_pSprites[i][j] = NULL;
}

Player::~Player()
{
	for (int i = 0; i < PLAYER_SPRITE_LEN; i++)
		for (int j = 0; j < PLAYER_DIRECTIONS; j++)
			if (m_pSprites[i][j]) delete m_pSprites[i][j];
}

void Player::Move(double dt)
{
	if (!IsPState(Player::PS_RUN)) return;

	m_bDirection == Player::LEFT ? MoveLeft(dt) : MoveRight(dt);
}

void Player::MoveLeft(double dt)
{
	m_fX -= PLAYER_SPEED * (float)dt;
}

void Player::MoveRight(double dt)
{
	m_fX += PLAYER_SPEED * (float)dt;
}

void Player::Jump()
{
	m_fSpeedY = PLAYER_JUMPPOWER;

	AddPState(Player::PS_JUMP);

	m_pSprites[Player::PSPRITE_JUMP][Player::DIRECTION::LEFT]->RestartAnimation();
	m_pSprites[Player::PSPRITE_JUMP][Player::DIRECTION::RIGHT]->RestartAnimation();
}
