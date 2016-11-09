#include "Map/Map.h"

uint32_t XML_depth     = 0;
uint32_t XML_NthColumn = 0;

Map::Map(Updatable* parent) : Drawable(parent, NULL), m_parser(XML_ParserCreate(NULL)), m_nbCasesX(0), m_nbCasesY(0), m_caseSizeX(0), m_caseSizeY(0)
{
}

void Map::parse(File& file)
{
	//Reinit global variables
	XML_depth = XML_NthColumn = 0;

	//Init XML parser
	XML_SetUserData(m_parser, (void*)this);
	XML_SetElementHandler(m_parser, &Map::startElement, &endElement);

	//Then parse the file
	char* line;
	std::string xmlCode;
	while((line = file.readLine()))
	{
		xmlCode.append(line);
		free(line);
	}
	if(XML_Parse(m_parser, xmlCode.c_str(), xmlCode.size(), 1) == 0)
		LOG_ERROR("Error while parsing the file. Abort");
	XML_ParserFree(m_parser);
}

Map::~Map()
{}

void Map::resetDefaultConf()
{
	setDefaultConf(Rectangle3f(0, 0, 0, m_nbCasesX * m_caseSizeX, m_nbCasesY * m_caseSizeY, 0));
}

void Map::onUpdate(Render& render)
{
}

void Map::onDraw(Render& render, const glm::mat4& mvp)
{
}

void Map::startElement(void* map, const char* name, const char** attrs)
{
	Map* self = (Map*)map;
	//Get Window datas
	if(!strcmp(name, "Window"))
	{
		uint32_t i;
		for(i=0; attrs[i]; i+=2)
		{
			if(!strcmp(attrs[i], "numberCases"))
				getXYFromStr(attrs[i+1], &self->m_nbCasesX, &self->m_nbCasesY);
			else if(!strcmp(attrs[i], "tileSize"))
				getXYFromStr(attrs[i+1], &self->m_caseSizeX, &self->m_caseSizeY);
		}

		self->resetDefaultConf();
	}

	//Set the correct Map::start function following the section (Files for Files, etc.)
	else if(!strcmp(name, "Files"))
		XML_SetElementHandler(self->m_parser, Map::startElementFiles, endElement);
	else if(!strcmp(name, "Objects"))
		XML_SetElementHandler(self->m_parser, Map::startElementObjects, endElement);
	else if(!strcmp(name, "Traces"))
		XML_SetElementHandler(self->m_parser, Map::startElementTraces, endElement);
	XML_depth++;
}

