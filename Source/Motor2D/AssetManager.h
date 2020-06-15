#ifndef __j1ASSETMANAGER_H__
#define __j1ASSETMANAGER_H__

#include "Module.h"

struct SDL_RWops;

class AssetManager : public Module
{
public:

	AssetManager();
	virtual ~AssetManager();

	bool Awake(pugi::xml_node&);

	bool Exists(const char* file) const;

	uint LoadData(const char* file, char** buffer) const;
	SDL_RWops* Load(const char* file) const;

	pugi::xml_parse_result LoadXML(pugi::xml_document &data_file, const char* path);

private:

	bool CreatePath(const char* newDir, const char* mount_point = nullptr);
};

#endif // __j1FILESYSTEM_H__