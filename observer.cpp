#include <iostream>
#include <list>
#include <map>

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
	void Notify();
protected:
	Subject();
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

void Subject::Notify() {
	std::list<Observer*>::iterator it;
	for (it = _observers.begin(); it != _observers.end(); ++it) {
    	Observer* o = *it;
		o->Update(this);
	}
}

class Collider;

// CLASS ICOLLIDE
class ICollide : public Subject {
public:
	virtual ~ICollide();
	Collider* FindNearest();
	bool TestRadii(Collider*, Collider*);
	bool TestAABB(Collider*, Collider*);
};


// CLASS COLLIDER
class Collider : public Observer, public Entity {
protected:
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
	// Actual collision checking done here.
	// 1. Find the nearest 2 colliders
	// 2. Check Radii regardless of distance..(Could be optimized)
	// 3. If Radii touch/overlap, test AABB.
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
    Collider* previousC, *nextC;

	std::list<Observer*>::iterator it;
	for (it = _observers.begin(); it != _observers.end(); ++it) {
		if (it == _observers.begin()) {
        	previousC = reinterpret_cast<Collider*>(*it);
			continue;
		}
    	Collider* nextC = reinterpret_cast<Collider*>(*it);
    	float x1 = previousC->GetX(), y1 = previousC->GetY(); 
		float x2 = nextC->GetX(), y2 = previousC->GetY();
		float xTemp, xClosest = 0, yTemp, yClosest = 0;

			float xOldNear = xClosest;
			float yOldNear = yClosest;
    		xTemp = c->GetX();
    		yTemp = c->GetX();
			xClosest = abs(x1 - xTemp);
			yClosest = abs(y1 - yTemp);
			if (xClosest < xOldNear) {
				xOldNear = xClosest; 
				closestC = c;
			}
			if (yClosest < yOldNear) {
				yOldNear = yClosest; 
				closestC = c;
			}
	}
	return closestC;
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
	Type_C = player;
	_collide->Detach(this);
}

Player::Player(ICollide* c) {
	x = 50;
	y = 50;
	r = 5;
	_collide = c;
	_collide->Attach(this);
}

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
	
	_collide = c;
	_collide->Attach(this);
}

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

	// Gotta figure a better way to do this.....
	// Can ICollide SOMEHOW just check for any collisions... in it's list and just push the updates
	// with one external function call.....
	player->CheckCollision();
	comet1->CheckCollision();
	comet2->CheckCollision();
	//iCollide->TestRadii(player);
	return 0;
}
