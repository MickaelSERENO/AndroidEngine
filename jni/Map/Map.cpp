#include "Map/Map.h"

uint32_t XML_depth     = 0;
uint32_t XML_NthColumn = 0;

Map::Map(Updatable* parent, File& file) : Updatable(parent), GroupTransformable(), m_parser(XML_ParserCreate(NULL)), m_nbCasesX(0), m_nbCasesY(0), m_caseSizeX(0), m_caseSizeY(0)
{
	//Reinit global variables
	XML_depth = XML_NthColumn = 0;

	//Init XML parser
	XML_SetUserData(m_parser, (void*)this);
	XML_SetElementHandler(m_parser, &startElement, &endElement);

	//Then parse the file
	std::string xmlCode;
	char* line;
	while((line = file.readLine()) != NULL)
	{
		if(line == NULL)
			break;
		xmlCode.append(line);
		free(line);
	}
	if(XML_Parse(m_parser, xmlCode.c_str(), xmlCode.size(), 1) == 0)
		LOG_ERROR("Error while parsing the file. Abort");
	XML_ParserFree(m_parser);
}

void startElement(void* map, const char* name, const char** attrs)
{
	Map* self = (Map*)map;
	//Get Window datas
	if(!strcmp(name, "Window"))
	{
		uint32_t i;
		for(i=0; attrs[i]; i+=2)
		{
			if(!strcmp(attrs[i], "numberCases"))
				getXYFromStr(attrs[i+1], &self->nbCasesX, &self->nbCasesY);
			else if(!strcmp(attrs[i], "tileSize"))
				getXYFromStr(attrs[i+1], &self->caseSizeX, &self->caseSizeY);
		}
	}

	//Set the correct start function following the section (Files for Files, etc.)
	else if(!strcmp(name, "Files"))
		XML_SetElementHandler(self->m_parser, startElementFiles, endElement);
	else if(!strcmp(name, "Objects"))
		XML_SetElementHandler(self->m_parser, startElementObjects, endElement);
	else if(!strcmp(name, "Traces"))
		XML_SetElementHandler(self->m_parser, startElementTraces, endElement);
	XML_depth++;
}

void startElementFiles(void *data, const char* name, const char** attrs)
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
			map->m_staticTraces.push_back(sf);
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
			map->m_dynamicFile.push_back(df);
        }

		//If a file isn't created
		if(file==NULL)
		{
			LOG_ERROR("Exit because can't load a file \n");
			return;
		}
		//Store this file (the image one)
		map->m_texture.push_back(texture);
	}

	else if(XML_depth == 3)
	{
		if(map->m_texture.size() == map->m_staticFiles.size()) //Because we load static files before dynamic files, if the len is equal, then the last file was static
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
			//And bind it to the file
			sf->addStaticDatas(sd);
		}

		//If we aren't dealing with static files, then we are with dynamic files
		else
		{
			//Then get the last one
			DynamicFile* df = map->m_dynamicFiles[map->m_dynamicFiles.size()-1];

			//Creating a dynamic entity which will contain all the subrect for this animation (dynamic == animation), and the create function for this tile
			const char* name;
			const char* type;
			for(uint32_t i=0; attrs[i]; i+=2)
			{
				if(!strcmp(attrs[i], "name"))
					name = attrs[i+1];
				else if(!strcmp(attrs[i], "type"))
					type = attrs[i+1];
			}
			DynamicEntity* de = new dynamicEntity();
			de->createDynamicTile = map->getDynamicTileFunction(name, type);
			df->addDynamicEntity(name, de);
		}
	}

	//This part is only for dynamic files, which contains position and size for dynamic entities
	else if(XML_depth == 4)
	{
		//Get the last dynamic entity
		DynamicFile* df   = map->m_dynamicFiles[map->m_dynamicFiles.size()-1];
		DynamicEntity* de = df->getLastDynamicEntity();

		//Then create the rect for this animation
		Rectangle2f* rect = new Rectangle2f();
		uint32_t i;
		for(i=0; attrs[i]; i+=2)
		{
			if(!strcmp(attrs[i], "pos"))
				getXYFromStr(attrs[i+1], &(rect->x), &(rect->y));

			else if(!strcmp(attrs[i], "size"))
				getXYFromStr(attrs[i+1], &(rect->width), &(rect->height));
		}
		de->tileRects.push_back(rect);
	}
	XML_depth++; //We see to the next XML depth
}

void startElementObjects(void *data, const char* name, const char** attrs)
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

		//Create CSV datas
		ObjectDatas* objDatas = map->m_objects[map->m_objects.size()-1];
		objDatas->CSVTileID.push_back(tileID);
		objDatas->CSVFileID.push_back(fileID);
	}

	XML_depth++;
}

