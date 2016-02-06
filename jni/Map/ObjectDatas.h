#ifndef  OBJECTDATAS_INC
#define  OBJECTDATAS_INC

/** \struct ObjectDatas
 * \brief object datas about objects. In fact, it keeps CSV string about tiles and files (where to look for creating tiles which will create the Object), and the Object characteristics, such as tileSize and nbCases*/
struct ObjectDatas
{
	Object*(*createObject)(uint32_t, uint32_t, uint32_t, uint32_t);
	std::vector<CSVParser*> CSVTileID;
	std::vector<CSVParser*> CSVFileID;
	uint32_t nbCasesX;
	uint32_t nbCasesY;
	uint32_t tileSizeX;
	uint32_t tileSizeY;
};

#endif