void Map::startElementFiles(void *data, const char* name, const char** attrs)
{
	uint32_t i;
	Map* map = (Map*)data;

	if(XML_depth == 2)
	{
		Texture* texture= NULL; //The image file

		//If the section if for Static files
		if(!strcmp(name, "Static"))
		{
			//Then get the information about it
			uint32_t spacingX, spacingY, tileSizeX, tileSizeY;
			for(i=0; attrs[i]; i+=2)
			{
				//The path
				if(!strcmp(attrs[i], "file"))
				{
					char p[60];
					//In Resources directory
					sprintf(p, "Resources/%s", attrs[i+1]);
					texture = Texture::loadAndroidFile(p);
				}

				//The spacing
                else if(!strcmp(attrs[i], "spacing"))
                    getXYFromStr(attrs[i+1], &(spacingX), &(spacingY));

				//And tile size
                else if(!strcmp(attrs[i], "tileSize"))
                    getXYFromStr(attrs[i+1], &(tileSizeX), &(tileSizeY));
			}
			//Then create it and store it
			StaticFile* sf = new StaticFile(texture, tileSizeX, tileSizeY, spacingX, spacingY);
			map->m_staticFiles.push_back(sf);
		}

		//Or if the file is a Dynamic file
        else if(!strcmp(name, "Dynamic"))
        {
			for(i=0; attrs[i]; i+=2)
			{
				//Get the path of the file
				if(!strcmp(attrs[i], "file"))
				{
					char p[60];
					sprintf(p, "Resources/%s", attrs[i+1]);
					texture = Texture::loadAndroidFile(p);
				}
			}

			//Then create it and store it
			DynamicFile* df = new DynamicFile(texture);
			map->m_dynamicFiles.push_back(df);
        }

		//If a file isn't created
		if(texture==NULL)
		{
			LOG_ERROR("Exit because can't load a file \n");
			return;
		}
		//Store this file (the image one)
		map->m_textures.push_back(texture);
	}

	else if(XML_depth == 3)
	{
		if(map->m_textures.size() == map->m_staticFiles.size()) //Because we load static files before dynamic files, if the len is equal, then the last file was static
		{
			//Get the last static file
			StaticFile* sf = map->m_staticFiles[map->m_staticFiles.size()-1];

			//Create a new StaticDatas
			const char* name;
			const char* type;
			for(uint32_t i=0; attrs[i]; i+=2)
			{
				if(!strcmp(attrs[i], "name"))
					name = attrs[i+1];
				else if(!strcmp(attrs[i], "type"))
					type = attrs[i+1];
			}
			StaticDatas* sd = new StaticDatas();
			sd->createStaticTile = map->getStaticTileFunction(name, type);
			sd->material = map->getStaticTileMaterial(name, type);
			sd->info = map->getStaticTileInfo(name, type);
			//And bind it to the file
			sf->addStaticDatas(sd);
		}

		//If we aren't dealing with static files, then we are with dynamic files
		else
		{
			//Then get the last one
			DynamicFile* df = map->m_dynamicFiles[map->m_dynamicFiles.size()-1];

			//Get common value between animation
			const char* animName;
			for(uint32_t i=0; attrs[i]; i+=2)
			{
				if(!strcmp(attrs[i], "name"))
					animName = attrs[i+1];
			}

			if(!strcmp(name, "dynamicEntity"))
			{
				//Creating a dynamic entity which will contain all the subrect for this animation (dynamic == animation), and the create function for this tile
				DynamicEntity* de = new DynamicEntity(map->getDynamicAnimFunction(name));
				de->material = map->getDynamicAnimMaterial(name);
				de->info = map->getDynamicAnimTileInfo(name);
				df->addDynamicEntity(name, de);
			}

			else if(!strcmp(name, "staticEntity"))
			{
				//get static animation values
				uint32_t n, nX, posX, posY, spacingX, spacingY, sizeX, sizeY;
				for(uint32_t i=0; attrs[i]; i+=2)
				{
					//The position
					if(!strcmp(attrs[i], "pos"))
						getXYFromStr(attrs[i+1], &(posX), &(posY));

					//The spacing
					else if(!strcmp(attrs[i], "spacing"))
						getXYFromStr(attrs[i+1], &(spacingX), &(spacingY));

					//And tile size
					else if(!strcmp(attrs[i], "size"))
						getXYFromStr(attrs[i+1], &(sizeX), &(sizeY));

					else if(!strcmp(attrs[i], "n"))
						n = atoi(attrs[i+1]);

					else if(!strcmp(attrs[i], "nX"))
						nX = atoi(attrs[i+1]);
				}
				
				//Create the static animation entity
				StaticEntity* se = new StaticEntity(map->getStaticAnimFunction(animName), n, nX, posX, posY, sizeX, sizeY, spacingX, spacingY);
				se->material = map->getStaticAnimMaterial(name);
				se->info = map->getStaticAnimTileInfo(name);
				df->addStaticEntity(animName, se);
			}
		}
	}

	//This part is only for dynamic files, which contains position and size for dynamic entities
	else if(XML_depth == 4)
	{
		if(!strcmp(name, "dynamicTile"))
		{
			//Get the last dynamic entity
			DynamicFile* df   = map->m_dynamicFiles[map->m_dynamicFiles.size()-1];
			DynamicEntity* de = df->getLastDynamicEntity();

			//Then create the rect for this animation
			Rectangle2ui* rect = new Rectangle2ui();
			const char* tileName=NULL;
			const char* tileType=NULL;
			uint32_t i;
			for(i=0; attrs[i]; i+=2)
			{
				if(!strcmp(attrs[i], "pos"))
					getXYFromStr(attrs[i+1], &(rect->x), &(rect->y));

				else if(!strcmp(attrs[i], "size"))
					getXYFromStr(attrs[i+1], &(rect->width), &(rect->height));

				else if(!strcmp(attrs[i], "name"))
					tileName = attrs[i+1];

				else if(!strcmp(attrs[i], "type"))
					tileType = attrs[i+1];
			}
			de->addTile(rect, tileName, tileType);
		}

		else if(!strcmp(name, "staticTile"))
		{
			//Get the last static entity
			DynamicFile* df   = map->m_dynamicFiles[map->m_dynamicFiles.size()-1];
			StaticEntity* se  = df->getLastStaticEntity();
			const char* tileName;
			const char* tileType;

			for(uint32_t i=0; attrs[i]; i+=2)
			{
				if(!strcmp(attrs[i], "name"))
					tileName = attrs[i+1];
				else if(!strcmp(attrs[i], "type"))
					tileType = attrs[i+1];
			}
			se->addEntity(tileName, tileType);
		}
	}
	XML_depth++; //We see to the next XML depth
}

