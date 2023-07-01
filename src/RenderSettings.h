#pragma once

struct RenderSettings {
	int size[2] = {1024, 1024};
	int characterSize[2] = { 48 * 2, 48 * 2 };
	int selectedOption = 0;
	int charactersPerRow = 1;
	int numOfColumns = 1;
};