#ifndef TERM_HPP
#define TERM_HPP

#include<memory>
#include<vector>
#include<string>
#include<algorithm>
#include<iostream>
#include<exception>
#include "sub.hpp"
#include<stack>
//#include "term_iterator.hpp"

/////////////////////////////////////////////////////////////////
//
// forward declarations;
//
/////////////////////////////////////////////////////////////////

// This just makes life a lot more convenient
template<typename T>
using term_ptr = std::shared_ptr<term<T>>;

template<typename T>
using rule = std::pair<term_ptr<T>, term_ptr<T>>;

/////////////////////////////////////////////////////////////////
//
// class definitions
//
// The overall structure is pretty simple
// variable <: term
// literal  <: term
// function <: term
//
// Each term can have subterms in it.
//
/////////////////////////////////////////////////////////////////
template<typename T>
class term_iterator {
private:
	std::stack<term<T>*> _path;
	term<T> * _root;

public:
	typedef term<T>					value_type;
	typedef term<T>*				pointer;
	typedef term<T>&				reference;
	typedef size_t					size_type;
	typedef ptrdiff_t				difference_type;
	typedef std::bidirectional_iterator_tag		iterator_category;

	term_iterator<T>() = delete;
	term_iterator<T>(term<T> * n, bool begin);
	term_iterator<T>(const term_iterator<T>& i) : _path(i._path), _root(i._root) {}

	term<T>& operator*() {return *_path.top();}
	term<T>* operator->() {return &_path.top();}
	term_iterator<T>& operator++();
	term_iterator<T>& operator--();
	term_iterator<T> operator++(int)
	{
		term_iterator<T> tmp(*this);
		++this;
		return tmp;
	}

	term_iterator<T> operator--(int)
	{
		term_iterator<T> tmp(*this);
		--this;
		return tmp;
	}

	term_iterator<T>& operator+=(unsigned int n)
	{
		for(int i = 0; i < n; i++)
			++*this;
		return *this;
	}

	term_iterator<T>& operator-=(unsigned int n)
	{
		for(int i = 0; i < n; i++)
			--*this;
		return *this;
	}

	bool operator==(const term_iterator<T>& rhs) const
	{
		return _path == rhs._path;
	}

	bool operator!=(const term_iterator<T>& rhs) const
	{
		return _path != rhs._path;
	}
};

template<typename T>
term_iterator<T>& term_iterator<T>::operator++()
{    
  	if(!_path.empty())
    	{
		if(_path.top()->goRight())
        	{
            		_path.push(_path.top()->goRight());
            		while(_path.top()->goLeft())
            		{
                		_path.push(_path.top()->goLeft());
            		}
        	}
        	else
        	{
        		term<T> * child = _path.top();
            		_path.pop();
            		while(!_path.empty() && _path.top()->goRight() == child)
            		{
               			child = _path.top();
             			_path.pop();
            		}
        	}
    	}

    	return *this;
}

template<typename T>
term_iterator<T>& term_iterator<T>::operator--()
{
    	if(!_path.empty())
    	{
        	if(_path.top()->goLeft())
        	{
        		_path.push(_path.top()->goLeft());
            		while(_path.top()->goRight())
            		{
                		_path.push(_path.top()->goRight());
            		}
        	}
        	else
        	{
        		term<T> * child = _path.top();
            		_path.pop();
            		while(!_path.empty() && _path.top()->goLeft() == child)
            		{
                		child = _path.top();
                		_path.pop();
            		}
        	}
    	}

    	else
    	{
        	_path.push(_root);
        	while(_path.top()->goRight())
        	{
     	       		_path.push(_path.top()->goRight());
        	}
    	}

	return *this;
}

template<typename T>
term_iterator<T>::term_iterator(term<T> * n, bool begin)
{
	_root = n;
	if(begin)
	{
		for(; n->goLeft(); n = n->goLeft())
		{
			_path.push(n);
		}
		_path.push(n);
	}
}

template<typename T>
class term { 
public:
	typedef term<T> 				value_type;
	typedef term<T>*				pointer;
	typedef term<T>&				reference;
	typedef size_t					size_type;
	typedef ptrdiff_t				difference_type;
	typedef std::bidirectional_iterator_tag		iterator_category;
	typedef term_iterator<T>			iterator;
	typedef term_iterator<const T>			const_iterator;
	typedef std::reverse_iterator<iterator>		reverse_iterator;
	typedef std::reverse_iterator<const_iterator>	const_reverse_iterator;

