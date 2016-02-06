#ifndef  MAP_INC
#define  MAP_INC

#include "Updatable.h"
#include "Map/MapFiles.h"
#include "Map/StaticDatas.h"
#include "Map/ObjectDatas.h"
#include "expat.h"

/** \class Map 
 * \brief basic class for decompiling Map files
 * The xml format file is like this :
 * <map>
 *	<Window numberCase="axb" tileSize="axb" title="title"/>
 *
 *	<Files>
 *	 <Static file="path" spacing="axb" tileSize="axb">
 *	  <StaticTile name="name" type="type"/>
 *	 </Static>
 *	 <Dynamic file="path"> //Used for animation
 *	  <DynamicEntity name="name">
 *	   <DynamicTile name="name" pos="axb" size="axb" type="type"/>
 *	  </DynamicEntity>
 *	 </Dynamic>
 *	</Files>
 *
 *  <Objects name="name" numberCases = "axb" tileSize="axb" type="type">
 *   <ObjectsColumn fileID="fileID" tileID="tileID"/>
 *  </Objects>
 *
 *  <Traces>
 *   <StaticTrace name="name" shift="axb" size="axb">
 *    <Column fileID="fileID" tileID="tileID" objectID="objectID"/>
 *   </StaticTraces>
 *   <DynamicTrace name="name">
 *    <DynamicTile animName="name" animeTime="time" fileID="fileID" origin="axb" position="axb" tileID="tileID"/>
 *    <StaticTile fileID="fileID" tileID="tileID" position="axb"/>
 *   </DynamicTrace>
 *  </Traces>
 * </map> 
 *
 * each fileID et tileID are represented in csv format for StaticTrace. It earns us some time process and file spaces. This couple tells us which tile it is (which file and which tile in this file)*/
class Map : public Updatable, GroupTransformable
{
	public:
		/** \brief Constructor of the class Map. It creates and parse a XML file. The default position of the Map is (1.0, 1.0) with the size (nbCaseX * tileSizeX, nbCaseY * tileSizeY). Remember to scale the Map after its creation
		 * \param parent the parent of the Map
		 * \param file the XML file*/
		Map(Updatable* parent, File& file);

		/** \brief destructor of the class Map. It will destroy each traces, each texture used in this map*/
		~Map();

		void onUpdate(Render& render);

		/** \brief get static datas from name and type. This function is aimed to be overwrited.
		 * \param name the name of the tile contained on the xml file
		 * \param type the type of the tile contained on the xml file
		 * \return a StaticDatas. NULL if not correct*/
		virtual StaticDatas* getStaticDatas(const char* name, const char* type); 

		/** \brief get the tile on the coord x, y on OpenGL coords
		 * \param x the x component
		 * \param y the y component
		 * \return the Tile at this position. NULL if nothing*/
		Tile* getTile(int32_t x, int32_t y);

		/** \brief get the tile on the coord x, y on OpenGL coords in a specific trace
		 * \param x the x component
		 * \param y the y component
		 * \param traceName the trace name. We will check this position on this trace only.
		 * \return the Tile at this position. NULL if nothing (trace not found or position not found)*/
		Tile* getTile(int32_t x, int32_t y, const char* traceName);

		/** \brief tell if the position x, y is outside the Map. The Map Rect is (0, 0, nbCaseX*tileSizeX, nbCaseY*tileSizeY)
		 * \param x the x coords
		 * \param y the y coords
		 * \return tells if the coords are outsize the Map or not.*/
		bool isOutside(int32_t x, int32_t y);
	private:
		XML_Parser parser;
		std::vector<Texture*> m_textures;
		uint32_t m_nbCasesX;
		uint32_t m_nbCasesY;
		uint32_t m_caseSizeX;
		uint32_t m_caseSizeY;
};

void startElement(void* map, const char* name, const char** attrs);
void startElementFiles(void* map, const char* name, const char** attrs);
void startElementObjects(void* map, const char* name, const char** attrs);
void startElementTraces(void* map, const char* name, const char** attrs);
void endElement(void* map, const char* name);

extern uint32_t XML_depth;
extern uint32_t XML_NthColumn;

#endif