void startElementTraces(void *data, const char* name, const char** attrs)
{
	Map* map = (Map*)data;
	if(XML_depth == 2)
	{
		//Are we dealing with static trace ?
		if(!strcmp(name, "StaticTrace"))
		{
			//Get the size and the padding of it
			uint32_t sizeX, sizeY, padX, padY;
			uint32_t i;
			for(i=0; attrs[i]; i+=2)
			{
				if(!strcmp(attrs[i], "size"))
					getXYFromStr(attrs[i+1], &sizeX, &sizeY);
				else if(!strcmp(attrs[i], "shift"))
					getXYFromStr(attrs[i+1], &padX, &padY);
			}
			//Then create and store it
			StaticTrace* st = new StaticTrace(sizeX, sizeY, map->m_nbCasesX * ((sizeX-padX)/map->m_caseSizeX), map->nbCasesY * ((sizeY-padY) / map->m_caseSizeY), padX, padY);
			map->addTransformable(st);
			map->m_staticTraces.push_back(st);

			//New trace --> we start at the column 0
			XML_NthColumn=0;
		}
		
		//Or create a dynamic trace
		else if(!strcmp(name, "DynamicTrace"))
		{
			//Give the map caseSize and map nbCases for squaring it (useful for collision : we don't check over all the trace !)
			DynamicTrace* dt = DynamicTrace_create(map->m_nbCasesX, map->m_nbCasesY, map->m_caseSizeX, map->m_caseSizeY);
			map->addTransformable(dt);
			map->m_dynamicTraces.push_back(dt);
		}
	}

	else if(XML_depth == 3)
	{
		//Column is for static tile
		if(!strcmp(name, "Column"))
		{
			//Get the last static trace
			StaticTrace* st = map->m_staticTraces[map->m_staticTraces.size()-1];

			//Get tileID, object ID and fileID CSV values
			CSVParser tileCSVID = CSVParser();
			CSVParser fileCSVID = CSVParser();
			CSVParser objectCSVID = CSVParser();

			//And parse these CSV
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

			//Get CSV values
			const int32_t* tileID   = tileCSVID.getValues();
			const int32_t* fileID   = fileCSVID.getValues();
			const int32_t* objectID = objectCSVID.getValue();

			//We check if we have normal tiles to parse
			for(i=0; i < tileCSVID.getLen(); i++)
			{
				if(tileID[i] != -1 && fileID[i] != -1)
				{
					//Get the file following the file ID (id 0 --> first file created)
					StaticFile* sf  = map->m_staticFiles[fileID[i]];

					//And create this tile
					Tile* tile      = sf->createTile(tileID[i], false);

					//If the tile is created
					if(tile != NULL)
					{
						if(st != NULL)
							st->addTile(tile, XML_NthColumn, i); //Add it
					}
				}

				//Then we look at the objects
				//Object id start from 1.
				else if(objectID[i] > 0)
				{
					//Get the object datas for this id
					ObjectDatas* objDatas   = map->m_objects[objectID[i]-1];
					if(objDatas->createObject == NULL)
						continue;

					//Create this object
					Object* obj = objDatas->createObject(objDatas->nbCasesX, objDatas->nbCasesY, objDatas->tileSizeX, objDatas->tileSizeY);

					uint32_t j;
					for(j=0; j < objDatas->CSVTileID.size(); j++)
					{
						//Parse CSV fileID and tileID
						CSVParser objectTileID = CSVParser();
						CSVParser objectFileID = CSVParser();

						objectTileID.parse(objDatas->CSVTileID[j]);
						objectFileID.parse(objDatas->CSVFileID[j]);

						const uint32_t* tileID = objectTileID.getValues();
						const uint32_t* fileID = objectFileID.getValues();

						//Then create the tiles bound to the Object
						uint32_t k;
						for(k=0; k < objectTileID.getLen(); k++)
						{
							//Get the file with the fileID
							StaticFile* sf = map->m_staticFiles[fileID[k]];
							if(sf == NULL)
								continue;

							//And create the tile
							Tile* tile = sf->createTile(tileID[k], true);
							if(tile == NULL)
								continue;

							//Then add this tile
							obj->addTile(j, k, tile);
						}
					}
					st->addTile(obj, XML_NthColumn, i);
				}
			}
			XML_NthColumn++;
		}

		//If we are with dynamic tile
		else if(!strcmp(name, "DynamicTile"))
		{
			//Get information about is
			char name[50];
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
					strcpy(name, attrs[i+1]);
			}

			//Get the last dynamic trace
			DynamicTrace* dt = map->m_dynamicTraces[map->m_dynamicTraces.size()-1];

			//The correct dynamic file
			DynamicFile* df = map->m_dynamicFiles[fileID - map->m_staticFiles.size()];
			if(df)
			{
				uint32_t i;
				//Get the entity of this tile
				DynamicEntity* de = df->getDynamicEntity(name);

				//Get the texture rect for this animation
				Rectangle2f* textureRect = de->tileRects[tileID];

				//Set the tile position
				Rectangle2f dest;
				dest.x      = posX;
				dest.y      = posY;
				dest.width  = textureRect->w;
				dest.height = textureRect->h;

				//Then finally create the tile
				Tile* tile = de->createDynamicTile(df->material, df->file->getTexture(), dest, textureRect, de->tileRects->size(), 0, 8);
				dt->addTile(tile);
				free(subRects);
			}
		}
	}

	XML_depth++;
}

void endElement(void *data, const char* name)
{
	XML_depth--;
	if(XML_depth == 1)
		XML_SetElementHandler(((Map*)data)->m_parser, startElement, endElement);
}

void getXYFromStr(const char* str, uint32_t* x, uint32_t* y)
{
    uint32_t i;
    for(i=0; str[i] != 'x' && str[i] != 'X' && str[i] != '\0'; i++);

    if(str[i] != '\0')
    {
        *x = atoi(str);
        *y = atoi(&(str[i+1]));
    }
}

createStaticTilePtr Map::getStaticTileFunction(const char* name, const char* type) const
{
	return NULL;
}

Material* Map::getStaticTileMaterial(const char* name, const char* type) const
{
	return NULL;
}

createDynamicTilePtr Map::getDynamicTileFunction(const char* name, const char* type) const
{
	return NULL;
}

Material* Map::getDynamicTileMaterial(const char* name, const char* type) const
{
	return NULL;
}