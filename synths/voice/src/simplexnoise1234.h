#pragma warning(push, 0)
#include <DSP/MLDSPOps.h>
#pragma warning(pop)

extern float snoise1( float x );
extern float snoise2( float x, float y );
extern float snoise3( float x, float y, float z );
extern float snoise4( float x, float y, float z, float w );


extern ml::DSPVector snoise1(const ml::DSPVector& x);