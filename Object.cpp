#include "Object.h"

Object::Object()
{
}

/// <summary>
/// updates the position and rotation of the object depending on what the user inputs when they are using the level creator
/// </summary>
/// <param name="t_position"></param>
/// <param name="t_deltaRotation"></param>
void Object::update(sf::Vector2f t_position, float t_deltaRotation)
{
	m_sprite.setPosition(t_position);
	m_sprite.rotate(t_deltaRotation);
}

/// <summary>
/// uses what type is selected to sort the objects in an array
/// wall is set to 1 and target is set to 0 so targets will be sorted before walls
/// </summary>
/// <param name="rhs"></param>
/// <returns></returns>
bool Object::operator<(Object rhs)
{
	return this->m_objectSelected < rhs.m_objectSelected;
}
