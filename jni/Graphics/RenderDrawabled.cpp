#include "RenderDrawabled.h"

RenderDrawabled::RenderDrawabled(Updatable* parent, Material* mtl, const Vector2f& pixelsSize) : Updatable(parent), m_render(NULL, pixelsSize), m_sprite(this, mtl, m_render)
{
}

void RenderDrawabled::setMaterial(Material* mtl)
{
	m_sprite.setMaterial(mtl);
}

RenderTexture& RenderDrawabled::getRenderTexture()
{
	return m_sprite.m_render;
}

Sprite& RenderDrawabled::getSprite()
{
	return m_sprite;
}

RenderDrawabled::RenderSprite::RenderSprite(Updatable* parent, Material* mtl, RenderTexture& render) :
	Sprite(parent, mtl, render.getTexture()), m_render(render)
{
	render.setParent(this);
}

void RenderDrawabled::RenderSprite::updateFocus(const TouchEvent& te, Render& render, const glm::mat4& mvp)
{
	LOG_ERROR("UPDATE FOCUS !!");
	glm::mat4 m = glm::scale(mvp, glm::vec3(0.5, 0.5, 1.0));
	m    	    = glm::translate(m, glm::vec3(1.0, 1.0, 0.0));
	Updatable::updateFocus(te, render, m*getMatrix());
	LOG_ERROR("UPDATE FOCUS 2 !!");
}