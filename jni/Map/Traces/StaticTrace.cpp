#include "Map/Traces/StaticTrace.h"

StaticTrace::StaticTrace(Updatable* parent, const std::string& name, uint32_t sizeX, uint32_t sizeY, uint32_t nbCasesX, uint32_t nbCasesY,
						 uint32_t padX, uint32_t padY) : Trace(parent, Rectangle3f(padX, padY, 0.0, sizeX * nbCasesX + padX, sizeY * nbCasesY + padY, 0.0), name),
						m_nbCasesX(nbCasesX), m_nbCasesY(nbCasesY), m_sizeX(sizeX), m_sizeY(sizeY), m_padX(padX), m_padY(padY)
{
	for(uint32_t i=0; i < nbCasesX; i++)
	{
		m_tiles.push_back(std::vector<Tile*>());
		for(uint32_t j=0; j < nbCasesY; j++)
			m_tiles[i].push_back(NULL);
	}
}

Tile* StaticTrace::getTileWorldCoords(double x, double y)
{
	glm::mat4 invMvp = glm::inverse(getMatrix());
	glm::vec4      v = invMvp * glm::vec4(x, y, 0.0, 1.0);
	v.y              = m_nbCasesY * m_sizeY + m_padY - v.y;
	return getTileTraceCoords(v.x, v.y);
}

Vector2i StaticTrace::getTileCoords(double x, double y) const
{
	glm::mat4 invMvp = glm::inverse(getMatrix());
	glm::vec4      v = invMvp * glm::vec4(x, y, 0.0, 1.0);
	v.y              = m_nbCasesY * m_sizeY + m_padY - v.y;
	return Vector2i((v.x - m_padX)/m_sizeX, (v.y - m_padY)/m_sizeY);
}

Tile* StaticTrace::getTileTraceCoords(int x, int y)
{
	if(x < 0 || y < 0 || x >= m_nbCasesX * m_sizeX - m_padX || y >= m_nbCasesY * m_sizeY - m_padY)
		return NULL;
	Tile* tile = m_tiles[(x - m_padX)/m_sizeX][(y - m_padY)/m_sizeY];
	return tile;

}

void StaticTrace::addTile(Tile* tile, uint32_t x, uint32_t y)
{
	bool isAdded = false;
	for(uint32_t i = 0; i < tile->getNbCasesX(); i++)
	{
		if(x >= (m_nbCasesX - i)* m_sizeX - m_padX)
			break;

		for(uint32_t j=0; j < tile->getNbCasesY(); j++)
		{
			if(y >= (m_nbCasesY - j ) * m_sizeY - m_padY)
				break;
			isAdded = true;
			m_tiles[(x-m_padX)/m_sizeX + i][(y-m_padY)/m_sizeY + j] = tile;
		}
	}

	if(isAdded)
		Trace::addTile(tile, x, m_nbCasesY * m_sizeY - y);
}

void StaticTrace::addTileInTraceCoord(Tile* tile, uint32_t x, uint32_t y)
{
	bool isAdded = false;
	for(uint32_t i=0; i < tile->getNbCasesX(); i++)
	{
		if(x + i >= m_nbCasesX)
			break;
		for(uint32_t j=0; j < tile->getNbCasesY(); j++)
		{
			if(y  + j >= m_nbCasesY)
				break;
			isAdded = true;
			m_tiles[x+i][y+j] = tile;
		}
	}

	if(isAdded)
		Trace::addTile(tile, x*m_sizeX + m_padX, (m_nbCasesY - y - tile->getNbCasesY()) * m_sizeY + m_padY);
}
