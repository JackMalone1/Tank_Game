#pragma once
#include <SFML/Graphics.hpp>
//used to figure out what type of object is currently selected
enum class ObjectSelected
{
	Wall = 1,
	Target = 0
};

class Object
{
public:
	Object();
	void update(sf::Vector2f t_position, float t_deltaRotation);//updates position and rotatiobn
	void setTexture(sf::Texture& t_texture) { m_sprite.setTexture(t_texture); }
	void setTextureRect(sf::IntRect t_intRect) { m_sprite.setTextureRect(t_intRect); }
	void setOrigin(sf::Vector2f t_pos) { m_sprite.setOrigin(t_pos); }
	void setPosition(sf::Vector2f t_pos) { m_sprite.setPosition(t_pos); }
	void setRotation(float t_rot) { m_sprite.setRotation(t_rot); }
	sf::Sprite getSprite()  { return m_sprite; }
	void setType(ObjectSelected t_type) { m_objectSelected = t_type; }
	bool operator<(Object rhs);//used to be able to sort the array of objects in the level creator
	ObjectSelected returnType() { return m_objectSelected; }
private:
	sf::Sprite m_sprite;//used to represent the object
	float m_rotation;
	ObjectSelected m_objectSelected{ ObjectSelected::Wall };//defaults to a wall if no type specified
};

