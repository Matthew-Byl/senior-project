/**
 * Define objects for raytracer.
 */

#ifdef _HOST_
#define FLOAT3_T cl_float3
#define UCHAR3_T cl_uchar3
#define UCHAR4_T cl_uchar4
#define INT3_T cl_int3
#else
#define FLOAT3_T float3
#define UCHAR3_T uchar3
#define UCHAR4_T uchar4
#define INT3_T int3
#endif

typedef struct {
	float radius;
} Sphere;

typedef struct {
	FLOAT3_T position;
//	float intensity;
} Light;

typedef struct {
	FLOAT3_T normal;
} Plane;

/* Object types */
#define SPHERE_TYPE 0
#define PLANE_TYPE 1
#define CUBE_TYPE 2

typedef struct {
	UCHAR4_T colour; // if 4th component is 1, then mirrored.
	int type;
	FLOAT3_T position;
	union {
		Sphere sphere;
		Plane plane;
	} objects;
} Object;

