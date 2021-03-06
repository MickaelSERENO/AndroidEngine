#ifndef NATIVECIRCLE_INCLUDE
#define NATIVECIRCLE_INCLUDE

#define GLM_FORCE_RADIANS

#include <jni.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shape/Circle.h"
#include "Materials/Material.h"

extern "C" {
	JNIEXPORT jlong       JNICALL Java_com_gaulois94_Graphics_Shape_Circle_createCircle(JNIEnv *, jclass, jlong, jlong, jfloat, jint);
	JNIEXPORT void        JNICALL Java_com_gaulois94_Graphics_Shape_Circle_setNbEdgeCircle(JNIEnv *, jobject, jlong, jint, jfloat);
	JNIEXPORT jfloatArray JNICALL Java_com_gaulois94_Graphics_Shape_Circle_getCenterCircle(JNIEnv *jenv, jobject jobj, jlong ptr);
	JNIEXPORT int         JNICALL Java_com_gaulois94_Graphics_Shape_Circle_getNbEdgeCircle(JNIEnv *, jobject, jlong);
}
#endif