void Map::startElementObjects(void *data, const char* name, const char** attrs)
{
	Map* map = (Map*)data;
	if(XML_depth == 2)
	{
		ObjectDatas* objDatas = new ObjectDatas;
		const char* name=NULL;
		const char* type=NULL;

		//Get object information
		for(uint32_t i=0; attrs[i]; i+=2)
		{
			if(!strcmp(attrs[i], "numberCase"))
				getXYFromStr(attrs[i+1], &(objDatas->nbCasesX), &(objDatas->nbCasesY));
			else if(!strcmp(attrs[i], "tileSize"))
				getXYFromStr(attrs[i+1], &(objDatas->tileSizeX), &(objDatas->tileSizeY));

			else if(!strcmp(attrs[i], "name"))
				name = attrs[i+1];
			else if(!strcmp(attrs[i], "type"))
				type = attrs[i+1];
		}
		objDatas->createObject = map->getObjectFunction(name, type);
		map->m_objects.push_back(objDatas);
	}

	//Here we deal with information about tile (what tile it is ?)
	else if(XML_depth == 3)
	{
		char* tileID;
		char* fileID;
		uint32_t i;
		for(i=0; attrs[i]; i+=2)
		{
			//Get the file ID bound to these tiles
			if(!strcmp(attrs[i], "fileID"))
			{
				fileID = (char*)malloc(sizeof(char)*(1+strlen(attrs[i+1])));
				strcpy(fileID, attrs[i+1]);
			}

			//And the tile id for this file
			else if(!strcmp(attrs[i], "tileID"))
			{
				tileID = (char*)malloc(sizeof(char)*(1+strlen(attrs[i+1])));
				strcpy(tileID, attrs[i+1]);
			}
		}

		//Create IntCSV datas
		ObjectDatas* objDatas = map->m_objects[map->m_objects.size()-1];
		objDatas->CSVTileID.push_back(tileID);
		objDatas->CSVFileID.push_back(fileID);
	}

	XML_depth++;
}

