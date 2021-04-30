#include "assetloader.h"

#include <stdlib.h>

ren_bitmap_t* asset_spritesheet = NULL;
ren_bitmap_t* asset_fontsheet = NULL;

__attribute__((constructor)) void assetloader_setup(void)
{
	log_debug("Asset loading begin...");
	asset_spritesheet = ren_load_bitmap("bin/asset.png");
	asset_fontsheet = ren_load_bitmap("bin/font.png");
	log_debug("Assets loaded successful!");
}

__attribute__((destructor)) void assetloader_cleanup(void)
{
	ren_free_bitmap(asset_spritesheet);
	ren_free_bitmap(asset_fontsheet);
	log_debug("Assets freed!");
}

