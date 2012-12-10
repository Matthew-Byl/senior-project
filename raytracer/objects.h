/**
 * Define objects for raytracer.
 */

#ifdef _HOST_
#define FLOAT3_T cl_float3
#define INT3_T cl_int3
#else
#define FLOAT3_T float3
#define INT3_T int3
#endif

typedef struct {
	float radius;
} Sphere;

typedef struct {
	float intensity;
} Light;

/* Object types */
#define SPHERE_TYPE 0
#define LIGHT_TYPE 1

typedef struct {
	INT3_T colour;
	int type;
	FLOAT3_T position;
	union {
		Sphere sphere;
		Light light;
	} objects;
} Object;