void Map::startElementTraces(void *data, const char* name, const char** attrs)
{
	Map* map = (Map*)data;
	if(XML_depth == 2)
	{
		XML_NthColumn=0;
		//Are we dealing with static trace ?
		if(!strcmp(name, "StaticTrace"))
		{
			//Get the size and the padding of it
			uint32_t sizeX, sizeY, padX, padY;
			uint32_t i;
			const char* traceName = NULL;
			for(i=0; attrs[i]; i+=2)
			{
				if(!strcmp(attrs[i], "size"))
					getXYFromStr(attrs[i+1], &sizeX, &sizeY);
				else if(!strcmp(attrs[i], "shift"))
					getXYFromStr(attrs[i+1], &padX, &padY);
				else if(!strcmp(attrs[i], "name"))
					traceName = attrs[i+1];
			}
			//Then create and store it
			StaticTrace* st = new StaticTrace(map, traceName, sizeX, sizeY, sizeX/map->m_caseSizeX*map->m_nbCasesX - (padX+map->m_caseSizeX-1)/map->m_caseSizeX, map->m_nbCasesY * sizeY/map->m_caseSizeY - (padX+map->m_caseSizeY-1)/map->m_caseSizeY, padX, padY);
			map->addTransformable(st);
			map->m_staticTraces.push_back(st);
			map->m_traces.push_back(st);

			//New trace --> we Map::start at the column 0
			XML_NthColumn=0;
		}
		
		//Or create a dynamic trace
		else if(!strcmp(name, "DynamicTrace"))
		{
			const char* traceName = NULL;
			for(uint32_t i=0; attrs[i]; i+=2)
			{
				if(!strcmp(attrs[i], "name"))
					traceName = attrs[i+1];
			}

			//Give the map caseSize and map nbCases for squaring it (useful for collision : we don't check over all the trace !)
			DynamicTrace* dt = new DynamicTrace(map, traceName, map->m_nbCasesX, map->m_nbCasesY, map->m_caseSizeX, map->m_caseSizeY);
			map->addTransformable(dt);
			map->m_dynamicTraces.push_back(dt);
			map->m_traces.push_back(dt);
		}
	}

	else if(XML_depth == 3)
	{
		//If we are with dynamic tile
		if(!strcmp(name, "DynamicTile"))
		{
			//Get information about is
			char animName[50];
			uint32_t fileID;
			int32_t posX, posY;
			uint32_t tileID;

			uint32_t i;
			for(i=0; attrs[i]; i+=2)
			{
				if(!strcmp(attrs[i], "tileID"))
					tileID = atoi(attrs[i+1]);
				else if(!strcmp(attrs[i], "position"))
					getXYFromStr(attrs[i+1], &posX, &posY);
				else if(!strcmp(attrs[i], "fileID"))
					fileID = atoi(attrs[i+1]);
				else if(!strcmp(attrs[i], "animName"))
					strcpy(animName, attrs[i+1]);
			}

			//Get the last dynamic trace
			DynamicTrace* dt = map->m_dynamicTraces[map->m_dynamicTraces.size()-1];
			//The correct dynamic file
			DynamicFile* df = map->m_dynamicFiles[fileID - map->m_staticFiles.size()];
			if(df)
			{
				//Get the entity of this tile
				//Is this entity a dynamic one ?
				DynamicEntity* de = df->getDynamicEntity(name);
				if(de)
				{
					//Then finally create the tile
					Tile* tile = de->createDynamicAnim(NULL, de->material, df->getTexture(), *(de->getSubRects()), de->info, tileID, 8, posX, posY);
					dt->addTile(tile, posX, posY);
				}
				else
				{
					StaticEntity* se = df->getStaticEntity(name);
					if(se)
					{
						Tile* tile = se->createStaticAnim(NULL, df->getTexture(), tileID, posX, posY);
						dt->addTile(tile, posX, posY);
					}
				}
			}
		}

		else if(!strcmp(name, "StaticTile"))
		{
			uint32_t fileID, tileID, posX, posY;

			for(uint32_t i=0; attrs[i]; i+=2)
			{
				if(!strcmp(attrs[i], "position"))
					getXYFromStr(attrs[i+1], &posX, &posY);
				else if(!strcmp(attrs[i], "fileID"))
					fileID = atoi(attrs[i+1]);
				else if(!strcmp(attrs[i], "tileID"))
					tileID = atoi(attrs[i+1]);
			}

			//Get the last dynamic trace
			DynamicTrace* dt = map->m_dynamicTraces[map->m_dynamicTraces.size()-1];
			//The correct dynamic file
			StaticFile* sf = map->m_staticFiles[fileID];

			if(sf)
			{
				Tile* tile = sf->createTile(NULL, tileID, false);
				dt->addTile(tile, posX, posY);
			}
		}
	}

	else if(XML_depth == 4)
	{
		//Column is for static tile in Column tree
		if(!strcmp(name, "Static"))
		{
			//Get the last static trace
			StaticTrace* st = map->m_staticTraces[map->m_staticTraces.size()-1];
			if(st == NULL)
				return;

			//Get tileID, object ID and fileID IntCSV values
			IntCSVParser tileCSVID = IntCSVParser();
			IntCSVParser fileCSVID = IntCSVParser();
			IntCSVParser objectCSVID = IntCSVParser();

			//And parse these IntCSV
			uint32_t i;
			for(i=0; attrs[i]; i+=2)
			{
				if(!strcmp(attrs[i], "fileID"))
					fileCSVID.parse(attrs[i+1]);
				else if(!strcmp(attrs[i], "tileID"))
					tileCSVID.parse(attrs[i+1]);
				else if(!strcmp(attrs[i], "objectID"))
					objectCSVID.parse(attrs[i+1]);
			}

			//Get IntCSV values
			const std::vector<int32_t>* tileID   = tileCSVID.getValues();
			const std::vector<int32_t>* fileID   = fileCSVID.getValues();
			const std::vector<int32_t>* objectID = objectCSVID.getValues();

			//We check if we have normal tiles to parse
			for(i=0; i < tileID->size(); i++)
			{
				if((*tileID)[i] != -1 && (*fileID)[i] != -1 && (*objectID)[i] == 0)
				{
					//Get the file following the file ID (id 0 --> first file created)
					StaticFile* sf  = map->m_staticFiles[(*fileID)[i]];

					//And create this tile
					Tile* tile      = sf->createTile(NULL, (*tileID)[i], false);

					//If the tile is created
					if(tile != NULL)
						st->addTileInTraceCoord(tile, XML_NthColumn, i); //Add it
					LOG_ERROR("TILE ADDED AT %d, %d", XML_NthColumn, i);
				}

				//Then we look for objects
				//Object id is 1 if the tile is an object.
				else if((*objectID)[i] == 1)
				{
					//Get the object datas for this id
					ObjectDatas* objDatas   = map->m_objects[(*tileID)[i]];
					if(objDatas->createObject == NULL)
						continue;

					//Create this object
					LOG_ERROR("MAKE POINTER OBJECT");
					TileObject* obj = objDatas->createObject(NULL, objDatas->nbCasesX, objDatas->nbCasesY, objDatas->tileSizeX, objDatas->tileSizeY, objDatas->info);

					uint32_t j;
					for(j=0; j < objDatas->CSVTileID.size(); j++)
					{
						//Parse IntCSV fileID and tileID
						IntCSVParser objectTileID = IntCSVParser();
						IntCSVParser objectFileID = IntCSVParser();

						objectTileID.parse(objDatas->CSVTileID[j]);
						objectFileID.parse(objDatas->CSVFileID[j]);

						const std::vector<int32_t>* tileID = objectTileID.getValues();
						const std::vector<int32_t>* fileID = objectFileID.getValues();

						//Then create the tiles bound to the Object
						uint32_t k;
						for(k=0; k < tileID->size(); k++)
						{
							//Get the file with the fileID
							StaticFile* sf = map->m_staticFiles[(*fileID)[k]];
							if(sf == NULL)
								continue;

							//And create the tile
							Tile* tile = sf->createTile(NULL, (*tileID)[k], false);
							if(tile->getMaterial())
								LOG_ERROR("TILE HAS MATERIAL");
							if(tile == NULL)
								continue;

							//Then add this tile
							obj->addTile(tile, j, k); //Because of OpenGL convention
						}
					}
					st->addTileInTraceCoord(obj, XML_NthColumn, i);
					LOG_ERROR("OBJECT ADDED AT %d, %d", XML_NthColumn, i);
				}
			}
		}

		//Handle Animation tile in StaticTrace
		else if(!strcmp(name, "Animation"))
		{
			//Get the last StaticTrace
			StaticTrace* st = map->m_staticTraces[map->m_staticTraces.size()-1];

			//Parse all the parameters
			IntCSVParser tileCSVID = IntCSVParser();
			IntCSVParser fileCSVID = IntCSVParser();
			StrCSVParser nameCSVID = StrCSVParser();

			for(uint32_t i=0; attrs[i]; i+=2)
			{
				if(!strcmp(attrs[i], "fileID"))
					fileCSVID.parse(attrs[i+1]);
				else if(!strcmp(attrs[i], "tileID"))
					tileCSVID.parse(attrs[i+1]);
				else if(!strcmp(attrs[i], "name"))
					nameCSVID.parse(attrs[i+1]);
			}

			LOG_ERROR("GET VALUES");

			//Get IntCSV values
			const std::vector<int32_t>*     tileID   = tileCSVID.getValues();
			const std::vector<int32_t>*     fileID   = fileCSVID.getValues();
			const std::vector<std::string>* names    = nameCSVID.getValues();

			for(uint32_t i=0; i < tileID->size(); i++)
			{
				if((*fileID)[i] == -1 || (*tileID)[i] == -1)
					continue;
				DynamicFile* df  = map->m_dynamicFiles[(*fileID)[i] - map->m_staticFiles.size()];
				if(df)
				{
					StaticEntity* se = df->getStaticEntity((*names)[i]);
					if(se)
					{
						//Need to be changed
						Tile* tile = se->createStaticAnim(NULL, df->getTexture(), (*tileID)[i], 0, 0);
						st->addTile(tile, XML_NthColumn, i);
					}
				}
			}
		}
	}

	XML_depth++;
}

