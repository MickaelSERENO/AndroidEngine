#include "Map/MapFiles.h"

StaticFile::StaticFile(Texture* texture, uint32_t spacX, uint32_t spacY, uint32_t sizeX, uint32_t sizeY) : m_texture(texture), m_spacingX(spacX), m_spacingY(m_spacY), m_tileSizeX(sizeX), m_tileSizeY(sizeY)
{
}

StaticFile::~StaticFile()
{
	for(uint32_t i=0; i < m_tileDatas.size(); i++)
		if(m_tileDatas[i])
			delete m_tileDatas[i];
}

void StaticFile::addTileDatas(TileDatas* tileDatas)
{
	m_tileDatas.push_back(tileDatas);
}

Tile* StaticFile::createTile(uint32_t tileID, bool def)
{
	if(tileID >= m_tileDatas.size())
		return NULL;
	if(!m_tileDatas[tileID])
		return NULL;

    //Get how many tiles we have in a row.
    //True value : (tWidth - tileSizeX + tileSizeX + spacingX -1) / (tileSizeX + spacingX) + 1 
    //where tileSizeX + spacingX -1 is for rounded the value to the upper case if needed;
	uint32_t numberTileX = (m_texture.getWidth() + m_spacingX - 1) / (m_spacingX + m_tileSizeX) + 1;
	Rectangle2f subRect;

	subRect.x      = (tileID % numberTileX) * (m_tileSizeX + m_spacingX);
	subRect.y      = (tileID / numberTileX) * (m_tileSizeY + m_spacingY);
	subRect.width  = m_tileSizeX;
	subRect.height = m_tileSizeY;

    if(!def)
        return tile->createStaticTile(m_texture, &subRect);
    return new DefaultTile(texture, &subRect);
}

DynamicFile::DynamicFile(Texture* texture) : m_texture(texture)
{}

DynamicFile::~DynamicFile()
{
    for(std::map<std::string, DynamicEntity*>::iterator it=m_dynamicEntities.begin(); it != m_dynamicEntities.end(); it++)
        if(it->second)
            delete it->second;
}

void DynamicFile::addDynamicEntity(const std::string& key, DynamicEntity* entity)
{
    m_dynamicEntities.insert(std::pair<std::string, DynamicEntity*>(key, entity));
}

void DynamicFile::getLastDynamicEntity()
{
    return (m_dynamicEntities.rbegin()->second);
}