#include "nativeTexture.h"

JNIEXPORT jlong JNICALL Java_com_gaulois94_Graphics_Texture_initTexture(JNIEnv* jenv, jclass jcls, jlong width, jlong height)
{
	Texture* texture = new Texture(width, height);
	return (jlong)texture;
}

JNIEXPORT jlong JNICALL Java_com_gaulois94_Graphics_Texture_loadFromPixelsTexture(JNIEnv* jenv, jclass jcls, jbyteArray pixels, jlong width, jlong height, jboolean alpha)
{
	unsigned char* p = (unsigned char*)(jenv->GetByteArrayElements(pixels, 0));
	Texture* texture = new Texture(p, width, height, alpha);
	jenv->ReleaseByteArrayElements(pixels, (jbyte*)p, 0);
	return (jlong)texture;
}

JNIEXPORT jlong JNICALL Java_com_gaulois94_Graphics_Texture_loadFromBitmapTexture(JNIEnv* jenv, jclass jcls, jlong bmp)
{
	Texture* texture = new Texture((Bitmap*) bmp);
	return (jlong)texture;
}

JNIEXPORT jlong JNICALL Java_com_gaulois94_Graphics_Texture_loadFromFileTexture(JNIEnv* jenv, jclass jcls, jstring path)
{
	//Load the android Bitmap
	//Function : Bitmap bmp = BitmapFactory.decodeStream(JniMadeOf::jassetsManager.open(path))
	jclass BitmapFactory   = jenv->FindClass("android/graphics/BitmapFactory");
	jmethodID decodeStream = jenv->GetStaticMethodID(BitmapFactory, "decodeStream", "(Ljava/io/InputStream;)Landroid/graphics/Bitmap;");
	jclass AssetManager    = jenv->FindClass("android/content/res/AssetManager");
	jmethodID open         = jenv->GetMethodID(AssetManager, "open", "(Ljava/lang/String;)Ljava/io/InputStream;");
	jobject jstream        = jenv->CallObjectMethod(JniMadeOf::jassetsManager, open, path);
	jobject jbmp           = jenv->CallStaticObjectMethod(BitmapFactory, decodeStream, jstream);
	
	//Load the texture
	Bitmap*	bmp = (Bitmap*) Java_com_gaulois94_Graphics_Bitmap_createBitmap(jenv, BitmapFactory, jbmp);
	Texture* texture = new Texture(bmp);

	delete bmp;
	return (jlong)texture;	
}

JNIEXPORT jlong JNICALL Java_com_gaulois94_Graphics_Texture_getIDTexture(JNIEnv* jenv, jobject jobj, jlong texture)
{
	Texture* ptr = (Texture*) texture;
	return (jlong) ptr->getID();
}

JNIEXPORT jlong JNICALL Java_com_gaulois94_Graphics_Texture_getWidthTexture(JNIEnv* jenv, jobject jobj, jlong texture)
{
	Texture* ptr = (Texture*) texture;
	return ptr->getWidth();
}

JNIEXPORT jlong JNICALL Java_com_gaulois94_Graphics_Texture_getHeightTexture(JNIEnv* jenv, jobject jobj, jlong texture)
{
	Texture* ptr = (Texture*) texture;
	return ptr->getHeight();
}
