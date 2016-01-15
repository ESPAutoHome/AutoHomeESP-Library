#ifndef OTAUpdate_H
#define OTAUpdate_H

#include "../AutoHome.h"

class OTAUpdate {

public:
	OTAUpdate();
	~OTAUpdate();
	void begin(char const* host);
};

#endif
