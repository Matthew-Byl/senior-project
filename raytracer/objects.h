/**
 * Define objects for raytracer.
 *
 * @author John Kloosterman for CS352 at Calvin College
 * @date Dec. 12, 2012
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
	FLOAT3_T position;
} Light;

typedef struct {
	float radius;
} Sphere;

typedef struct {
	FLOAT3_T normal;
} Plane;

/* Object types */
#define SPHERE_TYPE 0
#define PLANE_TYPE 1
#define CUBE_TYPE 2

typedef struct {
    // if 4th component is 1, then the object is mirrored.
	UCHAR4_T colour;

	int type;
	FLOAT3_T position;
	union {
		Sphere sphere;
		Plane plane;
	} objects;
} Object;

