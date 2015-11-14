LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES += $(NDK_APP_PROJECT_PATH)/jni/Graphics/
LOCAL_MODULE := engine
LOCAL_CFLAGS := -Wall --std=c++11
APP_OPTIM := debug

LOCAL_SRC_FILES := File.cpp JniMadeOf.cpp Updatable.cpp nativeUpdatable.cpp nativeJniMadeOf.cpp utils.cpp EventManager.cpp nativeEventManager.cpp

LOCAL_SRC_FILES += Graphics/nativeBitmap.cpp Graphics/Bitmap.cpp Graphics/nativeRenderer.cpp Graphics/Renderer.cpp Graphics/nativeTexture.cpp Graphics/Texture.cpp Graphics/nativeTransformable.cpp Graphics/Transformable.cpp Graphics/nativeShader.cpp Graphics/Shader.cpp Graphics/nativeFont.cpp Graphics/Font.cpp Graphics/nativeText.cpp Graphics/Text.cpp Graphics/nativeDrawable.cpp Graphics/Drawable.cpp Graphics/nativeCamera.cpp Graphics/Camera.cpp Graphics/Color.cpp Graphics/Sprite.cpp Graphics/nativeSprite.cpp Graphics/OBJWrapper.cpp Graphics/GroupDrawable.cpp Graphics/Render.cpp Graphics/nativeRender.cpp

LOCAL_SRC_FILES += Graphics/Shape/TriangleShape.cpp Graphics/Shape/nativeTriangleShape.cpp Graphics/Shape/Circle.cpp Graphics/Shape/nativeCircle.cpp Graphics/Shape/Rectangle.cpp Graphics/Shape/Sphere.cpp Graphics/Shape/LateralCylinder.cpp Graphics/Shape/Cylinder.cpp

LOCAL_SRC_FILES += Graphics/Effect/Explosion.cpp Graphics/Effect/Effect.cpp

LOCAL_SRC_FILES += Graphics/Materials/Material.cpp Graphics/Materials/ColorMaterial.cpp Graphics/Materials/UniColorMaterial.cpp Graphics/Materials/nativeMaterial.cpp Graphics/Materials/nativeUniColorMaterial.cpp Graphics/Materials/ObjMaterial.cpp Graphics/MaterialWrapper.cpp Graphics/Materials/EffectMaterial.cpp Graphics/Materials/ExplosionMaterial.cpp Graphics/Materials/TextureMaterial.cpp

LOCAL_SRC_FILES += Graphics/Animation.cpp Graphics/nativeAnimation.cpp Graphics/PatternAnimation.cpp Graphics/nativePatternAnimation.cpp Graphics/SelectAnimation.cpp Graphics/nativeSelectAnimation.cpp

LOCAL_SRC_FILES += Graphics/Widgets/Button.cpp Graphics/Widgets/CheckBox.cpp Graphics/Widgets/Active.cpp

LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2 -ljnigraphics -latomic
include $(BUILD_SHARED_LIBRARY)

ZPATH := $(LOCAL_PATH)
