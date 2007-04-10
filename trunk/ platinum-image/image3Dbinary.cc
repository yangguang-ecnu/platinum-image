

//

// Använd en 1 bitsvolym till binära data.

// n=1 maskas av en 8/(8/n) stor volym

//


#include "volumehandler_base.h"


// Denna är INTE en template - finns bara en binärtyp - det speciella är avkodningen

class volumehandler3Dbinary : public volumehandler_base // We MUST have a virtual base class

{

private:

 // vi vill använda 1 bit per voxel, alltså dela längd med 8

 unsigned char *volumeptr; // we will get pointer from ITK? Or internally?

 int width, height, depth; // volume size

// ??? int offsetX, offsetY, offsetZ;  // (behövs även offsets? Beror på hur ITK lagrar volymsdata) - En volym kanske inte täcker hela av en annan volym


public:

 void initialize_dataset(unsigned char *ptr, int w, int h, int d); // load must happen outside class - only part of the wholeness

// void raytrace(vector3D direction, int threshold, vector3D &resultpos, vector3D startpos = 0); // mostly a demo app.

};