	virtual void 		addArg(term_ptr<T> arg) {}
	virtual void 		display() const = 0;
	virtual T 		getValue() = 0;
	virtual std::string 	getName() {return "N/A";}
	virtual void 		setName(std::string n) {}
	
	//only function nodes can have left and right pointers
	virtual term<T>* 	goLeft() {return nullptr;}
	virtual term<T>* 	goRight() {return nullptr;}


	virtual bool 		hasChildren() {return false;}
	virtual int		numChildren() {return 0;}
	virtual term<T>* 	getChild(int num) {return nullptr;}
	virtual term_ptr<T>	getSharedChild(int num) {return nullptr;}
	term<T> * 		getSelf() {return this;}
	virtual void 		setChild(int num, term<T> * child) {};

	iterator begin() {return term_iterator<T>(this, true);}
	iterator end() {return term_iterator<T>(this, false);}
	
};

template<typename T>
class variable : public term<T>{
public:
	variable<T>(std::string n) {name = n;};
	variable(const variable<T>& rhs)
	{
		name = rhs.name;
		value = rhs.value;
	}

	variable<T>& operator= (const variable<T>& obj)
	{
		name = obj.name;
		value = obj.value;
		return *this;
	}

	//getters
	std::string getName() {return name;}
	void setName(std::string n) {name = n;}
	T getValue() { return value; }

	//Display the name of a variable.
	void display() const { std::cout << name; }

private:
	std::string name;
	T value;
};

template<typename T>
class literal : public term<T>{ 
public:
	//Check the state of value and display eithe true or false.
	void display() const 
	{ 
		if(value)
			std::cout << "True"; 
		else
			std::cout << "False"; 
	}

	T getValue() { return value; }

	literal<T>(T rhs) {value = rhs;}

	literal<T>(const literal<T>& obj)
	{
		value = obj.value;	
	}

	literal<T>& operator= (const literal<T>& obj)
	{
		value = obj.value;
		return *this;
	}

private:
	T value;
};

template<typename T>
class function : public term<T>{
public:
	term<T> * goLeft() { if(arguments[0]) return arguments[0].get(); }
	term<T> * goRight() { if(arguments[1]) return arguments[1].get(); }

	T getValue() { return value; }

	//name setter + getter
	std::string getName() {return name;}
	void setName(std::string n) {name = n;}

	//helper functions for checking if the arg vector is empty
	//or how many args it has.
	bool hasChildren() {if(!arguments.empty()) return true;}
	int numChildren() {return arguments.size();}
	
	void addArg(term_ptr<T> arg) {arguments.push_back(arg);}
	term<T> * getChild(int num) {return arguments[num].get();}
	term_ptr<T> getSharedChild(int num) {return arguments[num];}
	void setChild(int num, term<T> * newChild) 
	{
		term_ptr<T> temp(newChild);
		arguments[num] = temp;
	}

	/*displays the function, but this function also handles opening and
	closing terms with '(' and ')', as well as adding ',' between literals.
	additionally */
	void display() const 
	{ 
		if(arguments.empty())
			return;

		std::cout << name << "(";
		arguments[0]->display(); //go left

		if(arity == 2) //another argument is coming so we display comma.
			std::cout << ", ";

		if(arity == 1) //no other arguments so we can close the term.
			std::cout << ")";

		//since not all functions have two arguments, check to see if
		//there is a second argument. Alternatively we could check if
		//the arity is greater than 1.
		if(arguments[1])
		{
			arguments[1]->display(); //go right
			std::cout << ")";
		}	
	}

	function<T>(std::string n, int size) {name = n; arity = size;}
	function<T>(std::string n, int k_size, std::vector<term_ptr<T>> k)
	{
		name = n;
		arity = k_size;
		arguments = k;
	}

	function<T>(const function<T>& obj)
	{
		name = obj.name;
		value = obj.value;
		arity = obj.arity;
		arguments = obj.arguments;
	}