void Map::endElement(void *data, const char* name)
{
	XML_depth--;
	//Move to the next column of the trace
	if(XML_depth == 3 && !strcmp(name, "Column"))
		XML_NthColumn++;
	if(XML_depth == 1)
		XML_SetElementHandler(((Map*)data)->m_parser, Map::startElement, Map::endElement);
}

createStaticTilePtr Map::getStaticTileFunction(const char* name, const char* type) const
{
	return NULL;
}

Material* Map::getStaticTileMaterial(const char* name, const char* type)
{
	return NULL;
}

void* Map::getStaticTileInfo(const char* name, const char* type)
{
	return NULL;
}

createDynamicAnimPtr Map::getDynamicAnimFunction(const char* name) const
{
	return NULL;
}

Material* Map::getDynamicAnimMaterial(const char* name)
{
	return NULL;
}

void* Map::getDynamicAnimTileInfo(const char* name)
{
	return NULL;
}

createStaticAnimPtr Map::getStaticAnimFunction(const char* name) const
{
	return NULL;
}

Material* Map::getStaticAnimMaterial(const char* name)
{
	return NULL;
}

void* Map::getStaticAnimTileInfo(const char* name)
{
	return NULL;
}

createObjectPtr Map::getObjectFunction(const char* name, const char* type) const
{
	return NULL;
}

