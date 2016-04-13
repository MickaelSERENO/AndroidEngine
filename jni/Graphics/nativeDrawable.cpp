#include "nativeDrawable.h"

JNIEXPORT void JNICALL Java_com_gaulois94_Graphics_Drawable_addShaderDrawable(JNIEnv *jenv, jclass jcls, jstring key, jlong shader)
{
    const char *k = jenv->GetStringUTFChars(key, 0);
	Shader::shaders.add(std::string(k), (Shader*) shader);
	jenv->ReleaseStringUTFChars(key, k);
}

JNIEXPORT jlong JNICALL Java_com_gaulois94_Graphics_Drawable_getShaderDrawable(JNIEnv *jenv, jclass jcls, jstring key)
{
    const char *k = jenv->GetStringUTFChars(key, 0);
	Shader* shader = Shader::shaders.get(std::string(k));
	jenv->ReleaseStringUTFChars(key, k);

	return (jlong)shader;
}

JNIEXPORT jstring JNICALL Java_com_gaulois94_Graphics_Drawable_getKeyShaderDrawable(JNIEnv *jenv, jclass jcls, jlong shader)
{
	std::string key = Shader::shaders.get((Shader*) shader);
	jstring result  = jenv->NewStringUTF(key.c_str());
	return result;
}

JNIEXPORT jint JNICALL Java_com_gaulois94_Graphics_Drawable_removeShaderFromKeyDrawable(JNIEnv *jenv, jclass jcls, jstring key)
{
    const char *k = jenv->GetStringUTFChars(key, 0);
	int result = Shader::shaders.remove(std::string(k));
	jenv->ReleaseStringUTFChars(key, k);

	return result;
}

JNIEXPORT jint JNICALL Java_com_gaulois94_Graphics_Drawable_removeShaderFromShaderDrawable(JNIEnv *jenv, jclass jcls, jlong shader)
{
	Shader* s = (Shader*)shader;
	int result = Shader::shaders.remove(s);
	return result;
}

JNIEXPORT void JNICALL Java_com_gaulois94_Graphics_Drawable_cleanShaders(JNIEnv *jenv, jclass jcls)
{
	Shader::shaders.clean();
}

JNIEXPORT jint JNICALL Java_com_gaulois94_Graphics_Drawable_getNumberOfShaders(JNIEnv *jenv, jclass jcls)
{
	return Shader::shaders.getSize();
}

JNIEXPORT jint JNICALL Java_com_gaulois94_Graphics_Drawable_existShaderFromKeyDrawable(JNIEnv *jenv, jclass jcls, jstring key)
{
    const char *k = jenv->GetStringUTFChars(key, 0);
	int result = Shader::shaders.exist(std::string(k));
	jenv->ReleaseStringUTFChars(key, k);

	return result;
}

JNIEXPORT void JNICALL Java_com_gaulois94_Graphics_Drawable_loadShadersDrawable(JNIEnv *jenv, jclass jcls, jobject context)
{
	std::string files[]          = {"basic", "color", "obj", "texture"};
	int size                     = 4;

	for(int i = 0; i < size; i++)
	{
		std::string pathfString = "shaders/" + files[i] + ".frag";
		std::string pathvString = "shaders/" + files[i] + ".vert";

		File vertexFile        = File(jenv, context, pathvString.c_str(), "r");
		File fragFile          = File(jenv, context, pathfString.c_str(), "r");
		Shader::shaders.add(files[i], Shader::loadFromFiles(vertexFile, fragFile));
	}
}

JNIEXPORT void JNICALL Java_com_gaulois94_Graphics_Drawable_drawDrawable(JNIEnv *jenv, jobject jobj, jlong ptr, jlong render, jfloatArray transformation)
{
	Drawable* drawable = (Drawable*)ptr;
	jfloat* ft = jenv->GetFloatArrayElements(transformation, 0);
	glm::mat4 t = glm::make_mat4(ft);
	drawable->draw(*(Render*)render, t);
	jenv->ReleaseFloatArrayElements(transformation, ft, 0);
}

JNIEXPORT void JNICALL Java_com_gaulois94_Graphics_Drawable_onDrawDrawable(JNIEnv *jenv, jobject jobj, jlong ptr, jlong render, jfloatArray transformation)
{
	Drawable* drawable = (Drawable*)ptr;
	jfloat* ft = jenv->GetFloatArrayElements(transformation, 0);
	glm::mat4 t = glm::make_mat4(ft);
	drawable->onDraw(*(Render*)render, t);
	jenv->ReleaseFloatArrayElements(transformation, ft, 0);
}

JNIEXPORT void JNICALL Java_com_gaulois94_Graphics_Drawable_onMoveDrawable(JNIEnv *jenv, jobject jobj, jlong ptr, jfloatArray move, bool useScale)
{
	Drawable* drawable = (Drawable*)ptr;
	jfloat* m = jenv->GetFloatArrayElements(move, 0);
	drawable->onMove(glm::vec3(m[0], m[1], m[2]), useScale);
	jenv->ReleaseFloatArrayElements(move, m, 0);
}

JNIEXPORT void    JNICALL Java_com_gaulois94_Graphics_Drawable_onRotateDrawable(JNIEnv *jenv, jobject jobj, jlong ptr, jfloat angle, jfloatArray axis, jfloatArray origin)
{
	Drawable* drawable = (Drawable*)ptr;
	jfloat* a = jenv->GetFloatArrayElements(axis, 0);
	jfloat* o = jenv->GetFloatArrayElements(origin, 0);
	drawable->onRotate(angle, glm::vec3(a[0], a[1], a[2]), glm::vec3(o[0], o[1], o[2]));
	jenv->ReleaseFloatArrayElements(axis, a, 0);
	jenv->ReleaseFloatArrayElements(origin, o, 0);
}

JNIEXPORT void    JNICALL Java_com_gaulois94_Graphics_Drawable_onScaleDrawable(JNIEnv *jenv, jobject jobj, jlong ptr, jfloatArray scale)
{
	Drawable* drawable = (Drawable*)ptr;
	jfloat* s = jenv->GetFloatArrayElements(scale, 0);
	drawable->onScale(glm::vec3(s[0], s[1], s[2]));
	jenv->ReleaseFloatArrayElements(scale, s, 0);
}

JNIEXPORT void    JNICALL Java_com_gaulois94_Graphics_Drawable_staticToCameraDrawable(JNIEnv *jenv, jclass jcls, jlong ptr, jint s)
{
	Drawable* drawable = (Drawable*) ptr;
	drawable->staticToCamera(s);	
}

JNIEXPORT jint    JNICALL Java_com_gaulois94_Graphics_Drawable_isStaticToCameraDrawable(JNIEnv *jenv, jclass jcls, jlong ptr)
{
	Drawable* drawable = (Drawable*) ptr;
	return drawable->isStaticToCamera();
}

JNIEXPORT void    JNICALL Java_com_gaulois94_Graphics_Drawable_setMaterialDrawable(JNIEnv *jenv, jclass jcls, jlong ptr, jlong materialPtr)
{
	Drawable* drawable = (Drawable*) ptr;
	drawable->setMaterial((Material*)materialPtr);
}

JNIEXPORT jlong   JNICALL Java_com_gaulois94_Graphics_Drawable_getMaterialDrawable(JNIEnv *jenv, jclass jcls, jlong ptr)
{
	Drawable* drawable = (Drawable*) ptr;
	return (long)(drawable->getMaterial());
}