	function<T>& operator= (const function<T>& obj)
	{
		name = obj.name;
		arity = obj.arity;
		value = obj.value;
		arguments = obj.arguments;
		return *this;
	}
private:
	std::string name;
	int arity;
	T value;
	//arguments is a vector that will have one or two elements.
	//the first element is the left pointer and the 2nd element is
	//the right pointer
	std::vector<term_ptr<T>> arguments;
};


/////////////////////////////////////////////////////////////////
//
// unify
//
/////////////////////////////////////////////////////////////////

template<typename T>
bool unify(const term<T>& t1, const term<T>& t2, Sub<T>& sigma)
{
}

/////////////////////////////////////////////////////////////////
//
// reduce
//
/////////////////////////////////////////////////////////////////

template<typename T>
term_ptr<T> reduce(term_ptr<T> t, const std::vector<rule<T>>& rules)
{
}

/////////////////////////////////////////////////////////////////
//
// rewrite
//
/////////////////////////////////////////////////////////////////

//helper functions for rewrite
//Makes a copy of a tree given a root
template<typename T>
term<T> * copyTree(term<T>* src)
{
	term<T> * dest;
	//only functions have children
	if(!src->hasChildren())
	{
		//Literals don't have names or children
		if(src->getName() == "N/A")
		{
			return dest = new literal<T>(src->getValue());
		}
		//variables do have a name but still dont have kids
		else
		{
			return dest = new variable<T>(src->getName());
		}

	}

	if(src->hasChildren())
	{
		dest = new function<T>(src->getName(), src->numChildren());
		for(int i = 0; i < src->numChildren(); ++i)
		{
			dest->addArg(nullptr);	
		}
		if(src->numChildren() == 2)
		{
			dest->setChild(0, copyTree(src->goLeft()));
			dest->setChild(1, copyTree(src->goRight()));
			return dest;
		}
		else
			dest->setChild(0, copyTree(src->goLeft()));
			return dest;
	}
}

//delete a tree given a root
template<typename T>
void deleteTree(term<T> * root)
{
	//We are at a leaf node if no child so we can delete and return :)
	if(!root->hasChildren())
	{
		delete root;
		root = nullptr;
		return;
	}

	if(root->goLeft())
		deleteTree(root->goLeft());
	if(root->goRight())
		deleteTree(root->goRight());

	//after we have visted all the arguments of a node we can delete it and return.
	delete root;
	root = nullptr;
	return;
}

template<typename T>
std::string findKey(const Sub<T>& sigma, term<T> & rhs)
{
	//Traverse through the tree
	for(term<T> & t: rhs)
	{
		//Traverse through the nodes areguments
		if(t.hasChildren())
		{
			for(int i = 0; i < t.numChildren(); ++i)
			{
				std::string compare = t.getChild(i)->getName();
				if(sigma.contains(compare))
				{
					return compare;
				}
			}
		}
	}
}

template<typename T>
term_ptr<T> rewrite(term_ptr<T> t, term<T>& rhs, std::vector<int> path, const Sub<T>& sigma)
{
	std::string key = findKey(sigma, rhs);
	term<T> * sigmacopy = nullptr; 
	term<T> * rhscopy = nullptr;
	sigmacopy = copyTree(sigma(key).getSelf());
	rhscopy = copyTree(rhs.getSelf());

	//Traverse through the tree
	for(term<T> & t: *rhscopy)
	{
		//Traverse through the nodes areguments
		if(t.hasChildren())
		{
			for(int i = 0; i < t.numChildren(); ++i)
			{
				if(sigma.contains(key))
				{
					t.setChild(i, sigmacopy);

				}
			}
		}
	}

	//Traverse through t to the point where we can cut off the bracnh
	//and replace it with rhs. Path does not start at zero, but our
	//arguments vector is so we need to i-1 for the proper index.
	term_ptr<T>temp(rhscopy);
	if(path.empty())
	{
		t = temp;
	}
	else
	{
		for(int i : path)
		{
			t = t->getSharedChild(i-1);
		}
		t = temp;
	}
	return t;
}

/////////////////////////////////////////////////////////////////
//
// operator<<
//
/////////////////////////////////////////////////////////////////

template<typename T>
std::ostream& operator<<(std::ostream& out, const term<T>& t)
{
	t.display();
	//not sure why I need this but it segfaults without adding the endl \(O_O)/
	//Only thing I can think of is the output stream overflows?
	std::cout<<std::endl;
}


#endif // TERM_HPP
