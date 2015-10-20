#include "Active.h"

Active::Active(bool select, bool active, bool alwaysUpdateSelection, bool alwaysUpdateActivation) : m_isSelect(select), m_isActive(active), m_alwaysUpdateSelection(alwaysUpdateSelection), m_alwaysUpdateActivation(alwaysUpdateActivation), m_permanentSelected(false), m_permanentActivated(false), m_selectOnce(true), m_activeOnce(true), m_activeFunc(0)
{}

void Active::update()
{
	if(m_alwaysUpdateSelection)
		updateSelection();
	if(m_alwaysUpdateActivation)
		updateActivation();
}

bool Active::updateSelection()
{
	if((m_isSelect && m_selectOnce) && (howSelect() || m_permanentSelected))
		selectIt();
	else if(howDeselect())
		deselectIt();

	return m_isSelect;
}

bool Active::updateActivation()
{
	if(howActive() || m_permanentActivated)
		activeIt();
	else if(howDisactive() || (m_isActive && m_activeOnce))
		disactiveIt();

	return m_isActive;
}

bool Active::howActive()
{
	return false;
}

bool Active::howDisactive()
{
	return false;
}

bool Active::howSelect()
{
	return false;
}

bool Active::howDeselect()
{
	return false;
}

void Active::selectIt()
{
	m_isSelect = true;
}

void Active::deselectIt()
{
	m_isSelect = false;
}

void Active::activeIt()
{
	LOG_ERROR("ACTIVED !");
	m_isActive = true;
	if(m_activeFunc)
		m_activeFunc();
}

void Active::disactiveIt()
{
	LOG_ERROR("DISACTIVED !");
	m_isActive = false;
}

void Active::setPermanentSelected(bool permanentSelected)
{
	m_permanentSelected = permanentSelected;
	updateSelection();
}

void Active::setPermanentActivated(bool permanentActivated)
{
	m_permanentActivated = permanentActivated;
	updateActivation();
}

void Active::setAlwaysUpdateSelection(bool alwaysUpdateSelection)
{
	m_alwaysUpdateSelection = alwaysUpdateSelection;
}

void Active::setAlwaysUpdateActivation(bool alwaysUpdateActivation)
{
	m_alwaysUpdateActivation = alwaysUpdateActivation;
}

void Active::setSelectOnce(bool selectOnce)
{
	m_selectOnce = selectOnce;
}

void Active::setActiveOnce(bool activeOnce)
{
	m_activeOnce = activeOnce;
}

void Active::setActiveFunc(void(*f)(void))
{
	m_activeFunc = f;
}

bool Active::isPermanentSelected() const
{
	return m_permanentSelected;
}

bool Active::isPermanentActivated() const
{
	return m_permanentActivated;
}

bool Active::isSelect() const
{
	return m_isSelect;
}

bool Active::isActive() const
{
	return m_isActive;
}

bool Active::isAlwaysUpdateSelection() const
{
	return m_alwaysUpdateSelection;
}

bool Active::isAlwaysUpdateActivation() const
{
	return m_alwaysUpdateActivation;
}

bool Active::isSelectOnce() const
{
	return m_selectOnce;
}

bool Active::isActiveOnce() const
{
	return m_activeOnce;
}
