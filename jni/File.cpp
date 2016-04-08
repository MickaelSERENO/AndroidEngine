#include "File.h"

File::File(FILE* file, int size, int offset) : m_file(file), m_size(size), m_offset(0), m_pos(0)
{}

File::File(JNIEnv* env, jobject context, int id, const char* mode)
{
	m_pos  = 0;
	//Get the file descriptor
	jclass contextClass      = env->GetObjectClass(context);
	jclass fdClass           = env->FindClass("android/content/res/AssetFileDescriptor");

	jmethodID resourceMethod = env->GetMethodID(contextClass, "getResources", "()Landroid/content/res/Resources;");

	jobject resource         = env->CallObjectMethod(context, resourceMethod);
	jclass resourceClass     = env->GetObjectClass(resource);

	jmethodID openRaw        = env->GetMethodID(resourceClass, "openRawResourceFd", "(I)Landroid/content/res/AssetFileDescriptor;");
	jobject fdSys            = env->CallObjectMethod(resource, openRaw, id);

	if(fdClass != NULL && fdSys != NULL)
	{
		jmethodID createParcel = env->GetMethodID(fdClass, "getParcelFileDescriptor", "()Landroid/os/ParcelFileDescriptor;");
		jobject parcel         = env->CallObjectMethod(fdSys, createParcel);
		jclass parcelCls       = env->GetObjectClass(parcel);

		jmethodID fdFieldID    = env->GetMethodID(parcelCls, "detachFd", "()I");

		int fd = env->CallIntMethod(parcel, fdFieldID);

		jmethodID getLength    = env->GetMethodID(fdClass, "getLength", "()J");
		jmethodID getOffset    = env->GetMethodID(fdClass, "getStartOffset", "()J");

		m_size                 = (int) env->CallLongMethod(fdSys, getLength);
		m_offset               = (int) env->CallLongMethod(fdSys, getOffset);
		lseek(fd, m_offset, SEEK_SET);

		m_file = fdopen(fd, mode);
	}
	else
	{
		m_file = NULL;
		m_size = 0;
	}
}

File::File(const char* path, const char* mode)
{

}

File::File(JNIEnv* env, jobject context, const char* path, const char* mode)
{
	m_pos  = 0;
	m_path = std::string(path);
	//Get the file descriptor
	jclass contextClass      = env->GetObjectClass(context);
	jclass fdClass           = env->FindClass("android/content/res/AssetFileDescriptor");

	jmethodID getAssets      = env->GetMethodID(contextClass, "getAssets", "()Landroid/content/res/AssetManager;");
	jobject assets           = env->CallObjectMethod(context, getAssets);
	jclass assetsClass       = env->GetObjectClass(assets);

	jmethodID openFd         = env->GetMethodID(assetsClass, "openFd", "(Ljava/lang/String;)Landroid/content/res/AssetFileDescriptor;");
	
	jobject jpath            = env->NewStringUTF(path);
	jobject fdSys            = env->CallObjectMethod(assets, openFd, jpath);

	if(fdSys != NULL)
	{
		jmethodID createParcel = env->GetMethodID(fdClass, "getParcelFileDescriptor", "()Landroid/os/ParcelFileDescriptor;");
		jobject parcel         = env->CallObjectMethod(fdSys, createParcel);
		jclass parcelCls       = env->GetObjectClass(parcel);

		jmethodID fdFieldID    = env->GetMethodID(parcelCls, "detachFd", "()I");

		int fd = env->CallIntMethod(parcel, fdFieldID);

		jmethodID getLength    = env->GetMethodID(fdClass, "getLength", "()J");
		jmethodID getOffset    = env->GetMethodID(fdClass, "getStartOffset", "()J");

		m_size                 = (int) env->CallLongMethod(fdSys, getLength);
		m_offset               = (int) env->CallLongMethod(fdSys, getOffset);
		lseek(fd, m_offset, SEEK_SET);

		m_file = fdopen(fd, mode);
	}
	else
	{
		m_file = NULL;
		m_size = 0;
	}
}

File::~File()
{
	if(m_file != NULL)
		fclose(m_file);
}

char* File::readLine()
{
	if(m_pos >= m_size)
		return NULL;

	char* line = (char*) malloc(FILE_BUFFER_SIZE*sizeof(char));
	char c = '\0';
	int i=0;
	
	while(c != '\n' && m_pos < m_size && i < FILE_BUFFER_SIZE-1)
	{
		c = fgetc(m_file);
		*(line + i)=c;
		i++;
		m_pos++;
	}
	
	*(line + i) = '\0';
	return line;
}

char File::readChar()
{
	if(m_pos >= m_size)
		return '\0';
	m_pos++;
	return fgetc(m_file);
}

const std::string& File::getPath() const
{
	return m_path;
}