void* Map::getObjectTileInfo(const char* name, const char* type)
{
	return NULL;
}

Tile* Map::getTileWorldCoords(double x, double y)
{
	Tile* tile=NULL;
	for(std::vector<Trace*>::reverse_iterator it = m_traces.rbegin(); it != m_traces.rend(); it++)
	{
		if((tile = (*it)->getTileWorldCoords(x, y)) != NULL)
			return tile;
	}
	return NULL;
}

Tile* Map::getTileWorldCoords(double x, double y, const char* traceName)
{
	for(std::vector<Trace*>::reverse_iterator it = m_traces.rbegin(); it != m_traces.rend(); it++)
	{
		if(!strcmp((*it)->getName().c_str(), traceName))
			return (*it)->getTileWorldCoords(x, y);
	}
	return NULL;

}

Tile* Map::getTileTraceCoords(int x, int y)
{
	Tile* tile=NULL;
	for(std::vector<Trace*>::reverse_iterator it = m_traces.rbegin(); it != m_traces.rend(); it++)
	{
		if((tile = (*it)->getTileTraceCoords(x, y)) != NULL)
			return tile;
	}
	return NULL;
}

Tile* Map::getTileTraceCoords(int x, int y, const char* traceName)
{
	for(std::vector<Trace*>::reverse_iterator it = m_traces.rbegin(); it != m_traces.rend(); it++)
	{
		if(!strcmp((*it)->getName().c_str(), traceName))
			return (*it)->getTileTraceCoords(x, y);
	}
	return NULL;

}

Tile* Map::addTile(Tile* tile, uint32_t x, uint32_t y, uint32_t traceID)
{
	Tile* t = m_traces[traceID]->getTileTraceCoords(x, y);
	m_traces[traceID]->addTile(tile, x, y);
	return t;
}

Tile* Map::addTile(Tile* tile, uint32_t x, uint32_t y, const char* traceName)
{
	for(std::vector<Trace*>::reverse_iterator it = m_traces.rbegin(); it != m_traces.rend(); it++)
	{
		if(!strcmp((*it)->getName().c_str(), traceName))
		{
			Tile* t = (*it)->getTileTraceCoords(x, y);
			(*it)->addTile(tile, x, y);
			return t;
		}
	}
	return NULL;
}

bool Map::isOutside(uint32_t x, uint32_t y) const
{
	return (x > m_caseSizeX*m_nbCasesX || y > m_caseSizeY*m_nbCasesY);
}

Vector2i Map::getNbCases() const
{
	return Vector2i(m_nbCasesX, m_nbCasesY);
}

Vector2i Map::getCasesSize() const
{
	return Vector2i(m_caseSizeX, m_caseSizeY);
}
