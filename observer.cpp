#include <iostream>
#include <list>
#include <map>
#include <vector>

// Maybe i dont needs this now...... could just file it away though :)
// This should return the floor that is the least *which means closest*
/*template <class T>
T Change(T Control, T x1, T x2) {
    T temp1, temp2;
    temp1 = Control-x1;
    temp2 = Control-x2;
    return (temp1 > temp2 ? temp1 : temp2);
}*/


// CLASS ENTITY
class Entity {
public:
	virtual ~Entity();
	virtual float GetX();
	virtual float GetY();
	virtual float GetRad();
	virtual void Draw()=0;
	bool CheckCollision();
protected:
	bool isAlive;
	float x, y, r;
};

Entity::~Entity() {}

float Entity::GetX() {
	return x;
}

float Entity::GetY() {
	return y;
}

float Entity::GetRad() {
	return r;
}

class Subject;

//CLASS OBSERVER
class Observer {
public:
	virtual ~Observer();
	virtual void Update(Subject*) = 0;
protected:
	Observer();
};

Observer::Observer() {}

Observer::~Observer() {}


// CLASS SUBJECT
class Subject {
public:
	virtual ~Subject();

	void Attach(Observer*);
	void Detach(Observer*);
protected:
	Subject();
	virtual void Notify() = 0;
	std::list<Observer*> _observers;
};

Subject::Subject() {}

Subject::~Subject() {}

void Subject::Attach(Observer* o) {
	_observers.push_back(o);
}

void Subject::Detach(Observer* o) {
	_observers.remove(o);
}


class Collider;

// CLASS ICOLLIDE
class ICollide : public Subject {
public:
	virtual ~ICollide();
	Collider* FindNearest();
	bool TestRadii(Collider*, Collider*);
	bool TestAABB(Collider*, Collider*);
	void ConsiderType(Collider*&);
protected:
	virtual void Notify();
	std::vector<Collider*> potentialsVec;
	// Map will store Collider entity + Position Vector (For now, just hold X or something)
	// IDEA: Should it hold type?
	//std::map<Collider*, Vectorf> collidersMap;
	std::map<Collider*, Collider::CollEnum> collidersMap;
};


// CLASS COLLIDER
class Collider : public Observer, public Entity {
public:
	enum CollEnum {player, player_projectile, enemy_projectile, enemy};
	CollEnum Type_C;

public:
	virtual ~Collider();
	virtual bool CheckCollision();
	virtual void Update(Subject*);
	CollEnum GetType() const;

protected:
	Collider();
	ICollide* _collide;
};

Collider::Collider() {}

Collider::~Collider() {}

Collider::CollEnum Collider::GetType() const {return this->Type_C;};

bool Collider::CheckCollision() {
	if (!(_collide->TestRadii(this, _collide->FindNearest())))
		//_collide->TestAABB(this);
		return false;
	return true;
}

void Collider::Update(Subject* ChangedSubject) {
	if (ChangedSubject != _collide) {
		_collide = static_cast<ICollide*>(ChangedSubject);
	}
}

// CONTINUE ICOLLIDE
ICollide::~ICollide() {}

Collider* ICollide::FindNearest() {
    Collider* currentC, *nextC;

	std::list<Observer*>::iterator it;
	for (it = _observers.begin(); it != _observers.end(); ++it) {
		if (it == _observers.begin()) {
        	currentC = reinterpret_cast<Collider*>(*it);
			ConsiderType(currentC);
			continue;
		}
        nextC = reinterpret_cast<Collider*>(*it);
	}
	return currentC;
}

void ICollide::ConsiderType(Collider* &c) {
	// Gotta clear potentialsVec on every consider...
	potentialsVec.clear();
	Collider* currentC;
	std::list<Observer*>::iterator it;
	for (it = _observers.begin(); it != _observers.end(); ++it) {
    	currentC = reinterpret_cast<Collider*>(*it);

    	switch(c->GetType()) {
    	case Collider::player:
    		// Collides with Comets
            if (currentC->GetType() == Collider::enemy ||
                            currentC->GetType() == Collider::enemy_projectile) {
            		potentialsVec.push_back(currentC);
            }
        	break;
    	case Collider::enemy:
    		// Collides with player, player_proj
    		if (currentC->GetType() == Collider::player ||
                            currentC->GetType() == Collider::player_projectile) {
            		potentialsVec.push_back(currentC);
            }
        	break;
    	case Collider::player_projectile:
    		// Collides with enemy
    		if (currentC->GetType() == Collider::enemy) {
            		potentialsVec.push_back(currentC);
            }
        	break;
    
    	}
	}
}



bool ICollide::TestRadii(Collider* c1, Collider* c2) {
	const float TOUCH_DISTANCE = 0.00001;
	float x1 = c1->GetX(), y1 = c1->GetY(), r1 = c1->GetRad();
	float x2 = c2->GetX(), y2 = c1->GetY(), r2 = c1->GetRad();
	float distance_squared, radii_squared;

	//a^2 + b^2 = c^2
	distance_squared = ((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));

	// Multiply rather than sqrt, faster
	radii_squared = (r1 + r2)*(r1 + r2);

	// Finally, conduct tests
	if (-TOUCH_DISTANCE < radii_squared - distance_squared && radii_squared - distance_squared < TOUCH_DISTANCE) {
		// Touching
		return true;
	} else if (radii_squared > distance_squared) {
		// Overlapping
		return true;
	} else {
		// Nothing
		return false;
	}
}

void ICollide::Notify() {
	std::list<Observer*>::iterator it;
	for (it = _observers.begin(); it != _observers.end(); ++it) {
    	Observer* o = *it;
		o->Update(this);
	}
}
/*
bool ICollide::TestRadii(Collider* c1, Collider* c2) {
	// Needs to accept two Colliders...?
	float x = c1->GetX();
	//Notify();
	return o;
}
*/

bool ICollide::TestAABB(Collider* c1, Collider* c2) {return false;}


// CLASS PLAYER
class Player : public Collider {
public:
	Player(ICollide*);
	virtual ~Player();
	virtual void Update(Subject*);
	virtual void Draw();
};

Player::~Player() {
	_collide->Detach(this);
}

Player::Player(ICollide* c) {
	x = 50;
	y = 50;
	r = 5;
	Type_C = player;
	_collide = c;
	_collide->Attach(this);
}

void Player::Update(Subject*) {}

void Player::Draw() {}

// CLASS COMET
class Comet : public Collider {
public:
	Comet(ICollide*, int, int, int);
	virtual ~Comet();
	virtual void Update(Subject*);
	virtual void Draw();
};

Comet::~Comet() {
	_collide->Detach(this);
}

Comet::Comet(ICollide* c, int _x, int _y, int _r) {
	x = _x;
	y = _y;
	r = _r;
	
	Type_C = enemy;
	_collide = c;
	_collide->Attach(this);
}

void Comet::Update(Subject*) {}

void Comet::Draw() {}

// MAIN
int main(int argc, char* argv[]) {
	// Init Engine
	ICollide* iCollide = new ICollide();

	// First Entity (Collider)
	std::cout << "Player called w/ iCollide" << std::endl;
	Player* player = new Player(iCollide);

	Comet* comet1 = new Comet(iCollide, 200, 10, 10);
	Comet* comet2 = new Comet(iCollide, 5, 200, 12);

	player->CheckCollision();

	return 0;
}
