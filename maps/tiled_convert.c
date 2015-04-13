#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "../libs/TmxParser/Tmx.h"

// You must install TinyXML (MacPorts: `port install tinyxml`, ArchLinux: `pacman -S tinyxml`).
// Then you must compile the TmxParser library as per instructions here:
//  https://code.google.com/p/tmx-parser/wiki/CompilingUnderLinux
// Then, link both `tinyxml` and `tmxparser` libs:
//  -ltinyxml -lz -ltmxparser


/* Method declarations */
/* ******************* */

bool DoesFileExist(const std::string& filename);
int ProcessTmxFile(const std::string& filename);

/* ******************* */


// There are faster "does file exist" methods out there:
// http://stackoverflow.com/questions/12774207/fastest-way-to-check-if-a-file-exist-using-standard-c-c11-c
// But we don't need speed: we need safety to ensure that the file exists and
// will be properly accessible when needed.
//
bool DoesFileExist(const std::string& filename)
{
	if (FILE* file = fopen(filename.c_str(), "r"))
	{
		fclose(file);
		return true;
	}

	return false;
}


int ProcessTmxFile(const std::string& filename)
{
	Tmx::Map* map = new Tmx::Map();
	if (map == 0)
	{
		return -1;
	}

	map->ParseFile(filename);
	if (map->HasError())
	{
		printf("error code: %d\n", map->GetErrorCode());
		printf("error text: %s\n", map->GetErrorText().c_str());

		return map->GetErrorCode();
	}
	
	std::string mapOrientation;
	switch (map->GetOrientation())
	{
		case Tmx::TMX_MO_ORTHOGONAL:
			mapOrientation = "orthogonal";
			break;
		case Tmx::TMX_MO_ISOMETRIC:
			mapOrientation = "isometric";
			break;
		default:
			mapOrientation = "staggered";
			break;
	}
	printf("<map version=\"%1.1f\" orientation=\"%s\"",
	        map->GetVersion(),
	        mapOrientation.c_str()
	      );

	// Iterate through the tilesets.
	for (int i = 0; i < map->GetNumTilesets(); ++i)
	{
		printf("                                    \n");
		printf("====================================\n");
		printf("Tileset : %02d\n", i);
		printf("====================================\n");

		// Get a tileset.
		const Tmx::Tileset *tileset = map->GetTileset(i);

		// Print tileset information.
		printf("Name: %s\n", tileset->GetName().c_str());
		printf("Margin: %d\n", tileset->GetMargin());
		printf("Spacing: %d\n", tileset->GetSpacing());
		printf("Image Width: %d\n", tileset->GetImage()->GetWidth());
		printf("Image Height: %d\n", tileset->GetImage()->GetHeight());
		printf("Image Source: %s\n", tileset->GetImage()->GetSource().c_str());
		printf("Transparent Color (hex): %s\n", tileset->GetImage()->GetTransparentColor().c_str());

		if (tileset->GetTiles().size() > 0)
		{
			// Get a tile from the tileset.
			const Tmx::Tile *tile = *(tileset->GetTiles().begin());

			// Print the properties of a tile.
			std::map< std::string, std::string > list = tile->GetProperties().GetList();
			std::map< std::string, std::string >::iterator iter;
			for (iter = list.begin(); iter != list.end(); ++iter)
			{
				printf("%s = %s\n", iter->first.c_str(), iter->second.c_str());
			}
		}
	}

	// Iterate through the layers.
	for (int i = 0; i < map->GetNumLayers(); ++i)
	{
		// Get a layer.
		const Tmx::Layer *layer = map->GetLayer(i);

		printf("                                    \n");
		printf("====================================\n");
		printf("Layer : %02d/%s (%dx%d)\n", i, map->GetLayer(i)->GetName().c_str(), layer->GetWidth(), layer->GetHeight());
		printf("====================================\n");

		for (int y = 0; y < layer->GetHeight(); ++y)
		{
			for (int x = 0; x < layer->GetWidth(); ++x)
			{
				// Get the tile's id.
				printf("%4d", layer->GetTileId(x, y));

				// Find a tileset for that id.
				const Tmx::Tileset *tileset = map->FindTileset(layer->GetTileId(x, y));

				printf(layer->IsTileFlippedHorizontally(x, y) ? "h" : "");
				printf(layer->IsTileFlippedVertically(x, y) ? "v" : "");
				printf(layer->IsTileFlippedDiagonally(x, y) ? "d" : "");
			}

			printf("\n");
		}
	}

	printf("\n\n");

	// Iterate through all of the object groups.
	for (int i = 0; i < map->GetNumObjectGroups(); ++i)
	{
		printf("                                    \n");
		printf("====================================\n");
		printf("Object group : %02d\n", i);
		printf("====================================\n");

		// Get an object group.
		const Tmx::ObjectGroup *objectGroup = map->GetObjectGroup(i);

		// Iterate through all objects in the object group.
		for (int j = 0; j < objectGroup->GetNumObjects(); ++j)
		{
			// Get an object.
			const Tmx::Object *object = objectGroup->GetObject(j);

			// Print information about the object.
			printf("Object:  ");
			printf("Size: (%3d, %3d) ", object->GetWidth(), object->GetHeight());
			printf("Position: (%4d, %4d) ", object->GetX(), object->GetY());
			printf("Name: %s\n", object->GetName().c_str());

			// Print Polygon points.
			const Tmx::Polygon *polygon = object->GetPolygon();
			if (polygon != 0)
			{
				for (int i = 0; i < polygon->GetNumPoints(); i++)
				{
					const Tmx::Point &point = polygon->GetPoint(i);
					printf("Object Polygon: Point %d: (%d, %d)\n", i, point.x, point.y);
				}
			}

			// Print Polyline points.
			const Tmx::Polyline *polyline = object->GetPolyline();
			if (polyline != 0)
			{
				for (int i = 0; i < polyline->GetNumPoints(); i++)
				{
					const Tmx::Point &point = polyline->GetPoint(i);
					printf("Object Polyline: Point %d: (%d, %d)\n", i, point.x, point.y);
				}
			}
		}
	}

	delete map;
	return 0;
}


// Read in an XML document. Specifically look for this data:
//    <layer name="..." width="::x::" height="::y::">
// Store the ::x:: width and ::y:: height values.
//
// Then, look for this data:
//    <tile gid="::tileid::"/>
// Replace ::tileid::
//
int main(int argc, char** argv)
{
	// Get a list of valid filenames from argument parameters
	std::vector<std::string> filenames;

	for (int i = 1; i < argc; i++)
	{
		if (DoesFileExist(argv[i]))
		{
			filenames.push_back(argv[i]);
		}
	}

	size_t numFiles = filenames.size();
	for (int i = 0; i < numFiles; i++)
	{
		printf("argv[%d]: %s\n", i, filenames[i].c_str());
		int result = ProcessTmxFile(filenames[i]);
		if (result != 0)
		{
			// Quit the app at this point for any non-0 result.
			return result;
		}
	}

	return 0;
}
